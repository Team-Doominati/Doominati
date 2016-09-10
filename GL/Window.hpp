//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Graphics Library environment.
//
//-----------------------------------------------------------------------------

#ifndef Doom__GL__Window_H__
#define Doom__GL__Window_H__

#include "GL/Color.hpp"
#include "GL/Particle.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace GL
   {
      class Shader;

      //
      // Window
      //

      class Window
      {
      public:
         Window(int w, int h);
         ~Window();

         void renderBegin();
         void renderEnd();
         
         void drawColorSet(float r, float g, float b, float a = 1.0f);
         void drawColorSet(Color const &col);
         Color drawColorGet();
         
         void drawLine(int x1, int y1, int x2, int y2);
         void drawRectangle(int x1, int y1, int x2, int y2, float rot = 0, bool line = false);
         void drawParticleSystem(ParticleSystem const &ps);

         void shaderSwap(Shader *sp);
         void shaderDrop();
         void shaderUpdate();

         int realw, realh;
         int w, h;

      private:
         class PrivData;

         void resize(int w, int h);

         float cr, cg, cb, ca;

         std::unique_ptr<PrivData> privdata;

         std::unique_ptr<Shader> shaderBase;
         Shader *shaderCurrent;
      };
   }
}

#endif//Doom__GL__Window_H__

