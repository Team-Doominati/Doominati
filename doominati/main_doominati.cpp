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

#include "FS/Dir.hpp"

#include "GL/Window.hpp"

#include "Game/Input.hpp"

#include <GDCC/Core/Option.hpp>

#include <chrono>
#include <iostream>

#include "SDL.h"
#include "SDL_opengl.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

using PlayTick = std::chrono::duration<std::size_t, std::ratio<1, 50>>;


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static Doom::GL::Window *WindowCurrent;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

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

   glColor3f(1.0f, 1.0f, 1.0f);

   glVertex2f(xl / 2.0f, yl / 2.0f);
   glVertex2f(xl / 2.0f, yh / 2.0f);

   glVertex2f(xl / 2.0f, yh / 2.0f);
   glVertex2f(xh / 2.0f, yh / 2.0f);

   glVertex2f(xh / 2.0f, yh / 2.0f);
   glVertex2f(xh / 2.0f, yl / 2.0f);

   glVertex2f(xh / 2.0f, yl / 2.0f);
   glVertex2f(xl / 2.0f, yl / 2.0f);

   glColor3f(1.0f, 0.0f, 0.0f);

   glVertex2f(xl / 3.0f, yl / 3.0f);
   glVertex2f(xh / 3.0f, yh / 3.0f);

   glVertex2f(xl / 3.0f, yh / 3.0f);
   glVertex2f(xh / 3.0f, yl / 3.0f);

   glColor3f(0.0f, 1.0f, 0.0f);

   glVertex2f(-100.0f, -100.0f);
   glVertex2f(+100.0f, +100.0f);

   glVertex2f(-100.0f, +100.0f);
   glVertex2f(+100.0f, -100.0f);

   glEnd();
}

//
// GetTicks
//
template<typename T> static auto GetTicks() -> decltype(T().count())
{
   static std::chrono::time_point<std::chrono::steady_clock> base;

   if(base == std::chrono::time_point<std::chrono::steady_clock>())
      base = std::chrono::steady_clock::now();

   return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - base).count();
}

//
// Main
//
static int Main()
{
   struct CoreEnv
   {
      CoreEnv() {if(SDL_Init(0)) {std::cerr << "SDL_Init:" << SDL_GetError() << '\n'; throw EXIT_FAILURE;}}
      ~CoreEnv() {SDL_Quit();}
   } envCore;

   Doom::GL::Window window{640, 480};
   WindowCurrent = &window;

   Doom::Game::InputSource_Local input;

   Doom::FS::Dir::AddRoot(".");

   if(auto file = Doom::FS::Dir::FindFile("startmsg"))
      std::cout << file->data << std::endl;

   for(std::size_t timeLast = GetTicks<PlayTick>(), timeNext;; timeLast = timeNext)
   {
      timeNext = GetTicks<PlayTick>();

      for(std::size_t timePass = timeNext - timeLast; timePass--;)
      {
         // Playsim actions.

         input.poll();

         auto const &inputNext = input.getNext();
         auto const &inputLast = input.getLast();

         if(inputNext.menuClose)
            return EXIT_SUCCESS;

         if(inputNext.use1 && !inputLast.use1)
         {
            auto rx = inputNext.pos.x;
            auto ry = inputNext.pos.y;

            std::cout << "rx: " << rx << " ry: " << ry << std::endl;
         }
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

