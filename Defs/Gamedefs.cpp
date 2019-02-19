//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Handling for GAMEDEFS.
//
//-----------------------------------------------------------------------------

#include "Defs/Gamedefs.hpp"

#include "Core/NTS.hpp"

#include <GDCC/Core/Exception.hpp>


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Defs
{
   GamedefsGroup &Gamedefs::Root = Gamedefs::GetRoot();
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Defs
{
   //
   // Gamedefs constructor
   //
   Gamedefs::Gamedefs() :
      name{nullptr},
      link{this},
      group{nullptr}
   {
   }

   //
   // Gamedefs constructor
   //
   Gamedefs::Gamedefs(GamedefsGroup *group_, Core::HashedStr name_) :
      name{name_},
      link{this},
      group{nullptr}
   {
      insert(group_);
   }

   //
   // Gamedefs destructor
   //
   Gamedefs::~Gamedefs()
   {
      if(group) unlink();
   }

   //
   // Gamedefs::insert
   //
   void Gamedefs::insert(GamedefsGroup *group_)
   {
      (group = group_)->defs.insert(this);
   }

   //
   // Gamedefs::unlink
   //
   void Gamedefs::unlink()
   {
      group->defs.unlink(this);
      group = nullptr;
   }

   //
   // Gamedefs::GetRoot
   //
   GamedefsGroup &Gamedefs::GetRoot()
   {
      static GamedefsGroup root{};
      return root;
   }

   //
   // GamedefsCall::process
   //
   void GamedefsCall::process(GamedefsParserData const *data)
   {
      if(funcB) if(auto block = dynamic_cast<GamedefsParserBlock const *>(data))
         return funcB(block);

      if(funcV) if(auto value = dynamic_cast<GamedefsParserValue const *>(data))
         return funcV(value);

      if(func)
         return func(data);
   }

   //
   // GamedefsGroup destructor
   //
   GamedefsGroup::~GamedefsGroup()
   {
      for(auto &def : defs)
         def.group = nullptr;
   }

   //
   // GamedefsGroup::process
   //
   void GamedefsGroup::process(GamedefsParserData const *data)
   {
      auto def = defs.find(data->name);
      if(!def)
         GDCC::Core::ErrorExpect({}, "GAMEDEFS name", data->name);

      if(auto block = dynamic_cast<GamedefsParserBlock const *>(data))
      {
         for(auto &sub : block->data)
            def->process(sub.get());
      }
      else
         def->process(data);
   }

   //
   // GamedefsParser::get
   //
   std::unique_ptr<GamedefsParserData> GamedefsParser::get()
   {
      if(!in)
         return nullptr;

      Core::HashedStr name = in.get();

      if(in.drop("=")) return getValue(name);
      if(in.drop("{")) return getBlock(name);

      GDCC::Core::ErrorExpect({}, "GAMEDEFS data", in.peek());
   }

   //
   // GamedefsParser::getBlock
   //
   std::unique_ptr<GamedefsParserBlock> GamedefsParser::getBlock(Core::HashedStr name)
   {
      std::vector<std::unique_ptr<GamedefsParserData>> buf;

      while(!in.drop("}"))
      {
         if(!in)
            GDCC::Core::ErrorExpect({}, "}", "EOF", true, false);

         buf.push_back(get());
      }

      return std::unique_ptr<GamedefsParserBlock>{
         new GamedefsParserBlock{name, {GDCC::Core::Move, buf.begin(), buf.end()}}};
   }

   //
   // GamedefsParser::getValue
   //
   std::unique_ptr<GamedefsParserValue> GamedefsParser::getValue(Core::HashedStr name)
   {
      std::vector<Core::HashedStr> buf;

      while(!in.drop(";"))
      {
         if(!in)
            GDCC::Core::ErrorExpect({}, ";", "EOF", true, false);

         buf.push_back(in.get());
      }

      return std::unique_ptr<GamedefsParserValue>{
         new GamedefsParserValue{name, {GDCC::Core::Move, buf.begin(), buf.end()}}};
   }

   //
   // GamedefsParser::GetBool
   //
   bool GamedefsParser::GetBool(char const *value)
   {
      if(!std::strcmp(value, "false") || !std::strcmp(value, "0"))
         return false;

      if(!std::strcmp(value, "true") || !std::strcmp(value, "1"))
         return true;

      GDCC::Core::ErrorExpect({}, "boolean", value);
   }
}

// EOF

