//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Drawing primitives.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer.hpp"

#include "Code/Convert.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include "Core/Time.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::drawCircle
   //
   void Renderer::drawCircle(float x, float y, float radius, bool line) const
   {
      glPushMatrix();

      glMultMatrixf(Eigen::Affine3f(
         Eigen::Translation3f(x, y, 0.0f) *
         Eigen::Scaling(float(radius))
      ).data());

      if(!line) circleBuff->bindAndDraw();
      else      circleLineBuff->bindAndDraw();

      glPopMatrix();
   }

   //
   // Renderer::drawDigit
   //
   void Renderer::drawDigit(unsigned int dig, float xl, float yl, float xu, float yu) const
   {
      static struct
      {
         bool           seg0 : 1,
              seg1 : 1,           seg2 : 1,
                        seg3 : 1,
              seg4 : 1,           seg5 : 1,
                        seg6 : 1;
      }
      const digtab[10]{
         {1,1,1,0,1,1,1},
         {0,0,1,0,0,1,0},
         {1,0,1,1,1,0,1},
         {1,0,1,1,0,1,1},
         {0,1,1,1,0,1,0},
         {1,1,0,1,0,1,1},
         {1,1,0,1,1,1,1},
         {1,0,1,0,0,1,0},
         {1,1,1,1,1,1,1},
         {1,1,1,1,0,1,1},
      };

      auto &digit = digtab[dig];
      float ym = (yl + yu) / 2;

      GLfloat   prevwidth;
      GLboolean prevsmooth;

      glGetFloatv  (GL_LINE_WIDTH,  &prevwidth);
      glGetBooleanv(GL_LINE_SMOOTH, &prevsmooth);

      glEnable(GL_LINE_SMOOTH);
      glLineWidth(2);

      if(digit.seg0) drawLine(xl, yu, xu, yu);
      if(digit.seg1) drawLine(xl, ym, xl, yu);
      if(digit.seg2) drawLine(xu, ym, xu, yu);
      if(digit.seg3) drawLine(xl, ym, xu, ym);
      if(digit.seg4) drawLine(xl, yl, xl, ym);
      if(digit.seg5) drawLine(xu, yl, xu, ym);
      if(digit.seg6) drawLine(xl, yl, xu, yl);

                      glLineWidth(prevwidth);
      if(!prevsmooth) glDisable(GL_LINE_SMOOTH);
   }

   //
   // Renderer::drawEllipse
   //
   void Renderer::drawEllipse(float x1, float y1, float x2, float y2,
      bool line) const
   {
      if(x1 > x2) std::swap(x1, x2);
      if(y1 > y2) std::swap(y1, y2);

      float rx = (x2 - x1) * 0.5f;
      float ry = (y2 - y1) * 0.5f;

      glPushMatrix();

      glMultMatrixf(Eigen::Affine3f(
         Eigen::Translation3f(x1 + rx, y1 + ry, 0.0f) *
         Eigen::Scaling(rx, ry, 1.0f)
      ).data());

      if(!line) circleBuff->bindAndDraw();
      else      circleLineBuff->bindAndDraw();

      glPopMatrix();
   }

   //
   // Renderer::drawLine
   //
   void Renderer::drawLine(float x1, float y1, float x2, float y2) const
   {
      glBegin(GL_LINES);

      glVertex2f(x1, y1);
      glVertex2f(x2, y2);

      glEnd();
   }

   //
   // Renderer::drawParticleSys
   //
   void Renderer::drawParticleSys(float x, float y, Game::ParticleSys const &ps)
   {
      float frac = Core::GetTickFract<Core::PlayTick<float>>();

      TextureSaver texSave{*this, texBound};
      textureBind(textureGet(ps.tex));

      for(auto &p : ps.particles)
      {
         float px = x + Core::Lerp(p.oldpos.x, p.pos.x, frac);
         float py = y + Core::Lerp(p.oldpos.y, p.pos.y, frac);
         float sx = p.size.x, sy = p.size.y;

         drawColorSet(p.color);
         drawRectangle(px - sx, py - sy, px + sx, py + sy, p.rot);
      }
   }

   //
   // Renderer::drawRectangle
   //
   void Renderer::drawRectangle(float x1, float y1, float x2, float y2,
      float rot, bool line) const
   {
      if(x1 > x2) std::swap(x1, x2);
      if(y1 > y2) std::swap(y1, y2);

      Core::Vector2 v[4] = {{x1, y1}, {x2, y1}, {x2, y2}, {x1, y2}};

      if(rot)
      {
         float c = std::cos(rot);
         float s = std::sin(rot);

         float sx = x1 + ((x2 - x1) / 2.0f);
         float sy = y1 + ((y2 - y1) / 2.0f);

         for(int i = 0; i < 4; i++)
         {
            float x = v[i].x - sx;
            float y = v[i].y - sy;
            v[i].x = ((x * c) - (y * s)) + sx;
            v[i].y = ((x * s) + (y * c)) + sy;
         }
      }

      if(!line)
      {
         glBegin(GL_TRIANGLES);

         // B--A
         // | /
         // C
         glTexCoord2f(1, 0); glVertex2f(v[1].x, v[1].y);
         glTexCoord2f(0, 0); glVertex2f(v[0].x, v[0].y);
         glTexCoord2f(0, 1); glVertex2f(v[3].x, v[3].y);

         //    A
         //  / |
         // B--C
         glTexCoord2f(1, 0); glVertex2f(v[1].x, v[1].y);
         glTexCoord2f(0, 1); glVertex2f(v[3].x, v[3].y);
         glTexCoord2f(1, 1); glVertex2f(v[2].x, v[2].y);

         glEnd();
      }
      else
      {
         glBegin(GL_LINE_LOOP);

         // A--B
         // |  |
         // D--C
         for(int i = 0; i < 4; i++)
            glVertex2f(v[i].x, v[i].y);

         glEnd();
      }
   }

   //
   // Renderer::drawTriangle
   //
   void Renderer::drawTriangle(float x1, float y1, float x2, float y2,
      float x3, float y3, bool line) const
   {
      if(!line) glBegin(GL_TRIANGLES);
      else      glBegin(GL_LINE_LOOP);

      glTexCoord2f(0.5f, 0); glVertex2f(x1, y1);
      glTexCoord2f(0,    1); glVertex2f(x2, y2);
      glTexCoord2f(1,    1); glVertex2f(x3, y3);

      glEnd();
   }

   //
   // Renderer::drawColorSet
   //
   void Renderer::drawColorSet(float r, float g, float b, float a)
   {
      glColor4f(r, g, b, a);
      cr = r;
      cg = g;
      cb = b;
      ca = a;
   }

   //
   // Renderer::drawColorSet
   //
   void Renderer::drawColorSet(Core::Color const &col)
   {
      drawColorSet(col.r, col.g, col.b, col.a);
   }

   //
   // Renderer::drawColorGet
   //
   Core::Color Renderer::drawColorGet() const
   {
      return {cr, cg, cb, ca};
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   DGE_Code_NativeLoaderDefn(Renderer_Draw);

   //
   // void DGE_Draw_SetColor(unsigned long _Fract r, g, b, a)
   //
   DGE_Code_NativeDefn(DGE_Draw_SetColor)
   {
      auto r = argc > 0 ? Code::ULFractToHost(argv[0]) : 1.0f;
      auto g = argc > 1 ? Code::ULFractToHost(argv[1]) : 1.0f;
      auto b = argc > 2 ? Code::ULFractToHost(argv[2]) : 1.0f;
      auto a = argc > 3 ? Code::ULFractToHost(argv[3]) : 1.0f;

      Renderer::GetCurrent()->drawColorSet(r, g, b, a);
      return false;
   }

   //
   // void DGE_Draw_Circle(short accum x, y, radius)
   //
   DGE_Code_NativeDefn(DGE_Draw_Circle)
   {
      auto x = Code::SAccumToHost(argv[0]);
      auto y = Code::SAccumToHost(argv[1]);
      auto r = Code::SAccumToHost(argv[2]);

      Renderer::GetCurrent()->drawCircle(x, y, r);
      return false;
   }

   //
   // void DGE_Draw_CircleLine(short accum x, y, radius)
   //
   DGE_Code_NativeDefn(DGE_Draw_CircleLine)
   {
      auto x = Code::SAccumToHost(argv[0]);
      auto y = Code::SAccumToHost(argv[1]);
      auto r = Code::SAccumToHost(argv[2]);

      Renderer::GetCurrent()->drawCircle(x, y, r, true);
      return false;
   }

   //
   // void DGE_Draw_Ellipse(short accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_Draw_Ellipse)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawEllipse(x1, y1, x2, y2);
      return false;
   }

   //
   // void DGE_Draw_EllipseLine(short accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_Draw_EllipseLine)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawEllipse(x1, y1, x2, y2, true);
      return false;
   }

   //
   // void DGE_Draw_ParticleSys(unsigned id, short accum x, short accum y)
   //
   DGE_Code_NativeDefn(DGE_Draw_ParticleSys)
   {
      auto th = Game::ParticleSys::Get(argv[0]);
      auto x = Code::SAccumToHost(argv[1]);
      auto y = Code::SAccumToHost(argv[2]);

      if(th) Renderer::GetCurrent()->drawParticleSys(x, y, *th);
      return false;
   }

   //
   // void DGE_Draw_Rectangle(short accum x1, y1, x2, y2[,
   //    unsigned long fract rot])
   //
   DGE_Code_NativeDefn(DGE_Draw_Rectangle)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);
      auto rt = argc > 4 ? Code::ULFractToHost(argv[4]) : 0.0;

      Renderer::GetCurrent()->drawRectangle(x1, y1, x2, y2, rt);
      return false;
   }

   //
   // void DGE_Draw_RectangleLine(short accum x1, y1, x2, y2[,
   //    unsigned long fract rot])
   //
   DGE_Code_NativeDefn(DGE_Draw_RectangleLine)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);
      auto rt = argc > 4 ? Code::ULFractToHost(argv[4]) : 0.0;

      Renderer::GetCurrent()->drawRectangle(x1, y1, x2, y2, rt, true);
      return false;
   }

   //
   // void DGE_Draw_Line(short accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_Draw_Line)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawLine(x1, y1, x2, y2);
      return false;
   }

   //
   // void DGE_Draw_Triangle(short accum x1, y1, x2, y2, x3, y3)
   //
   DGE_Code_NativeDefn(DGE_Draw_Triangle)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);
      auto x3 = Code::SAccumToHost(argv[4]);
      auto y3 = Code::SAccumToHost(argv[5]);

      Renderer::GetCurrent()->drawTriangle(x1, y1, x2, y2, x3, y3);
      return false;
   }

   //
   // void DGE_Draw_TriangleLine(short accum x1, y1, x2, y2, x3, y3)
   //
   DGE_Code_NativeDefn(DGE_Draw_TriangleLine)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);
      auto x3 = Code::SAccumToHost(argv[4]);
      auto y3 = Code::SAccumToHost(argv[5]);

      Renderer::GetCurrent()->drawTriangle(x1, y1, x2, y2, x3, y3, true);
      return false;
   }
}

// EOF

