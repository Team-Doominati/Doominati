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

#include "GL/Texture.hpp"

#include "FS/File.hpp"

#include <GL/glu.h>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // TextureData constructor
      //
      TextureData::TextureData(GLsizei width, GLsizei height, TexturePixel const *texdata)
      {
         glGenTextures(1, &tex);
         glBindTexture(GL_TEXTURE_2D, tex);

         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, texdata);

         gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_FLOAT, texdata);
      }
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      std::unique_ptr<TextureLoader> CreateTextureLoader_PAM(FS::File *file);
      std::unique_ptr<TextureLoader> CreateTextureLoader_PBM(FS::File *file);
      std::unique_ptr<TextureLoader> CreateTextureLoader_PGM(FS::File *file);
      std::unique_ptr<TextureLoader> CreateTextureLoader_PPM(FS::File *file);

      //
      // CreateTextureLoader
      //
      std::unique_ptr<TextureLoader> CreateTextureLoader(FS::File *file)
      {
         switch(file->format)
         {
         case FS::Format::PAM: return CreateTextureLoader_PAM(file);
         case FS::Format::PBM: return CreateTextureLoader_PBM(file);
         case FS::Format::PGM: return CreateTextureLoader_PGM(file);
         case FS::Format::PPM: return CreateTextureLoader_PPM(file);

         default:
            throw TextureLoaderError("unknown format");
         }
      }
   }
}

// EOF

