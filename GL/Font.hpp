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
#include "GL/Types.hpp"

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
   // FontGlyphMetr
   //
   struct FontGlyphMetr
   {
      TextureDim w,  h;
      float      ax;
      int        ox, oy;
   };

   //
   // FontGlyph
   //
   class FontGlyph : public FontGlyphMetr
   {
   public:
      FontGlyph() = delete;

      FontGlyph(FontGlyph &&gly) :
         FontGlyphMetr{std::move(gly)},
         data{std::move(gly.data)}, link{this, std::move(gly.link)}, ch{gly.ch}
      {}

      FontGlyph(char32_t ch_, TextureData &&data_, FontGlyphMetr &&metr) :
         FontGlyphMetr{std::move(metr)},
         data{std::move(data_)}, link{this}, ch{ch_} {}

      FontGlyph(FontGlyph const &) = delete;

      TextureData data;
      Core::ListLink<FontGlyph> link;
      char32_t ch;
   };

   //
   // FontFace
   //
   class FontFace
   {
   public:
      FontFace() = delete;
      FontFace(FS::File *fp, int ptsize);
      FontFace(FontFace const &) = delete;
      FontFace(FontFace &&) = default;
      ~FontFace();

      FontGlyph &getChar(char32_t ch);

      void  kernReset()           {kernCh = 0;}
      float getKernAmount() const {return kernAmt;}
      float getHeight()     const {return height;}

   private:
      using GlyphMap = Core::HashMapKeyMem<char32_t, FontGlyph, &FontGlyph::ch, &FontGlyph::link>;
      using GlyphVec = std::vector<FontGlyph>;

      FontGlyph &addChar(char32_t ch, TexturePixel const *data, FontGlyphMetr &&metr);
      FontGlyph &getChar_Repl(char32_t ch);

      FT_FaceRec_ *face;

      GlyphMap glyMap;
      GlyphVec glyVec;

      bool     hasKerning : 1;
      char32_t kernCh;
      float    kernAmt;

      float height;
   };
}

#endif//DGE__GL__Font_H__
