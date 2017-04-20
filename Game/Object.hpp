//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Playsim objects
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Object_H__
#define DGE__Game__Object_H__

#include "Core/IDAllocator.hpp"

#include "Code/Types.hpp"

#include <GDCC/Core/Counter.hpp>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_ObjectPreamble
//
#define DGE_Game_ObjectPreamble(name, base) \
   GDCC_Core_CounterPreambleNoClone(DGE::Game::name, DGE::Game::base); \
   DGE_Game_ObjectPreambleCommon(name)

//
// DGE_Game_ObjectPreambleCommon
//
#define DGE_Game_ObjectPreambleCommon(name) \
public: \
   static This *Get(Code::Word id) \
      {return dynamic_cast<This *>(ObjectVec[id]);}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // Object
   //
   class Object : public GDCC::Core::Counter
   {
      GDCC_Core_CounterPreambleNoClone(DGE::Game::Object, GDCC::Core::Counter);
      DGE_Game_ObjectPreambleCommon(Object);

   public:
      void refAdd() {++refCount;}

      void refSub() {if(!--refCount) delete this;}

      Code::Word id;

   protected:
      Object();
      virtual ~Object();


      static Core::IDAllocator<Object, Code::Word> ObjectVec;
   };
}

#endif//DGE__Game__Object_H__

