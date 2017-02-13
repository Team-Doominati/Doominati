//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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

#include "Code/Codedefs.hpp"
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Thread.hpp"

#include "Core/Time.hpp"
#include "Core/Math.hpp"

#include "FS/Dir.hpp"

#include "GL/Particle.hpp"
#include "GL/Shader.hpp"
#include "GL/Window.hpp"
#include "GL/Renderer.hpp"

#include "Game/Input.hpp"

#include <GDCC/Core/Option.hpp>

#include <GDCC/Option/Bool.hpp>

#include "SDL.h"

#include <iostream>
#include <cstdlib>


//----------------------------------------------------------------------------|
// Options                                                                    |
//

//
// --fps-counter
//
static bool FpsCounterOn = false;
static GDCC::Option::Bool FpsCounterOpt{
   &GDCC::Core::GetOptionList(),
   GDCC::Option::Base::Info()
      .setName("fps-counter")
      .setDescS("Enables a simple frames per second counter."),
   &FpsCounterOn
};


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game {void NativeAdd();}
namespace DGE::GL   {void NativeAdd();}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// DrawDigit
//
static void DrawDigit(unsigned int dig, int xl, int yl, int xh, int yh)
{
   int ym = (yl + yh) / 2;

   static struct
   {
      bool seg0 : 1, seg1 : 1, seg2 : 1, seg3 : 1, seg4 : 1, seg5 : 1, seg6 : 1;
   }
   const digtab[10]
   {
      {1,1,1,0,1,1,1},
      {0,0,1,0,0,1,0},
      {1,0,1,1,1,0,1},
      {1,0,1,1,0,1,1},
      {0,1,1,1,0,1,0},
      {1,1,0,1,0,1,1},
      {1,1,0,1,1,1,1},
      {1,0,1,0,0,1,0},
      {1,1,1,1,1,1,1},
      {1,1,1,1,0,1,1},
   };

   auto &digit = digtab[dig];

   //  -0-
   // |   |
   // 1   2
   // |   |
   //  -3-
   // |   |
   // 4   5
   // |   |
   //  -6-

   auto renderer = DGE::GL::Renderer::GetCurrent();

   renderer->lineSmooth(true);
   renderer->lineWidth(2);
   if(digit.seg0) renderer->drawLine(xl, yh, xh, yh);
   if(digit.seg1) renderer->drawLine(xl, ym, xl, yh);
   if(digit.seg2) renderer->drawLine(xh, ym, xh, yh);
   if(digit.seg3) renderer->drawLine(xl, ym, xh, ym);
   if(digit.seg4) renderer->drawLine(xl, yl, xl, ym);
   if(digit.seg5) renderer->drawLine(xh, yl, xh, ym);
   if(digit.seg6) renderer->drawLine(xl, yl, xh, yl);
   renderer->lineSmooth(false);
   renderer->lineWidth(1);
}

//
// DrawFPS
//
static void DrawFPS()
{
   static double timeLast = 0;
   static double timeMean = 1/50.0;
   double timeThis, timePass;

   timeThis = DGE::Core::GetTicks<DGE::Core::Second<double>>();
   timePass = timeThis - timeLast;
   timeLast = timeThis;
   timeMean = (timeMean * 19 + timePass) / 20;

   unsigned int fps = std::round(1 / timeMean);

   DGE::GL::Renderer::GetCurrent()->drawColorSet(DGE::GL::GetColor("White"));
   DGE::GL::Renderer::GetCurrent()->textureUnbind();

   if(fps > 999) fps = 999;

   DrawDigit(fps / 100 % 10,  0, 25, 15, 0);
   DrawDigit(fps /  10 % 10, 20, 25, 35, 0);
   DrawDigit(fps /   1 % 10, 40, 25, 55, 0);
}

//
// LoadCodedefs
//
static void LoadCodedefs(DGE::Code::Program *prog)
{
   DGE::Code::Loader loader;
   DGE::FS::Dir::ForFile("codedefs",
      std::bind(&DGE::Code::Loader::loadCodedefs, &loader, std::placeholders::_1));

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

   loader.gen(prog);
}

//
// Main
//
static int Main()
{
   // Initialize SDL.
   if(SDL_Init(
      SDL_INIT_VIDEO |
      SDL_INIT_JOYSTICK |
      SDL_INIT_GAMECONTROLLER |
      SDL_INIT_EVENTS) != 0)
   {
      std::cerr << "SDL_Init: " << SDL_GetError() << '\n';
      throw EXIT_FAILURE;
   }

   // Initialize filesystem.
   if(GDCC::Core::GetOptionArgs().size())
   {
      for(auto const &arg : GDCC::Core::GetOptionArgs())
         DGE::FS::Dir::AddRoot(arg);
   }
   else
      DGE::FS::Dir::AddRoot(".");

   // Initialize rendering.
   DGE::GL::Window window{640, 480};
   DGE::GL::Renderer renderer{window};

   DGE::GL::Window::SetCurrent(&window);
   DGE::GL::Renderer::SetCurrent(&renderer);

   // Initialize input.
   DGE::Game::InputSource_Local input;

   DGE::Game::InputSource::SetCurrent(&input);

   // Initialize scripting and call main.
   DGE::Game::NativeAdd();
   DGE::GL::NativeAdd();

   DGE::Code::NativeAdder::Finish();

   DGE::Code::Program prog;
   LoadCodedefs(&prog);
   DGE::Code::Process proc{&prog};

   if(auto func = prog.funcs.find("main"))
      proc.mainThread()->startTask(func, nullptr, 0);

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
            input.poll();
            proc.exec();
            DGE::Game::EventFrame();
         }
      }
      else
         SDL_Delay(1);

      // Rendering actions.
      renderer.renderBegin();
      if(FpsCounterOn) DrawFPS();
      renderer.renderEnd();
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
         " Copyright (C) 2017 Team Doominati\n"
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

