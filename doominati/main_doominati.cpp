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
static Doom::GL::ParticleSystem ParticleSystem{100, 100};


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
   
   double seconds = Doom::Core::GetTicks<Doom::Core::Second>();
   
   WindowCurrent->drawParticleSystem(ParticleSystem);
   
   WindowCurrent->drawColorSet(Doom::GL::Color::FromHSV(std::abs(std::sin(seconds)), 1.0, 1.0));

   WindowCurrent->drawRectangle(xp, yp, xo, yo, true);
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

   std::size_t timeLast = Doom::Core::GetTicks<Doom::Core::PlayTick>();
   std::size_t timeNext;

   for(;;)
   {
      std::size_t timeDelta;

      timeNext  = Doom::Core::GetTicks<Doom::Core::PlayTick>();
      timeDelta = timeNext - timeLast;
      timeLast  = timeNext;

      if(!timeDelta)
         SDL_Delay(1);
      else
      {
         while(timeDelta--)
         {
            // Playsim actions.
            
            for(int i = 0; i < 120; i++)
            {
               float fuck3 = (rand() % 200) - 100;
               float fuck4 = (rand() % 200) - 100;
               
               Doom::GL::Particle test{};
               
               test.life = 50;
               test.oldposition = test.position = Doom::Core::Vector2{-30.f + fuck3, -30.f + fuck4};
               test.velocity = Doom::Core::Vector2{(1.f/4096)*((rand()%255)-128),(1.f/4096)*((rand()%255)-128)};
               test.acceleration = Doom::Core::Vector2{(1.f/16384)*((rand()%255)-128),(1.f/16384)*((rand()%255)-128)};
               test.color = Doom::GL::Color::Pink;
               test.colordest = Doom::GL::Color::Red;
               test.colordest.a = 0.5f;
               test.colorspeed = 0.04f;
               
               ParticleSystem.particles.emplace_back(std::move(test));
            }
            
            input.poll();
            proc.exec();
            ParticleSystem.update();
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

