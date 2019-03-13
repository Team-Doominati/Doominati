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

#include "Defs/Textdefs.hpp"

#include "Defs/Gamedefs.hpp"

#include "Core/NTS.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Defs
{
   //
   // Textdefs::find
   //
   Core::HashedStr Textdefs::find(Core::HashedStr name)
   {
      for(auto const &pref : prefs) if(pref)
      {
         if(auto text = pref->find(name))
            return *text;
      }

      return nullptr;
   }

   //
   // TextdefsParser::operator bool
   //
   TextdefsParser::operator bool () const
   {
      return static_cast<bool>(in);
   }

   //
   // TextdefsCompiler::add
   //
   void TextdefsCompiler::add(TextdefsParserLang const &lang)
   {
      auto &l = langs[lang.name];

      for(auto const &text : lang.data)
         l[text.name] = text.data;
   }

   //
   // TextdefsCompiler::finish
   //
   void TextdefsCompiler::finish(Textdefs &defs)
   {
      auto langsItr = langs.begin();
      defs.langs.reset(langs.size(), [&](auto lang)
      {
         auto langItr = langsItr->second.begin();

         new(&lang->key) Core::HashedStr{langsItr->first};
         new(&lang->val) Textdefs::Lang{langsItr->second.size(), [&](auto text)
         {
            new(&text->key) Core::HashedStr{langItr->first};
            new(&text->val) Core::HashedStr{langItr->second};
            ++langItr;
         }};

         ++langsItr;
      });
   }

   //
   // TextdefsParser::getLang
   //
   TextdefsParserLang TextdefsParser::getLang()
   {
      in.expect("language");

      Core::HashedStr name = in.get();

      in.expect("{");

      std::vector<TextdefsParserText> texts;

      while(!in.drop("}"))
         texts.push_back(getText());

      return {name, {GDCC::Core::Move, texts.begin(), texts.end()}};
   }

   //
   // TextdefsParser::getText
   //
   TextdefsParserText TextdefsParser::getText()
   {
      Core::HashedStr name = in.get(); in.expect("=");
      Core::HashedStr text = in.get(); in.expect(";");

      return {name, text};
   }
}

// EOF

