//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
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

#include "../GL/Texture.hpp"

#include "../Core/HashMap.hpp"
#include "../Core/List.hpp"
#include "../Core/ResourceManager.hpp"

#include <stdexcept>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct FT_FaceRec_;

namespace DGE::GL
{
   using Font = Core::Resource<FontFace>;

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
      TextureDim w, h;
      float      ax;
      int        ox, oy;
   };

   //
   // FontGlyph
   //
   struct FontGlyph : public FontGlyphMetr
   {
      std::size_t tex;
      char32_t ch;
   };

   //
   // FontFace
   //
   class FontFace
   {
   public:
      FontFace(Core::ResourceManager<TextureData> &man, FS::File *fp,
         int ptsize);
      FontFace(FontFace const &) = delete;
      FontFace(FontFace &&fnt) :
         face{fnt.face}, texMan{fnt.texMan}, planes{fnt.planes},
         hasKerning{fnt.hasKerning}, kernCh{fnt.kernCh}, kernAmt{fnt.kernAmt},
         height{fnt.height}
         {fnt.planes = nullptr; fnt.face = nullptr;}
      ~FontFace();

      FontGlyph const &getChar(char32_t ch);

      void  kernReset()           {kernCh = 0;}
      float getKernAmount() const {return kernAmt;}
      float getHeight()     const {return height;}

   private:
      static constexpr std::size_t Glyphs = 34;
      static constexpr std::size_t Blocks = 64;
      static constexpr std::size_t Groups = 32;
      static constexpr std::size_t Planes = 16;

      using GlyphData = FontGlyph [Glyphs];
      using BlockData = GlyphData*[Blocks];
      using GroupData = BlockData*[Groups];
      using PlaneData = GroupData*[Planes];

      void loadChar(FontGlyph &gly, char32_t ch);

      FT_FaceRec_ *face;

      Core::ResourceManager<TextureData> &texMan;

      PlaneData *planes;

      bool     hasKerning : 1;
      char32_t kernCh;
      float    kernAmt;

      float height;
   };
}

#endif//DGE__GL__Font_H__
