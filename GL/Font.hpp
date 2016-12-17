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

#ifndef DGE__GL__Font_H__
#define DGE__GL__Font_H__

#include "Core/List.hpp"
#include "Core/HashMap.hpp"

#include "GL/Texture.hpp"

#include <stdexcept>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct FT_FaceRec_;

namespace DGE::FS
{
   class File;
}

namespace DGE::GL
{
   //
   // FontLoadError
   //
   class FontLoadError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };
   
   //
   // FontGlyphInfo
   //
   struct FontGlyphInfo
   {
      GLsizei w, h;
      float   adv;
   };

   //
   // FontGlyph
   //
   class FontGlyph : public FontGlyphInfo
   {
   public:
      FontGlyph() = delete;

      FontGlyph(FontGlyph &&gly) :
         FontGlyphInfo{std::move(gly)},
         data{std::move(gly.data)}, link{this, std::move(gly.link)}, ch{gly.ch}
      {}

      FontGlyph(char32_t ch_, TextureData &&data_, FontGlyphInfo &&info) :
         FontGlyphInfo{std::move(info)},
         data{std::move(data_)}, link{this}, ch{ch_} {}

      FontGlyph(FontGlyph const &) = delete;

      TextureData data;
      Core::ListLink<FontGlyph> link;
      char32_t ch;
   };

   //
   // Font
   //
   class Font
   {
   public:
      Font() = delete;
      Font(FS::File *fp, int ptsize);
      Font(Font const &) = delete;
      Font(Font &&) = default;
      ~Font();

      FontGlyph &getChar(char32_t ch);

   private:
      using GlyphMap = Core::HashMapKeyMem<char32_t, FontGlyph,
         &FontGlyph::ch, &FontGlyph::link>;
      using GlyphVec = std::vector<FontGlyph>;

      FontGlyph &addChar(char32_t ch, TexturePixel const *data, FontGlyphInfo &&info);
      FontGlyph &getChar_Repl(char32_t ch);

      FT_FaceRec_ *face;

      GlyphMap glyMap;
      GlyphVec glyVec;
   };
}

#endif//DGE__GL__Font_H__
