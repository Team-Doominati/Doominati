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

#ifndef DGE__GL__Window_H__
#define DGE__GL__Window_H__

#include "GL/Color.hpp"
#include "GL/Particle.hpp"
#include "Core/Vector4.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE
{
   namespace GL
   {
      class Shader;
      class TextureData;

      //
      // Window
      //

      class Window
      {
      public:
         Window(int w, int h);
         ~Window();

         // circle
         void circlePrecision(int subdivisions);

         // draw
         void drawCircle(int x, int y, int radius, bool line = false) const;
         void drawEllipse(int x1, int y1, int x2, int y2, bool line = false) const;
         void drawLine(int x1, int y1, int x2, int y2) const;
         void drawParticleSystem(ParticleSystem const &ps);
         void drawRectangle(int x1, int y1, int x2, int y2, float rot = 0, bool line = false) const;
         void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool line = false) const;

         // drawColor
         void drawColorSet(float r, float g, float b, float a = 1.0f);
         void drawColorSet(Color const &col);
         Color drawColorGet() const;

         // render
         void renderBegin();
         void renderEnd();

         // shader
         void shaderSwap(Shader *sp);
         void shaderDrop();
         void shaderUpdate();

         // texture
         void textureBind(char const *name) {textureBind(textureGet(name));}
         void textureBind(TextureData *tex);
         TextureData *textureGet(char const *name);
         void textureUnbind();

         int realw, realh;
         int w, h;

      private:
         class PrivData;

         void circleCreateLines(int subdivisions);
         void circleCreateTris(int subdivisions);

         void resize(int w, int h);

         TextureData *textureGet_File(char const *name);
         TextureData *textureGet_None(char const *name);

         float cr, cg, cb, ca;

         std::unique_ptr<PrivData> privdata;

         std::unique_ptr<Shader> shaderBase;
         Shader *shaderCurrent;

         Core::Vector4 textureMinMax;

         TextureData *textureNone;
      };
   }
}

#endif//DGE__GL__Window_H__

