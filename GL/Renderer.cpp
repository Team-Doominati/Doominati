//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Graphics Library drawing.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer.hpp"

#include "GL/OpenGL2.1.h"
#include "GL/Window.hpp"

#include "Code/Convert.hpp"
#include "Code/Callback.hpp"
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include "Core/Time.hpp"

#include "FS/Dir.hpp"

#include "Game/BlockMap.hpp"
#include "Game/Event.hpp"
#include "Game/RenderThinker.hpp"
#include "Game/Sector.hpp"

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

      texMan{}, texBound{nullptr},
      shdMan{}, shdBound{nullptr},
      fntMan{}, fntBound{nullptr}
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
      textureGet_NoFi("TEXNOFI");
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
   // Renderer::getFocus
   //
   std::tuple<float, float> Renderer::getFocus()
   {
      // If viewPoint set, focus on that.
      if(viewPoint)
         return {viewPoint->x.toF() - w / 2, viewPoint->y.toF() - h / 2};

      // Otherwise, focus on center of world.
      else
         return {-w / 2, -h / 2};
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

      // Calculate render focus.
      float rx, ry;
      std::tie(rx, ry) = getFocus();

      drawColorSet(1.0f, 1.0f, 1.0f);

      // Render floors.
      for(auto &sec : Game::BlockMap::Root.listSec)
         if(sec.texf)
      {
         // TODO: Non-rectangular sector rendering.

         textureBind(textureGet(sec.texf));
         float sxl = sec.xl - rx, sxu = sec.xu - rx;
         float syl = sec.yl - ry, syu = sec.yu - ry;
         drawRectangle(sxl, syl, sxu, syu);
      }

      // Render thinkers.
      for(auto &th : Game::RenderThinker::Range())
      {
         textureBind(textureGet(th.sprite));
         float tx = th.x - rx, ty = th.y - ry;
         drawRectangle(tx - th.rsx, ty - th.rsy, tx + th.rsx, ty + th.rsy);
      }

      // TODO: Render ceilings.
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
   // void DGE_Draw_SetCirclePrecision(int subdivisions)
   //
   DGE_Code_NativeDefn(DGE_Draw_SetCirclePrecision)
   {
      Renderer::GetCurrent()->circlePrecision(argv[0]);
      return false;
   }

   //
   // void DGE_Draw_SetLineSmoothing(_Bool on)
   //
   DGE_Code_NativeDefn(DGE_Draw_SetLineSmoothing)
   {
      Renderer::GetCurrent()->lineSmooth(argv[0]);
      return false;
   }

   //
   // void DGE_Draw_SetLineWidth(int width)
   //
   DGE_Code_NativeDefn(DGE_Draw_SetLineWidth)
   {
      Renderer::GetCurrent()->lineWidth(argv[0]);
      return false;
   }

   //
   // DGE_Point2 DGE_Renderer_GetViewpoint(void)
   //
   DGE_Code_NativeDefn(DGE_Renderer_GetViewpoint)
   {
      float x, y;
      std::tie(x, y) = Renderer::GetCurrent()->getFocus();
      task->dataStk.push(Code::HostToSAccum(x));
      task->dataStk.push(Code::HostToSAccum(y));
      return false;
   }

   //
   // void DGE_Renderer_SetViewpoint(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Renderer_SetViewpoint)
   {
      Renderer::GetCurrent()->viewPoint = Game::PointThinker::Get(argv[0]);
      return false;
   }

   //
   // void DGE_Renderer_SetVirtualRes(unsigned w, unsigned h)
   //
   DGE_Code_NativeDefn(DGE_Renderer_SetVirtualRes)
   {
      unsigned w = argv[0];
      unsigned h = argv[1];

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, w, h, 0, 0, 0.01f);

      glMatrixMode(GL_MODELVIEW);

      Renderer::GetCurrent()->setSize(w, h);
      Game::SetResolutionVirt(w, h);

      return false;
   }
}

// EOF

