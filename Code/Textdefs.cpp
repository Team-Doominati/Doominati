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

#include "Code/MemPtr.hpp"
#include "Code/Native.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/NTS.hpp"
#include "Core/String.hpp"

#include "FS/Dir.hpp"

#include <GDCC/Core/Exception.hpp>
#include <GDCC/Core/Option.hpp>

#include <iostream>
#include <unordered_map>


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
}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   using TextLang = Core::HashMapFixed<Core::HashedStr, Core::HashedStr>;
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   GDCC::Core::Array<GDCC::Core::String>         TextFallbacks{"eng"};
   Core::HashMapFixed<Core::HashedStr, TextLang> TextLangs;
   GDCC::Core::Array<TextLang *>                 TextPrefs;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // LoadTextdefs
   //
   void LoadTextdefs()
   {
      std::unordered_map<Core::HashedStr, std::unordered_map<Core::HashedStr, Core::HashedStr>> langs;

      FS::Dir::Root->forFilePath("textdefs", [&](FS::File *file)
      {
         if(file->format != FS::Format::DGE_NTS)
            return;

         // The contents of this file are used as-is, so hold a reference.
         ++file->refs;

         Core::NTSArray  arr{file->data, file->size};
         Core::NTSStream in {arr};

         try
         {
            while(in)
            {
               in.expect("language");
               Core::HashedStr langName = in.get();
               auto           &lang     = langs[langName];

               in.expect("{");

               while(!in.peek("}"))
               {
                  Core::HashedStr name = in.get(); in.expect("=");
                  Core::HashedStr text = in.get(); in.expect(";");

                  lang[name] = text;
               }

               in.expect("}");
            }
         }
         catch(GDCC::Core::ParseException const &e)
         {
            std::cerr << "TEXTDEFS error in '" << file->name << "': " << e.what() << '\n';
         }
      });

      auto langsItr = langs.begin();
      TextLangs.reset(langs.size(), [&](auto lang)
      {
         auto langItr = langsItr->second.begin();

         new(&lang->key) Core::HashedStr{langsItr->first};
         new(&lang->val) TextLang{langsItr->second.size(), [&](auto text)
         {
            new(&text->key) Core::HashedStr{langItr->first};
            new(&text->val) Core::HashedStr{langItr->second};
            ++langItr;
         }};

         ++langsItr;
      });

      // Set up locale preferences.
      TextPrefs = {TextLocales.size() + TextFallbacks.size(), nullptr};
      auto prefItr = TextPrefs.begin();
      for(auto const &pref : TextLocales)   *prefItr++ = TextLangs.find(pref);
      for(auto const &pref : TextFallbacks) *prefItr++ = TextLangs.find(pref);
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
      for(auto const &pref : TextPrefs)
      {
         if(pref && (text = pref->find(name)))
            break;
      }

      if(!text)
         return task->dataStk.push(0), false;

      // Copy text into buffer.
      if(len)
      {
         if(len > text->len)
            len = text->len + 1;

         for(auto c = text->str, e = c + len - 1; c != e;)
            *buf++ = *c++;

         *buf++ = '\0';
      }

      task->dataStk.push(text->len);
      return false;
   }
}

// EOF

