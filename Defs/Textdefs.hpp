//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Handling for TEXTDEFS.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Defs__Textdefs_H__
#define DGE__Defs__Textdefs_H__

#include "../Defs/Types.hpp"

#include "../Core/HashMapFixed.hpp"
#include "../Core/String.hpp"

#include <GDCC/Core/Array.hpp>

#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Defs
{
   //
   // Textdefs
   //
   class Textdefs
   {
   public:
      using Lang = Core::HashMapFixed<Core::HashedStr, Core::HashedStr>;


      Core::HashedStr find(Core::HashedStr name);

      Core::HashMapFixed<Core::HashedStr, Lang> langs;
      GDCC::Core::Array<Lang *>                 prefs;
   };

   //
   // TextdefsCompiler
   //
   class TextdefsCompiler
   {
   public:
      void add(TextdefsParserLang const &lang);

      void finish(Textdefs &defs);

   private:
      std::unordered_map<Core::HashedStr, std::unordered_map<Core::HashedStr, Core::HashedStr>> langs;
   };

   //
   // TextdefsParser
   //
   class TextdefsParser
   {
   public:
      TextdefsParser(Core::NTSStream &in_) : in{in_} {}

      explicit operator bool () const;

      TextdefsParserLang getLang();

   private:
      TextdefsParserText getText();

      Core::NTSStream &in;
   };

   //
   // TextdefsParserLang
   //
   class TextdefsParserLang
   {
   public:
      using Data = GDCC::Core::Array<TextdefsParserText>;


      TextdefsParserLang(Core::HashedStr name_, Data &&data_) :
         data{std::move(data_)}, name{name_} {}

      Data data;
      Core::HashedStr name;
   };

   //
   // TextdefsParserText
   //
   class TextdefsParserText
   {
   public:
      TextdefsParserText(Core::HashedStr name_, Core::HashedStr data_) :
         name{name_}, data{data_} {}

      Core::HashedStr name;
      Core::HashedStr data;
   };
}

#endif//DGE__Defs__Textdefs_H__

