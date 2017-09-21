//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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
// Static Functions                                                           |
//

namespace DGE::GL
{
   //
   // FreeData (FontGlyph)
   //
   static void FreeData(FontGlyph &)
   {
   }

   //
   // FreeData
   //
   template<typename T>
   static void FreeData(T *&data)
   {
      if(!data) return;

      for(auto &itr : *data)
         FreeData(itr);

      delete[] data;
      data = nullptr;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // FontFace constructor
   //
   FontFace::FontFace(Core::ResourceManager<TextureData> &man, FS::File *fp, int ptsize) :
      face{nullptr},
      texMan{man},
      planes{nullptr},
      kernCh{},
      kernAmt{}
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
      if(face) FT_Done_Face(face);
      FreeData(planes);
   }

   //
   // FontFace::getChar
   //
   FontGlyph const &FontFace::getChar(char32_t ch)
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

      if(!planes) planes = new PlaneData[1]{};
      GroupData *&groups = (*planes)[ch / (Groups * Blocks * Glyphs)];

      if(!groups) groups = new GroupData[1]{};
      BlockData *&blocks = (*groups)[ch / (Blocks * Glyphs) % Groups];

      if(!blocks) blocks = new BlockData[1]{};
      GlyphData *&glyphs = (*blocks)[ch / Glyphs % Blocks];

      if(!glyphs)
      {
         glyphs = new GlyphData[1];
         for(std::size_t i = 0; i < Glyphs; i++)
            loadChar((*glyphs)[i], ch / Glyphs * Glyphs + i);
      }

      return (*glyphs)[ch % Glyphs];
   }

   //
   // FontFace::loadChar
   //
   void FontFace::loadChar(FontGlyph &gly, char32_t ch)
   {
      if(FT_Load_Char(face, FT_ULong(ch), FT_LOAD_RENDER))
      {
         gly = {0, 0, 0, 0, 0, texMan.resNone->idx, ch};
         return;
      }

      auto glyph   = face->glyph;
      TextureDim w = glyph->bitmap.width;
      TextureDim h = glyph->bitmap.rows;
      auto pitch   = glyph->bitmap.pitch;

      // We need to allocate an extra pixel due to a weird bug (feature?)
      // in GLu that causes an invalid read otherwise.
      std::unique_ptr<TexturePixel[]> buf{new TexturePixel[w * h + 1]{}};

      for(TextureDim y = 0; y < h; y++)
         for(TextureDim x = 0; x < w; x++)
      {
         auto v = glyph->bitmap.buffer[x + y * pitch];
         auto &pixel = buf[x + y * w];
         pixel[0] = pixel[1] = pixel[2] = 1.0f;
         pixel[3] = v / 255.0f;
      }

      if(DebugFontInfo)
         std::cout << "Glyph " << ch << ": W " << w << " H " << h << std::endl;

      gly = {w, h, glyph->advance.x / 64.0f, glyph->bitmap_left,
             -glyph->bitmap_top, texMan.add({w, h, buf.get()}, nullptr)->idx,
             ch};
   }
}

// EOF
