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
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Thread.hpp"

#include "Core/Time.hpp"
#include "Core/Math.hpp"

#include "FS/Dir.hpp"

#include "GL/Window.hpp"
#include "GL/Particle.hpp"

#include "Game/Input.hpp"

#include <GDCC/Core/Option.hpp>

#include <iostream>
#include <cstdlib>

#include "SDL.h"


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static Doom::GL::Window *WindowCurrent;
static Doom::GL::ParticleSystem ParticleSystem{1280/2, 720/2, 1000};


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
   auto w = WindowCurrent->w;
   auto h = WindowCurrent->h;

   auto xo = w / 3.0f;
   auto yo = h / 3.0f;

   auto xp = w - xo;
   auto yp = h - yo;

   double seconds = Doom::Core::GetTicks<Doom::Core::Second<double>>();

   WindowCurrent->drawParticleSystem(ParticleSystem);

   WindowCurrent->drawColorSet(Doom::GL::Color::FromHSV(std::abs(std::sin(seconds)), 1.0, 1.0));

   WindowCurrent->drawRectangle(xp, yp, xo, yo, 0, true);
   WindowCurrent->drawRectangle(w - 102, h - 102, w - 2, h - 2);
   WindowCurrent->drawRectangle(2, 2, 102, 102);

   WindowCurrent->drawColorSet(Doom::GL::Color::Red);

   WindowCurrent->drawLine(xp, yp, xo, yo);
   WindowCurrent->drawLine(xp, yo, xo, yp);

   WindowCurrent->drawColorSet(Doom::GL::Color::Green);

   double s = std::sin(seconds * 4.0) * 40.0;
   double c = std::cos(seconds * 4.0) * 40.0;

   WindowCurrent->drawLine(xp + s, yp + c, xo + s, yo + c);
   WindowCurrent->drawLine(xp + s, yo + c, xo + s, yp + c);
}

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

   if(digit.seg0) WindowCurrent->drawLine(xl, yh, xh, yh);
   if(digit.seg1) WindowCurrent->drawLine(xl, ym, xl, yh);
   if(digit.seg2) WindowCurrent->drawLine(xh, ym, xh, yh);
   if(digit.seg3) WindowCurrent->drawLine(xl, ym, xh, ym);
   if(digit.seg4) WindowCurrent->drawLine(xl, yl, xl, ym);
   if(digit.seg5) WindowCurrent->drawLine(xh, yl, xh, ym);
   if(digit.seg6) WindowCurrent->drawLine(xl, yl, xh, yl);
}

//
// DrawFPS
//
static void DrawFPS()
{
   static double timeLast = 0;
   static double timeMean = 1/50.0;
   double timeThis, timePass;

   timeThis = Doom::Core::GetTicks<Doom::Core::Second<double>>();
   timePass = timeThis - timeLast;
   timeLast = timeThis;
   timeMean = (timeMean * 19 + timePass) / 20;

   unsigned int fps = std::round(1 / timeMean);

   WindowCurrent->drawColorSet(Doom::GL::Color::White);

   int x = WindowCurrent->w - 65;
   int y = 35;

   if(fps > 999) fps = 999;

   DrawDigit(fps / 100 % 10, x +  0, y, x + 15, y - 25);
   DrawDigit(fps /  10 % 10, x + 20, y, x + 35, y - 25);
   DrawDigit(fps /   1 % 10, x + 40, y, x + 55, y - 25);
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
   Doom::Code::NativeAdder::Finish();
   Doom::Code::Program prog;
   LoadCodedefs(&prog);
   Doom::Code::Process proc{&prog};

   if(auto func = prog.funcs.find("main"))
      proc.threads.next->obj->startTask(func, nullptr, 0);

   std::size_t timeLast = Doom::Core::GetTicks<Doom::Core::PlayTick<>>();
   std::size_t timeNext;

   for(;;)
   {
      std::size_t timeDelta;

      timeNext  = Doom::Core::GetTicks<Doom::Core::PlayTick<>>();
      timeDelta = timeNext - timeLast;
      timeLast  = timeNext;

      if(!timeDelta)
         SDL_Delay(1);
      else
      {
         while(timeDelta--)
         {
            // Playsim actions.

            for(;;)
            {
               float fuck3 = (rand() % 1280) - (1280/2);
               float fuck4 = (rand() % 720 ) - (720/2);

               Doom::GL::Particle *test = ParticleSystem.create();

               if(test == nullptr)
                  break;

               test->life = (rand() % 30) + 10;
               test->oldposition.x = test->position.x = -30.0f + fuck3;
               test->oldposition.y = test->position.y = -30.0f + fuck4;
               test->velocity.x = (1.0f/4096)*((rand()%255)-128);
               test->velocity.y = (1.0f/4096)*((rand()%255)-128);
               test->acceleration.x = (1.0f/16384)*((rand()%255)-128);
               test->acceleration.y = (1.0f/16384)*((rand()%255)-128);
               test->scale.x = test->scale.y = (1.0f/4096)*((rand()%255)-128)*40;
               test->color = Doom::GL::Color::Pink;
               test->colordest = Doom::GL::Color::Red;
               test->colordest.a = 0.0f;
               test->colorspeed = 0.04f;
               test->rotspeed = (1.0f/4096)*((rand()%255)-128);
            }

            input.poll();
            proc.exec();
            ParticleSystem.update();
         }
      }

      // Rendering actions.

      window.renderBegin();

      DrawTest();

      DrawFPS();

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

