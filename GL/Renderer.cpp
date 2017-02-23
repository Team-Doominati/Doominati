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

#include "GL/Renderer.hpp"

#include "Code/Convert.hpp"
#include "Code/Callback.hpp"
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
   static Code::Callback CallbackDrawPre{"DrawPre"};
   static Code::Callback CallbackDraw{"Draw"};
   static Code::Callback CallbackDrawPost{"DrawPost"};

   static Renderer *CurrentRenderer;
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
   // Renderer constructor
   //
   Renderer::Renderer(Window &win_) :
      textAlignH{AlignHorz::Left}, textAlignV{AlignVert::Top},

      win{win_},

      texMan{},
      texBound{nullptr},

      shdMan{},
      shdBound{nullptr},

      fontBase{baseFont()},
      fontBound{fontBase.get()}
   {
      // Set up OpenGL server (device).
      glClearColor(0.23f, 0.23f, 0.23f, 1.0f);

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glEnable(GL_CULL_FACE); // Cull CCW-sided back facing triangles.

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glActiveTexture(GL_TEXTURE0);

      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
      lineSmooth(false);
      lineWidth(1);

      // Set up matrices.
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      // Set up base shader.
      drawColorSet(1.0, 1.0, 1.0, 1.0);
      shaderGet_Base("SHDNULL");
      shaderUnbind();

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
      // FONT_TODO: don't hardcode this
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
      auto &vbo = circleLineBuffers.emplace(
         std::piecewise_construct,
         std::forward_as_tuple(subdivisions),
         std::forward_as_tuple(VertexXY::Layout, GL_LINE_LOOP)).first->second;

      vbo.setupData(bufarray.size(), bufarray.data(), GL_STATIC_DRAW);

      circleLineBuff = &vbo;
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
      auto &vbo = circleBuffers.emplace(
         std::piecewise_construct,
         std::forward_as_tuple(subdivisions),
         std::forward_as_tuple(VertexXYUV::Layout)).first->second;

      vbo.setupData(bufarray.size(), bufarray.data(), GL_STATIC_DRAW);

      circleBuff = &vbo;
   }

   //
   // Renderer::circlePrecision
   //
   void Renderer::circlePrecision(int subdivisions)
   {
           if(subdivisions > MaxSubdivisions) subdivisions = MaxSubdivisions;
      else if(subdivisions < 0)               subdivisions = 0;

      auto tri = circleBuffers.find(subdivisions);
      if(tri != circleBuffers.end()) circleBuff = &tri->second;
      else                           circleCreateTris(subdivisions);

      auto lin = circleLineBuffers.find(subdivisions);
      if(lin != circleLineBuffers.end()) circleLineBuff = &lin->second;
      else                               circleCreateLines(subdivisions);
   }

   //
   // Renderer:lineSmooth
   //
   void Renderer::lineSmooth(bool on) const
   {
      if(on) glEnable(GL_LINE_SMOOTH);
      else   glDisable(GL_LINE_SMOOTH);
   }

   //
   // Renderer::lineWidth
   //
   void Renderer::lineWidth(int width) const
   {
      glLineWidth(width);
   }

   //
   // Renderer::renderBegin
   //
   void Renderer::renderBegin()
   {
      win.renderBegin();

      CallbackDrawPre(Code::HostToULFract(Core::GetTickFract<Core::PlayTick<float>>()));
   }

   //
   // Renderer::render
   //
   void Renderer::render()
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      CallbackDraw(Code::HostToULFract(Core::GetTickFract<Core::PlayTick<float>>()));
   }

   //
   // Renderer::renderEnd
   //
   void Renderer::renderEnd()
   {
      CallbackDrawPost(Code::HostToULFract(Core::GetTickFract<Core::PlayTick<float>>()));

      win.renderEnd();
   }

   //
   // Renderer::GetCurrent
   //
   Renderer *Renderer::GetCurrent()
   {
      return CurrentRenderer;
   }

   //
   // Renderer::SetCurrent
   //
   void Renderer::SetCurrent(Renderer *renderer)
   {
      CurrentRenderer = renderer;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // void DGE_DrawCirclePrecision(int subdivisions)
   //
   DGE_Code_NativeDefn(DGE_DrawCirclePrecision)
   {
      Renderer::GetCurrent()->circlePrecision(argv[0]);
      return false;
   }

   //
   // void DGE_DrawLineSmooth(_Bool on)
   //
   DGE_Code_NativeDefn(DGE_DrawLineSmooth)
   {
      Renderer::GetCurrent()->lineSmooth(argv[0]);
      return false;
   }

   //
   // void DGE_DrawLineWidth(int width)
   //
   DGE_Code_NativeDefn(DGE_DrawLineWidth)
   {
      Renderer::GetCurrent()->lineWidth(argv[0]);
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

