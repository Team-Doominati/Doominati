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

#include "GL/DynamicBuffer.hpp"
#include "GL/Font.hpp"
#include "GL/Particle.hpp"
#include "GL/ShaderData.hpp"
#include "GL/Texture.hpp"

#include "Core/ResourceManager.hpp"
#include "Core/Types.hpp"

#include <GDCC/Core/Counter.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC::Core
{
   class String;
}

namespace DGE::Game
{
   class PointThinker;
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
      void drawDigit(unsigned int dig, float xl, float yl, float xu, float yu) const;
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
      std::size_t fontAdd(GDCC::Core::String name, FS::File *fp, int ptSize);

      void fontBind(Font *fnt);

      Font const *fontCurrent() {return fntBound;}

      Font *fontGet(GDCC::Core::String name);
      Font *fontGet(std::size_t idx) {return fntMan.get(idx);}

      void fontUnbind() {fontBind(fntMan.resNone);}

      // line
      void lineSmooth(bool on)  const;
      void lineWidth(int width) const;

      // render
      void renderBegin();
      void render();
      void renderEnd();

      void setSize(int w_, int h_) {w = w_; h = h_;}

      // shader
      std::size_t shaderAdd(GDCC::Core::String name, char const *f, char const *v);
      std::size_t shaderAdd(GDCC::Core::String name, FS::File *f, FS::File *v);

      void shaderBind(Shader *shd);

      Shader const *shaderCurrent() {return shdBound;}

      Shader *shaderGet(GDCC::Core::String name);
      Shader *shaderGet(std::size_t idx) {return shdMan.get(idx);}

      void shaderUnbind() {shaderBind(shdMan.resNone);}

      // texture
      std::size_t textureAdd(GDCC::Core::String name, FS::File *fp);

      void textureBind(char const *name) {textureBind(textureGet(name));}
      void textureBind(Texture const *tex);

      Texture *textureGet(GDCC::Core::String name);
      Texture *textureGet(std::size_t idx) {return texMan.get(idx);}

      void textureUnbind() {textureBind(texMan.resNone);}

      std::tuple<float, float> getFocus();

      GDCC::Core::CounterPtr<Game::PointThinker> viewPoint;

      AlignHorz textAlignH;
      AlignVert textAlignV;


      static Renderer *GetCurrent();
      static void SetCurrent(Renderer *renderer);

   private:
      static constexpr int MaxSubdivisions = 9;

      FontFace *baseFont();

      // circle
      void circleCreateLines(int subdivisions);
      void circleCreateTris(int subdivisions);

      // shader
      Shader *shaderGet_Base(GDCC::Core::String name);

      // texture
      Texture *textureGet_File(GDCC::Core::String name, FS::File *file);
      Texture *textureGet_None(GDCC::Core::String name);
      Texture *textureGet_NoFi(GDCC::Core::String name);

      Window &win;

      float cr, cg, cb, ca;

      Core::ResourceManager<TextureData> texMan;
      Texture                     const *texBound;

      Core::ResourceManager<ShaderData> shdMan;
      Shader                           *shdBound;

      Core::ResourceManager<FontFace> fntMan;
      Font                           *fntBound;

      DynamicBuffer const *circleBuff, *circleLineBuff;

      std::unordered_map<int, DynamicBuffer> circleBuffers, circleLineBuffers;

      int w, h;
   };

   //
   // TextureSaver
   //
   class TextureSaver
   {
   public:
      TextureSaver(Renderer &ren_, Texture const *tex) :
         ren{ren_}, idx{tex ? tex->idx : SIZE_MAX} {}
      ~TextureSaver()
         {if(idx != SIZE_MAX) ren.textureBind(ren.textureGet(idx));}

   private:
      Renderer   &ren;
      std::size_t idx;
   };
}

#endif//DGE__GL__Renderer_H__

