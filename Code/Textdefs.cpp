//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Handling for TEXTDEFS.
//
//-----------------------------------------------------------------------------

#include "Code/Textdefs.hpp"

#include "Code/MemStr.hpp"
#include "Code/Native.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include "Defs/Gamedefs.hpp"

#include <GDCC/Core/Option.hpp>


//----------------------------------------------------------------------------|
// Options                                                                    |
//

namespace DGE::Code
{
   //
   // --text-locale
   //
   GDCC::Option::CStrV TextLocales
   {
      &GDCC::Core::GetOptionList(), GDCC::Option::Base::Info()
         .setName("text-locale")
         .setDescS("Adds a preferred text locale."),

      1
   };

   //
   // Textdefs::Fallback
   //
   static Defs::GamedefsCall TextdefsFallbackDef
   {
      &GetTextdefsDefs(), "Fallback",
      [](Defs::GamedefsParserValue const *value)
      {
         TextdefsFallbacks = {value->data.begin(), value->data.end()};
      }
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   Defs::Textdefs                        Textdefs;
   GDCC::Core::Array<GDCC::Core::String> TextdefsFallbacks{"eng"};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // GetTextdefsDefs
   //
   Defs::GamedefsGroup &GetTextdefsDefs()
   {
      static Defs::GamedefsGroup defs{&Defs::Gamedefs::GetRoot(), "Textdefs"};
      return defs;
   }

   //
   // TextdefsFinish
   //
   void TextdefsFinish()
   {
      // Set up locale preferences.
      Textdefs.prefs = {TextLocales.size() + TextdefsFallbacks.size(), nullptr};
      auto prefItr = Textdefs.prefs.begin();
      for(auto const &pref : TextLocales)       *prefItr++ = Textdefs.langs.find(pref);
      for(auto const &pref : TextdefsFallbacks) *prefItr++ = Textdefs.langs.find(pref);
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   //
   // unsigned DGE_Text_Read(str name, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Text_Read)
   {
      GDCC::Core::String       name{argv[0]};
      Code::MemPtr<Code::Byte> buf{&task->prog->memory, argv[1]};
      Code::Word               len{argv[2]};

      Core::HashedStr *text = nullptr;
      for(auto const &pref : Textdefs.prefs)
      {
         if(pref && (text = pref->find(name)))
            break;
      }

      if(!text)
         return task->dataStk.push(0), false;

      MemStrNCpy(buf, len, text->str, text->len);

      task->dataStk.push(text->len);
      return false;
   }
}

// EOF

