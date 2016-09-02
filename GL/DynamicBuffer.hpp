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
      // BufferUsageType
      //
      
      enum class BufferUsageType
      {
         Stream_Draw = GL_STREAM_DRAW,
         Stream_Read = GL_STREAM_READ,
         Stream_Copy = GL_STREAM_COPY,
         
         Static_Draw = GL_STATIC_DRAW,
         Static_Read = GL_STATIC_READ,
         Static_Copy = GL_STATIC_COPY,
         
         Dynamic_Draw = GL_DYNAMIC_DRAW,
         Dynamic_Read = GL_DYNAMIC_READ,
         Dynamic_Copy = GL_DYNAMIC_COPY
      };
      
      //
      // Vertex
      //
      
      class alignas(float[3]) Vertex
      {
      public:
         float x, y, z;
         static GLenum const type = GL_ARRAY_BUFFER;
      };
      
      //
      // DynamicBuffer
      //
      
      template<typename ArrayT = Vertex, GLenum BufferType = ArrayT::type>
      class DynamicBuffer
      {
      public:
         void generate()
         {
            glGenBuffers(1, &buffer);
         }
         
         void bind()
         {
            glBindBuffer(BufferType, buffer);
         }
         
         void bufferInit(std::size_t len, BufferUsageType usage = BufferUsageType::Static_Draw)
         {
            glBufferData(BufferType, sizeof(ArrayT) * len, nullptr, (GLenum)usage);
            arraylen = len;
         }
         
         void bufferSet(ArrayT const *array)
         {
            glBufferSubData(BufferType, 0, sizeof(ArrayT) * arraylen, array);
         }
         
      private:
         GLuint buffer;
         std::size_t arraylen;
      };
   }
}

#endif//Doom__GL__DynamicBuffer_H__

