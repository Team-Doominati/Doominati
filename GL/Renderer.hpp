//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Graphics library drawing.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Renderer_H__
#define DGE__GL__Renderer_H__

#include "GL/Particle.hpp"
#include "GL/Shader.hpp"
#include "GL/Texture.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC::Core
{
   class String;
}

namespace DGE::GL
{
   class Window;

   //
   // Renderer
   //
   class Renderer
   {
   public:
      Renderer() = delete;
      Renderer(Window &win_, int w_, int h_);
      Renderer(Renderer const &) = delete;
      Renderer(Renderer &&) = default;
      ~Renderer();

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

      // line
      void lineSmooth(bool on);
      void lineWidth(int width);

      // render
      void renderBegin();
      void renderEnd();

      // shader
      void shaderSwap(Shader &s) {(shaderCurrent = &s)->setCurrent();}
      void shaderDrop() {shaderSwap(shaderBase);}
      void shaderUpdate() {shaderCurrent->update();}

      // texture
      void textureBind(char const *name) {textureBind(textureGet(name));}
      void textureBind(TextureData *tex);

      TextureData *textureGet(GDCC::Core::String name);
      TextureData *textureGet(char const *name);
      TextureData *textureGet(std::size_t idx);

      std::size_t textureGetIdx(GDCC::Core::String name);

      void textureUnbind();

      int realw, realh;
      int w, h;

      static Renderer *Current;

   private:
      class PrivData;
      class Texture;

      void circleCreateLines(int subdivisions);
      void circleCreateTris(int subdivisions);

      void resize(int w, int h);

      Texture *textureGetRaw(GDCC::Core::String name);
      Texture *textureGet_File(GDCC::Core::String name);
      Texture *textureGet_None(GDCC::Core::String name);

      int prevw, prevh;

      float cr, cg, cb, ca;

      std::unique_ptr<PrivData> privdata;
      Window &win;

      Shader  shaderBase;
      Shader *shaderCurrent;
   };
}

#endif//DGE__GL__Renderer_H__
