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

         // render
         void renderBegin();
         void renderEnd();

         // drawColor
         void drawColorSet(float r, float g, float b, float a = 1.0f);
         void drawColorSet(Color const &col);
         Color drawColorGet() const;

         // draw
         void drawLine(int x1, int y1, int x2, int y2) const;
         void drawRectangle(int x1, int y1, int x2, int y2, float rot = 0, bool line = false) const;
         void drawParticleSystem(ParticleSystem const &ps);

         // shader
         void shaderSwap(Shader *sp);
         void shaderDrop();
         void shaderUpdate();

         // texture
         void textureSet(char const *name);
         void textureUnbind();

         int realw, realh;
         int w, h;

      private:
         class PrivData;

         void resize(int w, int h);

         float cr, cg, cb, ca;

         std::unique_ptr<PrivData> privdata;

         std::unique_ptr<Shader> shaderBase;
         Shader *shaderCurrent;

         Core::Vector4 textureMinMax;
      };
   }
}

#endif//Doom__GL__Window_H__

