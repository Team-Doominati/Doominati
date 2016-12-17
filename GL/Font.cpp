//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Font objects.
//
//-----------------------------------------------------------------------------

#include "GL/Font.hpp"

#include "FS/File.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // FontLibrary
   //
   class FontLibrary
   {
   public:
      FontLibrary() {FT_Init_FreeType(&lib);}
      ~FontLibrary() {FT_Done_FreeType(lib);}
      operator FT_Library & () {return lib;}

   private:
      FT_Library lib;
   };
}


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::GL
{
   static std::unordered_map<FT_Error, std::string> const FTError = {
#define FT_NOERRORDEF_(label, code, string) {FT_Err_##label, string},
#define FT_ERRORDEF_(label, code, string)   {FT_Err_##label, string},
#include FT_ERROR_DEFINITIONS_H
   };

   static FontLibrary FTLib;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Font constructor
   //
   Font::Font(FS::File *fp, int ptsize) :
      face{nullptr},
      glyMap{},
      glyVec{}
   {
      if(!fp) throw FontLoadError("no file");

      FT_Open_Args arg;

      arg.flags       = FT_OPEN_MEMORY;
      arg.memory_base = reinterpret_cast<FT_Byte const *>(fp->data);
      arg.memory_size = fp->size;

      if(FT_Error err = FT_Open_Face(FTLib, &arg, 0, &face))
         throw FontLoadError(FTError.at(err));

      FT_Set_Pixel_Sizes(face, 0, ptsize);
   }

   //
   // Font destructor
   //
   Font::~Font()
   {
      FT_Done_Face(face);
   }

   //
   // Font::addChar
   //
   FontGlyph &Font::addChar(char32_t ch, TexturePixel const *data, FontGlyphInfo &&info)
   {
      glyVec.emplace_back(ch, TextureData{info.w, info.h, data}, std::move(info));
      glyMap.insert(&glyVec.back());
      return glyVec.back();
   }

   //
   // Font::getChar
   //
   FontGlyph &Font::getChar(char32_t ch)
   {
      if(auto gly = glyMap.find(ch))
         return *gly;

      if(FT_Load_Char(face, FT_ULong(ch), FT_LOAD_RENDER))
         return getChar_Repl(ch);

      auto &glyph = face->glyph;
      GLsizei w   = glyph->bitmap.width;
      GLsizei h   = glyph->bitmap.rows;

      std::unique_ptr<TexturePixel[]> buf{new TexturePixel[w * h]{}};

      for(GLsizei y = 0; y < h; y++)
         for(GLsizei x = 0; x < w; x++)
      {
         auto g = glyph->bitmap.buffer[x + y * h];
         buf[x + y * w][3] = g / 255.0;
      }

      return addChar(ch, buf.get(), {w, h, glyph->advance.x / 64.0f});
   }

   //
   // Font::getChar_Repl
   //
   FontGlyph &Font::getChar_Repl(char32_t ch)
   {
      // TODO: actual replacement char
      TexturePixel const data[4] =
         {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

      return addChar(ch, data, {2, 2, 3});
   }
}

// EOF
