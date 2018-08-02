//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2018 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Glyph handling.
//
//-----------------------------------------------------------------------------

#include "Code/Glyph.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // GlyphError::whatGen
   //
   char const *GlyphError::whatGen() const noexcept
   {
      std::ostringstream oss;
      putOrigin(oss);
      oss << "unknown {" << type << "}: '" << name << '\'';
      return whatSet(oss.str());
   }

   //
   // GlyphType constructor
   //
   GlyphType::GlyphType(Core::HashedStr name_) :
      name{name_},
      link{this}
   {
      Map().insert(this);
   }

   //
   // GlyphType destructor
   //
   GlyphType::~GlyphType()
   {
      Map().unlink(this);
   }

   //
   // GlyphType::Find
   //
   GlyphType *GlyphType::Find(Core::HashedStr name)
   {
      return Map().find(name);
   }

   //
   // GlyphType::Map
   //
   GlyphType::MapT &GlyphType::Map()
   {
      static MapT map{16};
      return map;
   }
}


//----------------------------------------------------------------------------|
// Glyph Types                                                                |
//

namespace DGE::Code
{
   //
   // {s}
   //
   DGE_Code_GlyphTypeDefn(s)
   {
      return static_cast<std::size_t>(
         GDCC::Core::String{glyph.str, glyph.len, glyph.hash});
   }
}

// EOF

