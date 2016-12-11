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

#include "GL/DynamicBuffer.hpp"

#define GenLayoutPoint(type, fnName, name) \
   if(layout.name##Len) \
   { \
      glEnableClientState(type); \
      gl##fnName##Pointer(layout.name##Len, GL_FLOAT, layout.size, \
         reinterpret_cast<GLvoid *>(sizeof(float) * layout.name##Pos)); \
   } \
   else \
      glDisableClientState(type)


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::GL
{
   VertexLayout const VertexXY::Layout{
      /* Size    */ sizeof(VertexXY),
      /* Point   */     2, 0,
      /* Texture */     0, 0,
      /* Colorn  */     0, 0,
      /* Normal  */ false, 0
   };

   VertexLayout const VertexXYUV::Layout{
      /* Size    */ sizeof(VertexXYUV),
      /* Point   */     2, 0,
      /* Texture */     2, 2,
      /* Colorn  */     0, 0,
      /* Normal  */ false, 0
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // DynamicBuffer destructor
   //
   DynamicBuffer::~DynamicBuffer()
   {
      if(glIsBuffer(buffer)) glDeleteBuffers(1, &buffer);
   }

   //
   // DynamicBuffer::bind
   //
   void DynamicBuffer::bind()
   {
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
   }

   //
   // DynamicBuffer::draw
   //
   void DynamicBuffer::draw()
   {
      setupPointers();
      glDrawArrays(drawtype, 0, size);
   }

   //
   // DynamicBuffer::setupPointers
   //
   void DynamicBuffer::setupPointers()
   {
      GenLayoutPoint(GL_VERTEX_ARRAY,        Vertex,   pnt);
      GenLayoutPoint(GL_TEXTURE_COORD_ARRAY, TexCoord, tex);
      GenLayoutPoint(GL_COLOR_ARRAY,         Color,    col);

      if(layout.nrmUse)
      {
         glEnableClientState(GL_NORMAL_ARRAY);
         glNormalPointer(GL_FLOAT, layout.size,
            reinterpret_cast<GLvoid *>(sizeof(float) * layout.nrmPos));
      }
      else
         glDisableClientState(GL_NORMAL_ARRAY);
   }

   //
   // DynamicBuffer::setupData
   //
   void DynamicBuffer::setupData(std::size_t arraysize, void *data, GLenum type)
   {
      if(!glIsBuffer(buffer)) glGenBuffers(1, &buffer);

      bind();

      size = arraysize;
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, arraysize * layout.size, data, type);
   }
}

// EOF

