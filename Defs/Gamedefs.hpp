//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Handling for GAMEDEFS.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Defs__Gamedefs_H__
#define DGE__Defs__Gamedefs_H__

#include "../Defs/Types.hpp"

#include "../Core/HashMap.hpp"
#include "../Core/String.hpp"

#include <GDCC/Core/Array.hpp>

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Defs
{
   //
   // Gamedefs
   //
   class Gamedefs
   {
   public:
      Gamedefs();
      Gamedefs(GamedefsGroup *group, Core::HashedStr name);
      virtual ~Gamedefs();

      void insert(GamedefsGroup *group);

      virtual void process(GamedefsParserData const *data) = 0;

      void unlink();

      Core::HashedStr const name;


      friend class GamedefsGroup;

      static GamedefsGroup &GetRoot();

      static GamedefsGroup &Root;

   private:
      Core::ListLink<Gamedefs> link;
      GamedefsGroup           *group;
   };

   //
   // GamedefsCall
   //
   class GamedefsCall : public Gamedefs
   {
   private:
      using Func  = void (*)(GamedefsParserData const *);
      using FuncB = void (*)(GamedefsParserBlock const *);
      using FuncV = void (*)(GamedefsParserValue const *);

   public:
      GamedefsCall(GamedefsGroup *group_, Core::HashedStr name_, Func func_) :
         Gamedefs{group_, name_}, func{func_} {}
      GamedefsCall(GamedefsGroup *group_, Core::HashedStr name_, FuncB funcB_) :
         Gamedefs{group_, name_}, funcB{funcB_} {}
      GamedefsCall(GamedefsGroup *group_, Core::HashedStr name_, FuncV funcV_) :
         Gamedefs{group_, name_}, funcV{funcV_} {}

      virtual void process(GamedefsParserData const *data);

   private:
      Func  const func  = nullptr;
      FuncB const funcB = nullptr;
      FuncV const funcV = nullptr;
   };

   //
   // GamedefsGroup
   //
   class GamedefsGroup : public Gamedefs
   {
   public:
      GamedefsGroup() : Gamedefs{} {}
      GamedefsGroup(GamedefsGroup *group_, Core::HashedStr name_) :
         Gamedefs{group_, name_} {}
      virtual ~GamedefsGroup();

      virtual void process(GamedefsParserData const *data);


      friend class Gamedefs;

   private:
      Core::HashMapKeyMem<Core::HashedStr, Gamedefs, &Gamedefs::name, &Gamedefs::link> defs;
   };

   //
   // GamedefsParser
   //
   class GamedefsParser
   {
   public:
      GamedefsParser(Core::NTSStream &in_) : in{in_} {}


      std::unique_ptr<GamedefsParserData> get();

   private:
      std::unique_ptr<GamedefsParserBlock> getBlock(Core::HashedStr name);
      std::unique_ptr<GamedefsParserValue> getValue(Core::HashedStr name);

      Core::NTSStream &in;
   };

   //
   // GamedefsParserData
   //
   class GamedefsParserData
   {
   public:
      explicit GamedefsParserData(Core::HashedStr name_) : name{name_} {}
      virtual ~GamedefsParserData() {}

      Core::HashedStr name;
   };

   //
   // GamedefsParserBlock
   //
   class GamedefsParserBlock : public GamedefsParserData
   {
   public:
      using Data = GDCC::Core::Array<std::unique_ptr<GamedefsParserData>>;


      GamedefsParserBlock(Core::HashedStr name_, Data &&data_) :
         GamedefsParserData{name_}, data{std::move(data_)} {}

      Data data;
   };

   //
   // GamedefsParserValue
   //
   class GamedefsParserValue : public GamedefsParserData
   {
   public:
      using Data = GDCC::Core::Array<Core::HashedStr>;


      GamedefsParserValue(Core::HashedStr name_, Data &&data_) :
         GamedefsParserData{name_}, data{std::move(data_)} {}

      Data data;
   };
}

#endif//DGE__Defs__Gamedefs_H__

