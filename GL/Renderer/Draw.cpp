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

#include "GL/Renderer/PrivData.hpp"

#include "Code/Convert.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::drawCircle
   //
   void Renderer::drawCircle(int x, int y, int radius, bool line) const
   {
      glPushMatrix();

      glMultMatrixf(Eigen::Affine3f(
         Eigen::Translation3f(x, y, 0.0f) *
         Eigen::Scaling(float(radius))
      ).data());

      if(!line) privdata->circleBuff->bindAndDraw();
      else      privdata->circleLineBuff->bindAndDraw();

      glPopMatrix();
   }

   //
   // Renderer::drawEllipse
   //
   void Renderer::drawEllipse(int x1, int y1, int x2, int y2, bool line) const
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

      if(!line) privdata->circleBuff->bindAndDraw();
      else      privdata->circleLineBuff->bindAndDraw();

      glPopMatrix();
   }

   //
   // Renderer::drawLine
   //
   void Renderer::drawLine(int x1, int y1, int x2, int y2) const
   {
      glBegin(GL_LINES);

      glVertex2f(x1, y1);
      glVertex2f(x2, y2);

      glEnd();
   }

   //
   // Renderer::drawRectangle
   //
   void Renderer::drawRectangle(int x1, int y1, int x2, int y2, float rot,
      bool line) const
   {
      if(x1 > x2) std::swap(x1, x2);
      if(y1 > y2) std::swap(y1, y2);

      Core::Vector2 v[4] = {
         { float(x1), float(y1) },
         { float(x2), float(y1) },
         { float(x2), float(y2) },
         { float(x1), float(y2) }
      };

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
   void Renderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3,
      bool line) const
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
   // void DGE_DrawColor(unsigned r, unsigned g, unsigned b, unsigned a)
   //
   DGE_Code_NativeDefn(DGE_DrawColor)
   {
      float r =            Code::ULFractToHost(argv[0]);
      float g =            Code::ULFractToHost(argv[1]);
      float b =            Code::ULFractToHost(argv[2]);
      float a = argc > 3 ? Code::ULFractToHost(argv[3]) : 1.0f;

      Renderer::GetCurrent()->drawColorSet(r, g, b, a);

      return false;
   }

   //
   // void DGE_DrawCircle(int x, int y, int radius)
   //
   DGE_Code_NativeDefn(DGE_DrawCircle)
   {
      int x = static_cast<int>(argv[0]);
      int y = static_cast<int>(argv[1]);
      int r = static_cast<int>(argv[2]);

      Renderer::GetCurrent()->drawCircle(x, y, r);

      return false;
   }

   //
   // void DGE_DrawCircleLine(int x, int y, int radius)
   //
   DGE_Code_NativeDefn(DGE_DrawCircleLine)
   {
      int x = static_cast<int>(argv[0]);
      int y = static_cast<int>(argv[1]);
      int r = static_cast<int>(argv[2]);

      Renderer::GetCurrent()->drawCircle(x, y, r, true);

      return false;
   }

   //
   // void DGE_DrawEllipse(int x1, int y1, int x2, int y2)
   //
   DGE_Code_NativeDefn(DGE_DrawEllipse)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);

      Renderer::GetCurrent()->drawEllipse(x1, y1, x2, y2);

      return false;
   }

   //
   // void DGE_DrawEllipseLine(int x1, int y1, int x2, int y2)
   //
   DGE_Code_NativeDefn(DGE_DrawEllipseLine)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);

      Renderer::GetCurrent()->drawEllipse(x1, y1, x2, y2, true);

      return false;
   }

   //
   // void DGE_DrawRectangle(int x1, int y1, int x2, int y2)
   //
   DGE_Code_NativeDefn(DGE_DrawRectangle)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);

      Renderer::GetCurrent()->drawRectangle(x1, y1, x2, y2);

      return false;
   }

   //
   // void DGE_DrawRectangleLine(int x1, int y1, int x2, int y2)
   //
   DGE_Code_NativeDefn(DGE_DrawRectangleLine)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);

      Renderer::GetCurrent()->drawRectangle(x1, y1, x2, y2, 0, true);

      return false;
   }

   //
   // void DGE_DrawLine(int x1, int y1, int x2, int y2)
   //
   DGE_Code_NativeDefn(DGE_DrawLine)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);

      Renderer::GetCurrent()->drawLine(x1, y1, x2, y2);

      return false;
   }

   //
   // void DGE_DrawTexture(unsigned tex)
   //
   DGE_Code_NativeDefn(DGE_DrawTexture)
   {
      if(argv[0])
         Renderer::GetCurrent()->textureBind(Renderer::GetCurrent()->textureGet(argv[0]));
      else
         Renderer::GetCurrent()->textureUnbind();

      return false;
   }

   //
   // void DGE_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
   //
   DGE_Code_NativeDefn(DGE_DrawTriangle)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);
      int x3 = static_cast<int>(argv[4]);
      int y3 = static_cast<int>(argv[5]);

      Renderer::GetCurrent()->drawTriangle(x1, y1, x2, y2, x3, y3);

      return false;
   }

   //
   // void DGE_DrawTriangleLine(int x1, int y1, int x2, int y2, int x3, int y3)
   //
   DGE_Code_NativeDefn(DGE_DrawTriangleLine)
   {
      int x1 = static_cast<int>(argv[0]);
      int y1 = static_cast<int>(argv[1]);
      int x2 = static_cast<int>(argv[2]);
      int y2 = static_cast<int>(argv[3]);
      int x3 = static_cast<int>(argv[4]);
      int y3 = static_cast<int>(argv[5]);

      Renderer::GetCurrent()->drawTriangle(x1, y1, x2, y2, x3, y3, true);

      return false;
   }

   //
   // unsigned DGE_GetTexture(__str_ent *name)
   //
   DGE_Code_NativeDefn(DGE_GetTexture)
   {
      GDCC::Core::String str{argv[0]};
      task->dataStk.push(Renderer::GetCurrent()->textureGetIdx(str));
      return false;
   }
}

// EOF

