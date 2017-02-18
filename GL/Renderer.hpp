//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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

#include "Core/Types.hpp"


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
      Renderer(Window &win_);
      Renderer(Renderer const &) = delete;
      Renderer(Renderer &&) = default;
      ~Renderer();

      // circle
      void circlePrecision(int subdivisions);

      // draw
      void drawCircle(float x, float y, float radius, bool line = false) const;
      void drawDigit(unsigned int dig, float xl, float yl, float xh, float yh) const;
      void drawEllipse(float x1, float y1, float x2, float y2, bool line = false) const;
      void drawLine(float x1, float y1, float x2, float y2) const;
      void drawParticleSystem(ParticleSystem const &ps);
      void drawRectangle(float x1, float y1, float x2, float y2, float rot = 0, bool line = false) const;
      void drawText(float x, float y, char const *str, float maxwidth = 0);
      void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, bool line = false) const;

      // drawColor
      void drawColorSet(float r, float g, float b, float a = 1.0f);
      void drawColorSet(Color const &col);
      Color drawColorGet() const;

      // font
      void fontBind(FontFace &font) {fontCurrent = &font;}
      void fontUnbind()             {fontCurrent = fontBase.get();}

      // line
      void lineSmooth(bool on)  const;
      void lineWidth(int width) const;

      // render
      void renderBegin();
      void render();
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

      AlignHorz textAlignH;
      AlignVert textAlignV;


      static Renderer *GetCurrent();
      static void SetCurrent(Renderer *renderer);

   private:
      class PrivData;
      using Texture = Core::Resource<TextureData>;

      static constexpr int MaxSubdivisions = 9;

      FontFace *baseFont() const;

      void circleCreateLines(int subdivisions);
      void circleCreateTris(int subdivisions);

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

