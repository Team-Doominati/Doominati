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

#include "GL/Font.hpp"
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
   // AlignHorz
   //
   enum class AlignHorz
   {
      Keep,
      Left,
      Right,
      Center,
      RightBox,
      CenterBox
   };

   //
   // AlignVert
   //
   enum class AlignVert
   {
      Keep,
      Top,
      Bottom,
      Center
   };

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
      void drawText(int x, int y, char const *str, int maxwidth = 0);
      void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool line = false) const;

      // drawColor
      void drawColorSet(float r, float g, float b, float a = 1.0f);
      void drawColorSet(Color const &col);
      Color drawColorGet() const;

      // font
      void fontBind(FontFace &font) {fontCurrent = &font;}
      void fontUnbind() {fontCurrent = fontBase.get();}

      // line
      void lineSmooth(bool on);
      void lineWidth(int width);

      // render
      void renderBegin();
      void renderEnd();

      // shader
      void shaderSwap(Shader &s) {(shaderCurrent = &s)->setCurrent();}
      void shaderDrop()          {shaderSwap(shaderBase);}
      void shaderUpdate()        {shaderCurrent->update();}

      // texture
      void textureBind(char const *name) {textureBind(textureGet(name));}
      void textureBind(TextureData const *tex);

      TextureData const *textureCurrent();

      TextureData *textureGet(GDCC::Core::String name);
      TextureData *textureGet(char const *name);
      TextureData *textureGet(std::size_t idx);

      std::size_t textureGetIdx(GDCC::Core::String name);

      void textureUnbind();

      int w, h;

      AlignHorz textAlignH;
      AlignVert textAlignV;


      static Renderer *GetCurrent();
      static void SetCurrent(Renderer *renderer);

   private:
      class PrivData;
      class Texture;

      static constexpr int MaxSubdivisions = 9;

      FontFace *baseFont() const;

      void circleCreateLines(int subdivisions);
      void circleCreateTris(int subdivisions);

      void resize(int w, int h);

      Texture *textureGetRaw(GDCC::Core::String name);
      Texture *textureGet_File(GDCC::Core::String name);
      Texture *textureGet_None(GDCC::Core::String name);
      Texture *textureGet_NoFi(GDCC::Core::String name);

      float cr, cg, cb, ca;

      std::unique_ptr<PrivData> privdata;
      Window &win;

      Shader  shaderBase;
      Shader *shaderCurrent;

      std::unique_ptr<FontFace> fontBase;
      FontFace                 *fontCurrent;
   };
}

#endif//DGE__GL__Renderer_H__

