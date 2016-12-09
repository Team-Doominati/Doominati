//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dynamic GPU object buffers.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__DynamicBuffer_H__
#define DGE__GL__DynamicBuffer_H__

#include "GL/OpenGL2.1.h"
#include <cstddef>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   class Shader;

   //
   // VertexLayout
   //

   struct VertexLayout
   {
      GLsizei size;
      GLsizei pntLen, pntPos; // Point
      GLsizei texLen, texPos; // Texture
      GLsizei colLen, colPos; // Color
      GLsizei         nrmPos; // Normal
      bool    nrmUse;
   };

   //
   // VertexXY
   //

   struct VertexXY
   {
      float x, y;

      static VertexLayout Layout;
   };

   //
   // VertexXYUV
   //

   struct VertexXYUV
   {
      float x, y, u, v;

      static VertexLayout Layout;
   };

   //
   // DynamicBuffer
   //

   class DynamicBuffer
   {
   public:
      DynamicBuffer() = delete;
      DynamicBuffer(DynamicBuffer const &other) = delete;

      DynamicBuffer(VertexLayout layout_, GLenum drawtype_ = GL_TRIANGLES) :
         buffer{},
         size{},
         layout{layout_},
         drawtype{drawtype_}
      {
      }

      void bind();
      void bindAndDraw() { bind(); draw(); }
      void draw();
      void setupData(std::size_t arraysize, void *data, GLenum type);
      void setupPointers();

   private:
      GLuint       buffer;
      std::size_t  size;
      VertexLayout layout;
      GLenum       drawtype;
   };
}

#endif//DGE__GL__DynamicBuffer_H__

