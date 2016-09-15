//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Texture management.
//
//-----------------------------------------------------------------------------

#ifndef Doom__GL__Texture_H__
#define Doom__GL__Texture_H__

#include "GL/gl_2_1.h"

#include "Core/Vector4.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace FS
   {
      class File;
   }

   namespace GL
   {
      using TexturePixel = GLfloat[4];

      //
      // TextureData
      //
      class TextureData
      {
      public:
         TextureData(TextureData const &other) = delete;
         TextureData(TextureData &&other) = default;
         TextureData(GLsizei width, GLsizei height, TexturePixel *texdata);

         GLuint        tex;
         Core::Vector4 minmax;
      };

      //
      // TextureLoader
      //
      class TextureLoader
      {
      public:
         virtual void data(TexturePixel *buf) = 0;

         virtual std::pair<GLsizei, GLsizei> size() = 0;
      };

      //
      // TextureLoaderError
      //
      class TextureLoaderError : public std::runtime_error
      {
         using std::runtime_error::runtime_error;
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
      std::unique_ptr<TextureLoader> CreateTextureLoader(FS::File *file);
   }
}

#endif//Doom__GL__Texture_H__

