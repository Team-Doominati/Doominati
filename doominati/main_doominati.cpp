//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Program entry point.
//
//-----------------------------------------------------------------------------

#include "Code/Codedefs.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Thread.hpp"

#include "FS/Dir.hpp"

#include "GL/Window.hpp"

#include "Game/Input.hpp"

#include <GDCC/Core/Option.hpp>

#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "SDL.h"
#include "GL/gl_2_1.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

using PlayTick = std::chrono::duration<std::size_t, std::ratio<1, 50>>;
using Millisecond = std::chrono::duration<double, std::ratio<1, 1000>>;


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static Doom::GL::Window *WindowCurrent;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// GetTicks
//

template<typename T>
static decltype(T().count()) GetTicks()
{
   static std::chrono::time_point<std::chrono::steady_clock> base;

   if(base == std::chrono::time_point<std::chrono::steady_clock>())
      base = std::chrono::steady_clock::now();

   return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - base).count();
}

//
// DrawTest
//
// Draws test pattern.
//

static void DrawTest()
{
   auto xl = WindowCurrent->xl;
   auto xh = WindowCurrent->xh;
   auto yl = WindowCurrent->yl;
   auto yh = WindowCurrent->yh;

   glBegin(GL_LINES);

   WindowCurrent->setDrawColor(0.0f, 0.0f, 1.0f);

   glVertex2f(xl / 2.0f, yl / 2.0f);
   glVertex2f(xl / 2.0f, yh / 2.0f);

   glVertex2f(xl / 2.0f, yh / 2.0f);
   glVertex2f(xh / 2.0f, yh / 2.0f);

   glVertex2f(xh / 2.0f, yh / 2.0f);
   glVertex2f(xh / 2.0f, yl / 2.0f);

   glVertex2f(xh / 2.0f, yl / 2.0f);
   glVertex2f(xl / 2.0f, yl / 2.0f);

   WindowCurrent->setDrawColor(1.0f, 0.0f, 0.0f);

   glVertex2f(xl / 3.0f, yl / 3.0f);
   glVertex2f(xh / 3.0f, yh / 3.0f);

   glVertex2f(xl / 3.0f, yh / 3.0f);
   glVertex2f(xh / 3.0f, yl / 3.0f);

   WindowCurrent->setDrawColor(0.0f, 1.0f, 0.0f);

   double s = std::sin(GetTicks<Millisecond>() / 1000.0) * 40.0;
   double c = std::cos(GetTicks<Millisecond>() / 1000.0) * 40.0;

   glVertex2f(-100.0f + s, -100.0f + c);
   glVertex2f(+100.0f + s, +100.0f + c);

   glVertex2f(-100.0f + s, +100.0f + c);
   glVertex2f(+100.0f + s, -100.0f + c);

   glEnd();
}

//
// LoadCodedefs
//
static void LoadCodedefs(Doom::Code::Program *prog)
{
   Doom::Code::Loader loader;
   Doom::FS::Dir::ForFile("codedefs",
      std::bind(&Doom::Code::Loader::loadCodedefs, &loader, std::placeholders::_1));

   std::cerr << "Loaded " << loader.loadPASS << " codedefs.\n";

   if(loader.loadFAIL)
   {
      std::cerr << "Encountered " << loader.loadFAIL << " codedefs errors.\n";
      throw EXIT_FAILURE;
   }

   loader.gen(prog);
}

//
// Main
//

static int Main()
{
   if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
   {
      std::cerr << "SDL_Init: " << SDL_GetError() << '\n';
      throw EXIT_FAILURE;
   }

   std::atexit(SDL_Quit);

   Doom::GL::Window window{640, 480};
   WindowCurrent = &window;

   Doom::Game::InputSource_Local input;

   Doom::FS::Dir::AddRoot("."); // HACK

   if(auto file = Doom::FS::Dir::FindFile("startmsg"))
      std::cout << file->data << std::endl; // HACK

   // Initialize scripting and call main.
   Doom::Code::Program prog;
   LoadCodedefs(&prog);
   Doom::Code::Process proc{&prog};

   if(auto func = prog.funcs.find("main"))
      proc.threads.next->obj->startTask(func, nullptr, 0);

   std::size_t timeLast = GetTicks<PlayTick>();
   std::size_t timeNext;

   for(;;)
   {
      std::size_t timeDelta;

      timeNext  = GetTicks<PlayTick>();
      timeDelta = timeNext - timeLast;
      timeLast  = timeNext;

      if(!timeDelta)
         SDL_Delay(1);

      else while(timeDelta--)
      {
         // Playsim actions.
         input.poll();
         proc.exec();
      }

      // Rendering actions.

      window.renderBegin();

      DrawTest();

      window.renderEnd();
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
         "Doominati v0.0.0 -- Copyright (C) Team Doominati 2016\n"
         "See COPYING for license information.\n\n";

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

