//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Texture management.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Texture_H__
#define DGE__GL__Texture_H__

#include "GL/OpenGL2.1.h"
#include "GL/Types.hpp"

#include <exception>
#include <stdexcept>
#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class File;
}

namespace DGE::GL
{
   //
   // TextureData
   //
   class TextureData
   {
   public:
      TextureData() : tex{0} {}
      TextureData(TextureData const &) = delete;
      TextureData(TextureData &&t) : tex{t.tex} {t.tex = 0;}
      TextureData(TextureDim width, TextureDim height,
         TexturePixel const *texdata);
      ~TextureData();

      TextureData &operator = (TextureData const &) = delete;
      TextureData &operator = (TextureData &&t)
         {std::swap(tex, t.tex); return *this;}

      GLuint tex;
   };

   //
   // TextureLoader
   //
   class TextureLoader
   {
   public:
      virtual ~TextureLoader() {}

      virtual void data(TexturePixel *buf) = 0;

      virtual std::pair<TextureDim, TextureDim> size() = 0;
   };

   //
   // TextureLoaderError
   //
   class TextureLoaderError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   std::unique_ptr<TextureLoader> CreateTextureLoader(FS::File *file);
}

#endif//DGE__GL__Texture_H__

