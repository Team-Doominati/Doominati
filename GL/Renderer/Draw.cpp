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
   void Renderer::drawDigit(unsigned int dig, float xl, float yl, float xh, float yh) const
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
      float ym = (yl + yh) / 2;

      GLfloat   prevwidth;
      GLboolean prevsmooth;

      glGetFloatv  (GL_LINE_WIDTH,  &prevwidth);
      glGetBooleanv(GL_LINE_SMOOTH, &prevsmooth);

      glEnable(GL_LINE_SMOOTH);
      glLineWidth(2);

      if(digit.seg0) drawLine(xl, yh, xh, yh);
      if(digit.seg1) drawLine(xl, ym, xl, yh);
      if(digit.seg2) drawLine(xh, ym, xh, yh);
      if(digit.seg3) drawLine(xl, ym, xh, ym);
      if(digit.seg4) drawLine(xl, yl, xl, ym);
      if(digit.seg5) drawLine(xh, yl, xh, ym);
      if(digit.seg6) drawLine(xl, yl, xh, yl);

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
   // Renderer::drawParticleSystem
   //
   void Renderer::drawParticleSystem(ParticleSystem const &ps)
   {
      glPushMatrix();

      glMultMatrixf(ps.mat.data());

      float frac = Core::GetTickFract<Core::PlayTick<float>>();

      Core::ResourceSaver<TextureData> texSave{texMan, texBound};

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

      glPopMatrix();
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
   void Renderer::drawColorSet(Color const &col)
   {
      drawColorSet(col.r, col.g, col.b, col.a);
   }

   //
   // Renderer::drawColorGet
   //
   Color Renderer::drawColorGet() const
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
   // void DGE_DrawColor(unsigned long _Fract r, g, b, a)
   //
   DGE_Code_NativeDefn(DGE_DrawColor)
   {
      auto r =            Code::ULFractToHost(argv[0]);
      auto g =            Code::ULFractToHost(argv[1]);
      auto b =            Code::ULFractToHost(argv[2]);
      auto a = argc > 3 ? Code::ULFractToHost(argv[3]) : 1.0f;

      Renderer::GetCurrent()->drawColorSet(r, g, b, a);

      return false;
   }

   //
   // void DGE_DrawCircle(short _Accum x, y, radius)
   //
   DGE_Code_NativeDefn(DGE_DrawCircle)
   {
      auto x = Code::SAccumToHost(argv[0]);
      auto y = Code::SAccumToHost(argv[1]);
      auto r = Code::SAccumToHost(argv[2]);

      Renderer::GetCurrent()->drawCircle(x, y, r);

      return false;
   }

   //
   // void DGE_DrawCircleLine(short _Accum x, y, radius)
   //
   DGE_Code_NativeDefn(DGE_DrawCircleLine)
   {
      auto x = Code::SAccumToHost(argv[0]);
      auto y = Code::SAccumToHost(argv[1]);
      auto r = Code::SAccumToHost(argv[2]);

      Renderer::GetCurrent()->drawCircle(x, y, r, true);

      return false;
   }

   //
   // void DGE_DrawEllipse(short _Accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_DrawEllipse)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawEllipse(x1, y1, x2, y2);

      return false;
   }

   //
   // void DGE_DrawEllipseLine(short _Accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_DrawEllipseLine)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawEllipse(x1, y1, x2, y2, true);

      return false;
   }

   //
   // void DGE_DrawRectangle(short _Accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_DrawRectangle)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawRectangle(x1, y1, x2, y2);

      return false;
   }

   //
   // void DGE_DrawRectangleLine(short _Accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_DrawRectangleLine)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawRectangle(x1, y1, x2, y2, 0, true);

      return false;
   }

   //
   // void DGE_DrawLine(short _Accum x1, y1, x2, y2)
   //
   DGE_Code_NativeDefn(DGE_DrawLine)
   {
      auto x1 = Code::SAccumToHost(argv[0]);
      auto y1 = Code::SAccumToHost(argv[1]);
      auto x2 = Code::SAccumToHost(argv[2]);
      auto y2 = Code::SAccumToHost(argv[3]);

      Renderer::GetCurrent()->drawLine(x1, y1, x2, y2);

      return false;
   }

   //
   // void DGE_DrawTriangle(short _Accum x1, y1, x2, y2, x3, y3)
   //
   DGE_Code_NativeDefn(DGE_DrawTriangle)
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
   // void DGE_DrawTriangleLine(short _Accum x1, y1, x2, y2, x3, y3)
   //
   DGE_Code_NativeDefn(DGE_DrawTriangleLine)
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

