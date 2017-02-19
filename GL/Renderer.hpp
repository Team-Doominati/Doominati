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
      void fontBind(FontFace &font) {fontBound = &font;}
      void fontUnbind()             {fontBound = fontBase.get();}

      // line
      void lineSmooth(bool on)  const;
      void lineWidth(int width) const;

      // render
      void renderBegin();
      void render();
      void renderEnd();

      // shader
      std::size_t shaderAdd(GDCC::Core::String name, char const *f, char const *v);
      std::size_t shaderAdd(GDCC::Core::String name, FS::File *f, FS::File *v);

      void shaderBind(ShaderData *shd);

      ShaderData const *shaderCurrent() {return shdBound;}

      ShaderData *shaderGet(GDCC::Core::String name) {return &shaderGetRaw(name)->data;}
      ShaderData *shaderGet(char const *name)        {return &shaderGetRaw(name)->data;}
      ShaderData *shaderGet(std::size_t idx)         {return &shdMan.get(idx)->data;}

      std::size_t shaderGetIdx(GDCC::Core::String name) {return shaderGetRaw(name)->idx;}

      void shaderUnbind() {shaderBind(&shdMan.resNone->data);}

      // texture
      void textureBind(char const *name) {textureBind(textureGet(name));}
      void textureBind(TextureData const *tex);

      TextureData const *textureCurrent() {return texBound;}

      TextureData *textureGet(GDCC::Core::String name) {return &textureGetRaw(name)->data;}
      TextureData *textureGet(char const *name)        {return &textureGetRaw(name)->data;}
      TextureData *textureGet(std::size_t idx)         {return &texMan.get(idx)->data;}

      std::size_t textureGetIdx(GDCC::Core::String name) {return textureGetRaw(name)->idx;}

      void textureUnbind();

      AlignHorz textAlignH;
      AlignVert textAlignV;


      static Renderer *GetCurrent();
      static void SetCurrent(Renderer *renderer);

   private:
      using Texture = Core::Resource<TextureData>;
      using Shader  = Core::Resource<ShaderData>;

      static constexpr int MaxSubdivisions = 9;

      FontFace *baseFont() const;

      // circle
      void circleCreateLines(int subdivisions);
      void circleCreateTris(int subdivisions);

      // shader
      Shader *shaderGetRaw(GDCC::Core::String name);
      Shader *shaderGet_Base(GDCC::Core::String name);

      // texture
      Texture *textureGetRaw(GDCC::Core::String name);
      Texture *textureGet_File(GDCC::Core::String name);
      Texture *textureGet_None(GDCC::Core::String name);
      Texture *textureGet_NoFi(GDCC::Core::String name);

      Window &win;

      float cr, cg, cb, ca;

      Core::ResourceManager<TextureData> texMan;
      TextureData                 const *texBound;

      Core::ResourceManager<ShaderData> shdMan;
      ShaderData                       *shdBound;

      std::unique_ptr<FontFace> fontBase;
      FontFace                 *fontBound;

      DynamicBuffer const *circleBuff, *circleLineBuff;

      std::unordered_map<int, DynamicBuffer> circleBuffers, circleLineBuffers;
   };
}

#endif//DGE__GL__Renderer_H__

