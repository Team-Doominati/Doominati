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

#include "GL/Particle.hpp"
#include "GL/Shader.hpp"
#include "GL/Window.hpp"

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
   static char const *shaderFragment = R"(
      #version 120

      uniform int dge_ticks;
      uniform int dge_mseconds;
      uniform int dge_seconds;

      vec4 HSVToRGB(float h, float s, float v, float a)
      {
         h *= 360.0;

         float chroma = v * s;
         float hp = h / 60.0;
         float x = chroma * (1 - abs(mod(hp, 2) - 1));
         float r1 = 0, g1 = 0, b1 = 0;

              if(0 <= hp && hp < 1) r1 = chroma, g1 = x,      b1 = 0;
         else if(1 <= hp && hp < 2) r1 = x,      g1 = chroma, b1 = 0;
         else if(2 <= hp && hp < 3) r1 = 0,      g1 = chroma, b1 = x;
         else if(3 <= hp && hp < 4) r1 = 0,      g1 = x,      b1 = chroma;
         else if(4 <= hp && hp < 5) r1 = x,      g1 = 0,      b1 = chroma;
         else if(5 <= hp && hp < 6) r1 = chroma, g1 = 0,      b1 = x;

         float m = v - chroma;
         return vec4(r1 + m, g1 + m, b1 + m, a);
      }

      void main(void)
      {
         const float pi = 3.1415926535897932384626433832795;

         float t = float(dge_mseconds) / 32.0;
         vec2 uv = (gl_FragCoord.xy / vec2(640.0, 480.0)) * vec2(256.0, 224.0);

         float bx = uv.x + 0.5 * sin(t / 8.0);
         float by = uv.y + 0.5 * cos(t / 8.0);

         float v;

         v  = sin((-bx + t) / 32.0);
         v += cos((by + t) / 32.0);
         v += sin((bx + by + t) / 32.0);
         v += sin((sqrt(pow(bx + sin(t / 3.0), 2.0) + pow(by + cos(t / 2.0), 2.0) + 1.0) + t) / 128.0);

         gl_FragColor = HSVToRGB(
            sin(v * pi) * 0.5 + 0.5,
            sin(v * pi/2.0) * 0.5 + 0.5,
            sin(v * pi/4.0) * 0.5 + 0.5,
            1.0);
      }
   )";

   static char const *shaderVertex = R"(
      #version 120

      void main(void)
      {
         gl_Position = ftransform();
      }
   )";


   static Doom::GL::Shader *shader;

   if(!shader)
      shader = new Doom::GL::Shader{shaderFragment, shaderVertex};

   auto w = WindowCurrent->w;
   auto h = WindowCurrent->h;

   auto xo = w / 3.0f;
   auto yo = h / 3.0f;

   auto xp = w - xo;
   auto yp = h - yo;

   double seconds = Doom::Core::GetTicks<Doom::Core::Second<double>>();

   WindowCurrent->drawParticleSystem(ParticleSystem);

   WindowCurrent->drawColorSet(Doom::GL::Color::FromHSV(std::sin(seconds) * 0.5f + 0.5f, 1.0, 1.0));

   WindowCurrent->drawRectangle(xp, yp, xo, yo, 0, true);

   WindowCurrent->shaderSwap(shader);
   WindowCurrent->shaderUpdate();
   WindowCurrent->drawRectangle(2, 2, 302, 302);
   WindowCurrent->shaderDrop();

   WindowCurrent->textureSet("test.ppm");
   WindowCurrent->drawRectangle(w - 102, h - 102, w - 2, h - 2);

   WindowCurrent->textureSet("test3.ppm");

   WindowCurrent->drawColorSet(Doom::GL::Color::White);
   WindowCurrent->drawRectangle(303, 2, 603, 302);

   WindowCurrent->textureUnbind();

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

