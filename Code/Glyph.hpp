//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Glyph handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Glyph_H__
#define DGE__Code__Glyph_H__

#include "Code/Types.hpp"

#include "Core/HashMap.hpp"
#include "Core/String.hpp"

#include <GDCC/Core/Exception.hpp>

#include <exception>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Code_GlyphTypeArgs
//
#if __GNUC__
# define DGE_Code_GlyphTypeArgs() \
   __attribute__((unused)) ::DGE::Code::Loader   &loader, \
   __attribute__((unused)) ::DGE::Core::HashedStr glyph
#else
# define DGE_Code_GlyphTypeArgs() \
   ::DGE::Code::Loader   &loader, \
   ::DGE::Core::HashedStr glyph
#endif

//
// DGE_Code_GlyphTypeDefn
//
#define DGE_Code_GlyphTypeDefn(name) \
   class GlyphType_##name : public ::DGE::Code::GlyphType \
   { \
   public: \
      GlyphType_##name() : ::DGE::Code::GlyphType{#name} {} \
      \
      virtual ::DGE::Code::Word resolve(DGE_Code_GlyphTypeArgs()); \
   } GlyphType_Obj_##name; \
   \
   ::DGE::Code::Word GlyphType_##name::resolve(DGE_Code_GlyphTypeArgs()) \


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // GlyphError
   //
   // Thrown to indicate glyph resolution error.
   //
   class GlyphError : public GDCC::Core::ParseException
   {
   public:
      GlyphError(char const *type_, Core::HashedStr name_) :
         type{type_}, name{name_} {}

   private:
      virtual void genMsg() const;

      char            const *type;
      Core::HashedStr const  name;
   };

   //
   // GlyphType
   //
   class GlyphType
   {
   public:
      virtual Word resolve(DGE_Code_GlyphTypeArgs()) = 0;

      Core::HashedStr const name;


      static GlyphType *Find(Core::HashedStr name);

   protected:
      GlyphType(Core::HashedStr name);
      virtual ~GlyphType();

   private:
      Core::ListLink<GlyphType> link;


      using MapT = Core::HashMapKeyMem<Core::HashedStr, GlyphType,
         &GlyphType::name, &GlyphType::link>;

      static MapT &Map();
   };
}

#endif//DGE__Code__Glyph_H__

