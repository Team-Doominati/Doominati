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

#ifndef Doom__GL__DynamicBuffer_H__
#define Doom__GL__DynamicBuffer_H__

#include "GL/gl_2_1.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace GL
   {
      //
      // Vertex
      //

      class Vertex
      {
      public:
         float x, y, u, v;
      };

      //
      // DynamicBuffer
      //
      // This used to be a templated madhouse, but fuck that shit.
      // Just gimme the data.
      //

      class DynamicBuffer
      {
      public:
         GLuint      buffer{};
         std::size_t size{};

         //
         // DynamicBuffer::SetupPointers
         //

         static void SetupPointers()
         {
            glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), nullptr);
            glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), reinterpret_cast<GLvoid *>(2 * sizeof(float)));
         }
      };
   }
}

#endif//Doom__GL__DynamicBuffer_H__

