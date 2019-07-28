//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Program entry point.
//
//-----------------------------------------------------------------------------

#if DGE_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#define DGE_doominati_InitMain() SDL_SetMainReady()
#else
#define DGE_doominati_InitMain()
#endif

#include "AL/Renderer.hpp"

#include "Code/Callback.hpp"
#include "Code/Codedefs.hpp"
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Textdefs.hpp"
#include "Code/Thread.hpp"

#include "Core/NTS.hpp"
#include "Core/Time.hpp"

#include "Defs/Gamedefs.hpp"

#include "FS/Dir.hpp"

#include "GL/Defs.hpp"
#include "GL/Renderer.hpp"
#include "GL/Window.hpp"

#include "Game/Defs.hpp"
#include "Game/Input.hpp"
#include "Game/Thinker.hpp"

#include "Sh/Command.hpp"
#include "Sh/Shell.hpp"
#include "Sh/StdIn.hpp"

#include <GDCC/Core/Exception.hpp>
#include <GDCC/Core/Option.hpp>

#include <GDCC/Option/Bool.hpp>

#include "SDL.h"

#include <iostream>
#include <cstdlib>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static DGE::Code::CallbackSet<void()> InitCB{"Init"};

static GDCC::Core::Array<std::unique_ptr<DGE::FS::Dir>> RootDirs;

static bool WindowEnable = true;
static char const *WindowTitle = nullptr;
static int WindowSizeX = 640;
static int WindowSizeY = 480;

//
// GL::Window::Enable
//
static DGE::Defs::GamedefsCall WindowEnableDef
{
   &DGE::GL::GetWindowDefs(), "Enable",
   [](DGE::Defs::GamedefsParserValue const *value)
   {
      if(value->data.size() != 1)
         std::cerr << "GL::Window::Enable takes 1 value\n", throw EXIT_FAILURE;

      WindowEnable = DGE::Defs::GamedefsParser::GetBool(value->data[0]);
   }
};

//
// GL::Window::Title
//
static DGE::Defs::GamedefsCall WindowTitleDef
{
   &DGE::GL::GetWindowDefs(), "Title",
   [](DGE::Defs::GamedefsParserValue const *value)
   {
      if(value->data.size() != 1)
         std::cerr << "GL::Window::Title takes 1 value\n", throw EXIT_FAILURE;

      WindowTitle = value->data[0];
   }
};

//
// GL::Window::Size
//
static DGE::Defs::GamedefsCall WindowSizeDef
{
   &DGE::GL::GetWindowDefs(), "Size",
   [](DGE::Defs::GamedefsParserValue const *value)
   {
      if(value->data.size() != 2)
         std::cerr << "GL::Window::Size takes 2 values\n", throw EXIT_FAILURE;

      // TODO: Replace with shared number parser in DGE::Defs.
      WindowSizeX = std::strtoul(value->data[0], nullptr, 16);
      WindowSizeY = std::strtoul(value->data[1], nullptr, 16);
   }
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// LoadCodedefs
//
static void LoadCodedefs(DGE::Code::Program *prog)
{
   // Finish internal loader data.
   DGE::Game::NativeAdd();
   DGE::GL::NativeAdd();

   DGE::Code::NativeAdder::Finish();
   DGE::Game::Object::ObjectTypeAdder::Finish();

   // Load CODEDEFS files.
   DGE::Code::Loader loader;
   DGE::FS::Dir::Root->forFilePath("boot/codedefs",
      std::bind(&DGE::Code::Loader::loadCodedefs, &loader, std::placeholders::_1));

   // Print status.
   if(loader.loadPASS)
   {
      char const *s = (loader.loadPASS != 1) ? "s" : "";
      std::cerr << "Loaded " << loader.loadPASS << " codedef" << s << ".\n";
   }

   if(loader.loadFAIL)
   {
      char const *s = (loader.loadFAIL != 1) ? "s" : "";
      std::cerr << "Encountered " << loader.loadFAIL <<
         " codedefs error" << s << ".\n";
      throw EXIT_FAILURE;
   }

   // Finish external loader data.
   for(auto &em : DGE::Code::ExtMems) em.val->finish();

   // Process loaded CODEDEFS.
   loader.gen(prog);
}

//
// LoadGamedefs
//
static void LoadGamedefs()
{
   // Load GAMEDEFS files.
   DGE::FS::Dir::Root->forFilePath("boot/gamedefs", [](DGE::FS::File *file)
   {
      if(file->format != DGE::FS::Format::DGE_NTS)
         return;

      DGE::Core::NTSArray       arr{file->data, file->size};
      DGE::Core::NTSStream      str{arr};
      DGE::Defs::GamedefsParser in {str};

      try
      {
         while(auto data = in.get())
            DGE::Defs::Gamedefs::Root.process(data.get());
      }
      catch(GDCC::Core::Exception const &e)
      {
         std::cerr << "GAMEDEFS error in '" << file->name << "': " << e.what() << '\n';
      }
   });
}

//
// LoadTextdefs
//
static void LoadTextdefs()
{
   DGE::Defs::TextdefsCompiler comp;

   // Load TEXTDEFS files.
   DGE::FS::Dir::Root->forFilePath("boot/textdefs", [&comp](DGE::FS::File *file)
   {
      if(file->format != DGE::FS::Format::DGE_NTS)
         return;

      // The contents of this file are used as-is, so hold a reference.
      ++file->refs;

      DGE::Core::NTSArray       arr{file->data, file->size};
      DGE::Core::NTSStream      str{arr};
      DGE::Defs::TextdefsParser in {str};

      try
      {
         while(in)
            comp.add(in.getLang());
      }
      catch(GDCC::Core::Exception const &e)
      {
         std::cerr << "TEXTDEFS error in '" << file->name << "': " << e.what() << '\n';
      }
   });

   // Compile TEXTDEFS.
   comp.finish(DGE::Code::Textdefs);

   // Finish TEXTDEFS.
   DGE::Code::TextdefsFinish();
}

//
// Main
//
static int Main()
{
   // Initialize SDL.
   class SDLHandler
   {
   public:
      SDLHandler()
      {
         if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
         {
            std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
            throw EXIT_FAILURE;
         }
      }

      ~SDLHandler()
      {
         SDL_Quit();
      }
   };

   SDLHandler sdl;

   // Initialize filesystem.
   if(GDCC::Core::GetOptionArgs().size())
   {
      std::vector<std::unique_ptr<DGE::FS::Dir>> dirs;
      for(auto const &arg : GDCC::Core::GetOptionArgs())
         if(auto dir = DGE::FS::CreateDir(arg))
            dirs.emplace_back(std::move(dir));

      if(DGE::FS::Dir::Extra)
         dirs.emplace_back(DGE::FS::CreateDir_Extra());

      if(dirs.size() != 1)
      {
         RootDirs = {GDCC::Core::Move, dirs.rbegin(), dirs.rend()};

         DGE::FS::Dir::Root = DGE::FS::CreateDir_Union(
            {RootDirs.begin(), RootDirs.end(),
               [](std::unique_ptr<DGE::FS::Dir> &d){return d.get();}});
      }
      else
         DGE::FS::Dir::Root = std::move(dirs[0]);
   }
   else
      DGE::FS::Dir::Root = DGE::FS::CreateDir(".");

   // Load GAMEDEFS.
   LoadGamedefs();

   // Load TEXTDEFS.
   LoadTextdefs();

   // Initialize shell.
   DGE::Sh::StdIn_Init();
   DGE::Sh::CommandAdder::Finish();
   DGE::Sh::Shell shell;

   // Initialize rendering.
   std::unique_ptr<DGE::GL::Window>   window;
   std::unique_ptr<DGE::GL::Renderer> renderer;
   if(WindowEnable)
   {
      window.reset(new DGE::GL::Window{WindowSizeX, WindowSizeY});
      renderer.reset(new DGE::GL::Renderer{*window});

      DGE::GL::Window::SetCurrent(window.get());
      DGE::GL::Renderer::SetCurrent(renderer.get());

      if(WindowTitle)
         window->setTitle(WindowTitle);
   }

   // Initialize audio.
   DGE::AL::Renderer audio;

   DGE::AL::Renderer::SetCurrent(&audio);

   // Initialize input.
   DGE::Game::InputSource_Local input;

   DGE::Game::InputSource_Local::SetCurrent(&input);
   DGE::Game::InputSource::Set(0, &input);

   // Initialize scripting.
   DGE::Code::Program prog;
   LoadCodedefs(&prog);
   DGE::Code::Process proc{&prog};

   // Call Init functions.
   InitCB();

   // Call main.
   if(auto func = prog.funcs.find("main"))
      proc.mainThread()->startTask(func, nullptr, 0);

   proc.exec();

   // Run main loop.
   std::size_t timeLast = DGE::Core::GetTicks<DGE::Core::PlayTick<>>();
   std::size_t timeNext;

   for(;;)
   {
      std::size_t timeDelta;

      timeNext  = DGE::Core::GetTicks<DGE::Core::PlayTick<>>();
      timeDelta = timeNext - timeLast;
      timeLast  = timeNext;

      DGE::Game::PumpEvents();

      if(timeDelta)
      {
         while(timeDelta--)
         {
            shell.exec();
            input.poll();
            proc.exec();
            DGE::Game::EventFrame();
         }
      }
      else
         SDL_Delay(1);

      // Rendering actions.
      if(renderer)
      {
         renderer->renderBegin();
         renderer->render();
         renderer->renderEnd();
      }
   }

   return EXIT_SUCCESS;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

//
// main
//
int main(int argc, char **argv)
{
   DGE_doominati_InitMain();

   auto &opts = GDCC::Core::GetOptions();

   // Don't want an output option.
   opts.optOutput.remove();

   opts.list.name     = "doominati";
   opts.list.nameFull = "Doominati";

   opts.list.version = "v0.0.0";

   opts.list.usage = "[option]...";

   opts.list.descS = "Doominati game engine.";

   try
   {
      if(argc > 1)
         GDCC::Core::ProcessOptions(opts, argc, argv, false);

      std::cout <<
         " Doominati " << opts.list.version <<
         " Copyright (C) 2017-2019 Team Doominati\n"
         "      See COPYING for license information.\n\n";

      return Main();
   }
   catch(std::exception const &e)
   {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   catch(int e)
   {
      return e;
   }
}

// EOF

