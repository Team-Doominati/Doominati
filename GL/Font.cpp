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

#include <GDCC/Core/Option.hpp>

#include <GDCC/Option/Bool.hpp>

#include <iostream>
#include <unordered_map>


//----------------------------------------------------------------------------|
// Options                                                                    |
//

namespace DGE::GL
{
   //
   // --font-debug-info
   //
   static bool DebugFontInfo = false;
   static GDCC::Option::Bool FontDebugInfoOpt{
      &GDCC::Core::GetOptionList(),
      GDCC::Option::Base::Info()
         .setName("font-debug-info")
         .setDescS("Enables FontFace debug output."),
      &DebugFontInfo
   };
}


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
   // FontFace constructor
   //
   FontFace::FontFace(FS::File *fp, int ptsize) :
      kernAmt{},
      face{nullptr},
      glyMap{},
      glyVec{},
      kernCh{}
   {
      if(!fp) throw FontLoadError("no file");

      FT_Open_Args arg;

      arg.flags       = FT_OPEN_MEMORY;
      arg.memory_base = reinterpret_cast<FT_Byte const *>(fp->data);
      arg.memory_size = fp->size;

      if(FT_Error err = FT_Open_Face(FTLib, &arg, 0, &face))
         throw FontLoadError(FTError.at(err));

      FT_Set_Pixel_Sizes(face, 0, ptsize);
      hasKerning = FT_HAS_KERNING(face);
      height     = face->size->metrics.height / 64.0f;

      if(DebugFontInfo)
      {
         std::cout << "Font face ";
         if(face->family_name) std::cout << face->family_name;
         else                  std::cout << "with no name";
         std::cout << " loaded.\n";
         if(face->style_name)
            std::cout << "Style:    " << face->style_name   << std::endl;
         std::cout    << "Faces:    " << face->num_faces    << std::endl;
         std::cout    << "Glyphs:   " << face->num_glyphs   << std::endl;
         std::cout    << "Charmaps: " << face->num_charmaps << std::endl;
         std::cout    << "Height:   " <<       height       << std::endl;
      }
   }

   //
   // FontFace destructor
   //
   FontFace::~FontFace()
   {
      FT_Done_Face(face);
   }

   //
   // FontFace::addChar
   //
   FontGlyph &FontFace::addChar(char32_t ch, TexturePixel const *data,
      FontGlyphMetr &&metr)
   {
      if(DebugFontInfo)
         std::cout <<
            "Glyph " << ch << ": W " << metr.w << " H " << metr.h << std::endl;

      glyVec.emplace_back(ch, TextureData{metr.w, metr.h, data}, std::move(metr));
      glyMap.insert(&glyVec.back());
      return glyVec.back();
   }

   //
   // FontFace::getChar
   //
   FontGlyph &FontFace::getChar(char32_t ch)
   {
      if(hasKerning)
      {
         if(kernCh)
         {
            FT_UInt prev = FT_Get_Char_Index(face, kernCh);
            FT_UInt next = FT_Get_Char_Index(face, ch);

            FT_Vector kvec;
            FT_Get_Kerning(face, prev, next, FT_KERNING_DEFAULT, &kvec);
            kernAmt = kvec.x / 64.0f;
         }

         kernCh = ch;
      }

      if(auto gly = glyMap.find(ch))
         return *gly;

      if(FT_Load_Char(face, FT_ULong(ch), FT_LOAD_RENDER))
         return getChar_Repl(ch);

      auto glyph = face->glyph;
      GLsizei w  = glyph->bitmap.width;
      GLsizei h  = glyph->bitmap.rows;
      auto pitch = glyph->bitmap.pitch;

      std::unique_ptr<TexturePixel[]> buf{new TexturePixel[w * h]};

      for(GLsizei y = 0; y < h; y++)
         for(GLsizei x = 0; x < w; x++)
      {
         auto g = glyph->bitmap.buffer[x + y * pitch];
         auto &pixel = buf[x + y * w];
         pixel[0] = pixel[1] = pixel[2] = 1.0f;
         pixel[3] = g / 255.0f;
      }

      return addChar(ch, buf.get(), {
         w, h,
         glyph->advance.x / 64.0f, glyph->advance.y / 64.0f,
         glyph->bitmap_left, -glyph->bitmap_top
      });
   }

   //
   // FontFace::getChar_Repl
   //
   FontGlyph &FontFace::getChar_Repl(char32_t ch)
   {
      GLsizei w = face->size->metrics.max_advance / 64.0f;
      GLsizei h = face->size->metrics.height      / 64.0f;

      std::unique_ptr<TexturePixel[]> buf{new TexturePixel[w * h]};

      for(GLsizei y = 0; y < h; y++)
         for(GLsizei x = 0; x < w; x++)
      {
         auto &pixel = buf[x + y * w];
         pixel[0] = pixel[1] = pixel[2] = pixel[3] = 1.0f;
      }

      return addChar(ch, buf.get(), {w, h, float(w), float(h), 0, 0});
   }
}

// EOF
