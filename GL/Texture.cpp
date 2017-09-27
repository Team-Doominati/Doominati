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

namespace DGE::GL
{
   //
   // TextureData constructor
   //
   TextureData::TextureData(TextureDim w_, TextureDim h_, TexturePixel const *texdata) :
      w{w_},
      h{h_},
      tex{0}
   {
      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_FLOAT, texdata);
   }

   //
   // TextureData destructor
   //
   TextureData::~TextureData()
   {
      if(tex && glIsTexture(tex))
         glDeleteTextures(1, &tex);
   }

   #if DGE_GL_Use_JPEG
   std::unique_ptr<TextureLoader> CreateTextureLoader_JPEG(FS::File *file);
   #endif
   std::unique_ptr<TextureLoader> CreateTextureLoader_PAM(FS::File *file);
   std::unique_ptr<TextureLoader> CreateTextureLoader_PBM(FS::File *file);
   std::unique_ptr<TextureLoader> CreateTextureLoader_PGM(FS::File *file);
   std::unique_ptr<TextureLoader> CreateTextureLoader_PNG(FS::File *file);
   std::unique_ptr<TextureLoader> CreateTextureLoader_PPM(FS::File *file);

   //
   // CreateTextureLoader
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader(FS::File *file)
   {
      switch(file->format)
      {
      #if DGE_GL_Use_JPEG
      case FS::Format::JPEG: return CreateTextureLoader_JPEG(file);
      #endif
      case FS::Format::PAM: return CreateTextureLoader_PAM(file);
      case FS::Format::PBM: return CreateTextureLoader_PBM(file);
      case FS::Format::PGM: return CreateTextureLoader_PGM(file);
      case FS::Format::PNG: return CreateTextureLoader_PNG(file);
      case FS::Format::PPM: return CreateTextureLoader_PPM(file);

      default:
         throw TextureLoaderError("unknown format");
      }
   }
}

// EOF

