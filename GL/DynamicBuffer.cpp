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


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace Doom
{
   namespace GL
   {
      VertexLayout VertexXY::Layout{
         /* size           */ sizeof(VertexXY),
         /* pntLen, pntPos */     2, 0,
         /* texLen, texPos */     0, 0,
         /* colLen, colPos */     0, 0,
         /* nrmUse, nrmPos */ false, 0
      };

      VertexLayout VertexXYUV::Layout{
         /* size           */ sizeof(VertexXYUV),
         /* pntLen, pntPos */     2, 0,
         /* texLen, texPos */     2, 2,
         /* colLen, colPos */     0, 0,
         /* nrmUse, nrmPos */ false, 0
      };
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
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
         if(layout.pntLen)
         {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(layout.pntLen, GL_FLOAT, layout.size, (GLvoid *)(sizeof(float) * layout.pntPos));
         }
         else
            glDisableClientState(GL_VERTEX_ARRAY);

         if(layout.texLen)
         {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(layout.texLen, GL_FLOAT, layout.size, (GLvoid *)(sizeof(float) * layout.texPos));
         }
         else
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

         if(layout.colLen)
         {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(layout.colLen, GL_FLOAT, layout.size, (GLvoid *)(sizeof(float) * layout.colPos));
         }
         else
            glDisableClientState(GL_COLOR_ARRAY);

         if(layout.nrmUse)
         {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, layout.size, (GLvoid *)(sizeof(float) * layout.nrmPos));
         }
         else
            glDisableClientState(GL_NORMAL_ARRAY);
      }

      //
      // DynamicBuffer::setupData
      //

      void DynamicBuffer::setupData(std::size_t arraysize, void *data, GLenum type)
      {
         if(!buffer)
            glGenBuffers(1, &buffer);

         bind();

         size = arraysize;
         glBindBuffer(GL_ARRAY_BUFFER, buffer);
         glBufferData(GL_ARRAY_BUFFER, arraysize * layout.size, data, type);
      }
   }
}

// EOF

