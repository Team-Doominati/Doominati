//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Graphics Library drawing.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer/PrivData.hpp"

#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"

#include "Core/Time.hpp"

#include "FS/Dir.hpp"

#include "GL/OpenGL2.1.h"
#include "GL/Window.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::GL
{
   //
   // BaseFragShader
   //
   static char const BaseFragShader[] = R"(
      #version 120

      uniform sampler2D dge_texture;

      varying vec4 color;
      varying vec4 texcoord;

      void main(void)
      {
         gl_FragColor = texture2D(dge_texture, texcoord.xy) * color;
//       gl_FragColor = vec4(texcoord.xy, 0.0, 1.0);
      }
   )";

   //
   // BaseVertShader
   //
   static char const BaseVertShader[] = R"(
      #version 120

      varying vec4 color;
      varying vec4 texcoord;

      void main(void)
      {
         gl_Position = ftransform();

         texcoord = gl_MultiTexCoord0;
         color = gl_Color;
      }
   )";

   static std::vector<Code::Function *> CallbackRenderBegin;
   static std::vector<Code::Function *> CallbackRenderEnd;
   static std::vector<Code::Function *> CallbackResize;
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::GL
{
   Renderer *Renderer::Current = nullptr;
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::GL
{
   //
   // Circle_CalcBufSize
   //
   static inline void Circle_CalcBufSize(int subdivisions, std::size_t &bufsize)
   {
      if(!subdivisions) return;

      bufsize += 3;

      for(int i = 0; i < 2; i++)
         Circle_CalcBufSize(subdivisions - 1, bufsize);
   }

   //
   // Circle_CalcPoint
   //
   static inline void Circle_CalcPoint(float angl, VertexXYUV *&buf)
   {
      float s = std::sin(angl);
      float c = std::cos(angl);
      *buf++ = { s, c, s * 0.5f + 0.5f, c * 0.5f + 0.5f };
   }

   //
   // Circle_CalcFaces
   //
   static inline void Circle_CalcFaces(int subdivisions, float anglA, float anglC, VertexXYUV *&buf)
   {
      if(!subdivisions) return;

      float anglB = Core::Lerp(anglA, anglC, 0.5f);

      Circle_CalcPoint(anglA, buf);
      Circle_CalcPoint(anglB, buf);
      Circle_CalcPoint(anglC, buf);

      Circle_CalcFaces(subdivisions - 1, anglA, anglB, buf);
      Circle_CalcFaces(subdivisions - 1, anglB, anglC, buf);
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::PrivData constructor
   //
   Renderer::PrivData::PrivData() :
      texBound{nullptr},
      circleBuff    {VertexXYUV::Layout},
      circleLineBuff{VertexXY::Layout, GL_LINE_LOOP}
   {
   }

   //
   // Renderer::PrivData destructor
   //
   Renderer::PrivData::~PrivData()
   {
   }

   //
   // Renderer constructor
   //
   Renderer::Renderer(Window &win_, int w_, int h_) :
      w{w_}, h{h_},
      textAlignH{AlignHorz::Left}, textAlignV{AlignVert::Top},
      privdata{new PrivData()},
      win{win_},
      shaderBase{BaseFragShader, BaseVertShader},
      shaderCurrent{&shaderBase},
      fontBase{baseFont()},
      fontCurrent{fontBase.get()}
   {
      // Set up OpenGL server (device).
      glClearColor(0.23f, 0.23f, 0.23f, 1.0f);

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glEnable(GL_CULL_FACE); // Cull CCW-sided back facing triangles.

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glActiveTexture(GL_TEXTURE0);

      lineSmooth(false);
      lineWidth(1);

      // Set up matrices.
      resize(w, h);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      // Set up base shader.
      drawColorSet(1.0, 1.0, 1.0);

      // Set up basic no-texture.
      textureGet_None("TEXNULL");
      textureUnbind();

      // Set up VBOs.
      circlePrecision(4);
   }

   //
   // Renderer destructor
   //
   Renderer::~Renderer()
   {
   }

   //
   // Renderer::baseFont
   //
   FontFace *Renderer::baseFont() const
   {
      // TODO: don't hardcode this
      try
         {return new FontFace(FS::Dir::FindFile("Fonts/base.ttf"), 72);}
      catch(FontLoadError const &exc)
      {
         std::cerr << "couldn't load base font: " << exc.what() << std::endl;
         return nullptr;
      }
   }

   //
   // Renderer::circleCreateLines
   //
   void Renderer::circleCreateLines(int subdivisions)
   {
      std::size_t bufsize = 3 * 2;

      for(int i = 0; i < 4; i++)
         Circle_CalcBufSize(subdivisions, bufsize);

      GDCC::Core::Array<VertexXY> bufarray(bufsize);

      for(std::size_t i = 0; i < bufsize; i++)
      {
         float angl = Core::Lerp(0.0f, Core::Tau, i / float(bufsize));
         float s = std::sin(angl);
         float c = std::cos(angl);
         bufarray[i] = { s, c };
      }

      // Generate the VBO.
      privdata->circleLineBuff.setupData(bufarray.size(), bufarray.data(), GL_DYNAMIC_DRAW);
   }

   //
   // Renderer::circleCreateTris
   //
   void Renderer::circleCreateTris(int subdivisions)
   {
      std::size_t bufsize = 3 * 2;

      for(int i = 0; i < 4; i++)
         Circle_CalcBufSize(subdivisions, bufsize);

      GDCC::Core::Array<VertexXYUV> bufarray(bufsize);
      VertexXYUV *buf = bufarray.data();

      // First, make a diamond out of two tris.
      Circle_CalcPoint(Core::Pi + Core::Pi2, buf);
      Circle_CalcPoint(0, buf);
      Circle_CalcPoint(Core::Pi2, buf);

      Circle_CalcPoint(Core::Pi + Core::Pi2, buf);
      Circle_CalcPoint(Core::Pi2, buf);
      Circle_CalcPoint(Core::Pi, buf);

      // Then create a fractal of triangles around that, for each quarter.
      for(int i = 0; i < 4; i++)
         Circle_CalcFaces(subdivisions, Core::Pi2 * i, (Core::Pi2 * i) + Core::Pi2, buf);

      // Generate the VBO.
      privdata->circleBuff.setupData(bufarray.size(), bufarray.data(), GL_DYNAMIC_DRAW);
   }

   //
   // Renderer::circlePrecision
   //
   void Renderer::circlePrecision(int subdivisions)
   {
      circleCreateTris(subdivisions);
      circleCreateLines(subdivisions);
   }

   //
   // Renderer::drawParticleSystem
   //
   void Renderer::drawParticleSystem(ParticleSystem const &ps)
   {
      glPushMatrix();

      glMultMatrixf(ps.mat.data());

      float frac = Core::GetTickFract<Core::PlayTick<float>>();

      auto origtex = textureCurrent();

      textureBind(ps.texname.get());

      for(auto &p : ps.particles)
      {
         float x = Core::Lerp(p.oldposition.x, p.position.x, frac);
         float y = Core::Lerp(p.oldposition.y, p.position.y, frac);

         float sx = 8 * p.scale.x;
         float sy = 8 * p.scale.y;

         drawColorSet(p.color);
         drawRectangle(x - sx, y - sy, x + sx, y + sy, p.rot);
      }

      textureBind(origtex);

      glPopMatrix();
   }

   //
   // Renderer:lineSmooth
   //
   void Renderer::lineSmooth(bool on)
   {
      if(on) glEnable(GL_LINE_SMOOTH);
      else   glDisable(GL_LINE_SMOOTH);
   }

   //
   // Renderer::lineWidth
   //
   void Renderer::lineWidth(int width)
   {
      glLineWidth(width);
   }

   //
   // Renderer::renderBegin
   //
   void Renderer::renderBegin()
   {
      win.renderBegin();

      if(w != win.w || h != win.h)
         resize(win.w, win.h);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for(auto const &callback : CallbackRenderBegin)
         Code::Process::Main->call(callback);
   }

   //
   // Renderer::renderEnd
   //
   void Renderer::renderEnd()
   {
      win.renderEnd();

      for(auto const &callback : CallbackRenderEnd)
         Code::Process::Main->call(callback);
   }

   //
   // Renderer::resize
   //
   void Renderer::resize(int w_, int h_)
   {
      if(w != w_ || h != h_)
      {
         glViewport(0, 0, w = w_, h = h_);

         for(auto const &callback : CallbackResize)
            Code::Process::Main->call<2>(callback, {{static_cast<Code::Word>(w), static_cast<Code::Word>(h)}});
      }
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // void DGE_CallbackDrawBegin(void (*callback)(void))
   //
   DGE_Code_NativeDefn(DGE_CallbackDrawBegin)
   {
      CallbackRenderBegin.push_back(&Code::Process::Main->prog->funcs[argv[0]]);
      return false;
   }

   //
   // void DGE_CallbackDrawEnd(void (*callback)(void))
   //
   DGE_Code_NativeDefn(DGE_CallbackDrawEnd)
   {
      CallbackRenderEnd.push_back(&Code::Process::Main->prog->funcs[argv[0]]);
      return false;
   }

   //
   // void DGE_CallbackResize(void (*callback)(int w, int h))
   //
   DGE_Code_NativeDefn(DGE_CallbackResize)
   {
      CallbackResize.push_back(&Code::Process::Main->prog->funcs[argv[0]]);
      return false;
   }

   //
   // void DGE_SetVirtualResolution(unsigned w, unsigned h)
   //
   DGE_Code_NativeDefn(DGE_SetVirtualResolution)
   {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, argv[0], argv[1], 0, 0, 0.01f);

      glMatrixMode(GL_MODELVIEW);

      return false;
   }
}

// EOF

