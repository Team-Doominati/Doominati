//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
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

#include "../GL/Types.hpp"

#include <cstddef>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // VertexLayout
   //
   struct VertexLayout
   {
      std::size_t size;
      std::size_t pntLen, pntPos; // Point
      std::size_t texLen, texPos; // Texture
      std::size_t colLen, colPos; // Color
      bool        nrmUse;
      std::size_t nrmPos; // Normal
   };

   //
   // VertexXY
   //
   struct VertexXY
   {
      float x, y;

      static VertexLayout const Layout;
   };

   //
   // VertexXYUV
   //
   struct VertexXYUV
   {
      float x, y, u, v;

      static VertexLayout const Layout;
   };

   //
   // DynamicBuffer
   //
   class DynamicBuffer
   {
   public:
      DynamicBuffer() = delete;
      DynamicBuffer(VertexLayout layout_, GLenum drawtype_ = GL_TRIANGLES) :
         buffer{}, size{}, layout{layout_}, drawtype{drawtype_} {}
      DynamicBuffer(DynamicBuffer const &) = delete;
      DynamicBuffer(DynamicBuffer &&) = default;
      ~DynamicBuffer();

      void bind() const;
      void bindAndDraw() const {bind(); draw();}
      void draw() const;
      void setupData(std::size_t arraysize, void *data, GLenum type);
      void setupPointers() const;

   private:
      GLuint       buffer;
      std::size_t  size;
      VertexLayout layout;
      GLenum       drawtype;
   };
}

#endif//DGE__GL__DynamicBuffer_H__

