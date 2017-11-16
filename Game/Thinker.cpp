//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Objects that change over time.
//
//-----------------------------------------------------------------------------

#include "Game/Thinker.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include <string>


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Game
{
   Thinker Thinker::ThinkerCap{0};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ThinkerImplementCommon(Thinker);

   //
   // Thinker constructor
   //
   Thinker::Thinker(Code::Word *emv_, std::size_t emc_) :
      Object{emv_, emc_},
      next{ThinkerCap.next},
      prev{&ThinkerCap}
   {
      prev->next = next->prev = this;
   }

   //
   // ThinkerCap constructor
   //
   Thinker::Thinker(int) :
      Object{nullptr, 0},
      next{this},
      prev{this}
   {
      ++refCount; // Must never free ThinkerCap.
   }

   //
   // Thinker destructor
   //
   Thinker::~Thinker()
   {
      if(next)
         unlink();
   }

   //
   // Thinker::insert
   //
   void Thinker::insert()
   {
      next = ThinkerCap.next;
      prev = &ThinkerCap;

      prev->next = next->prev = this;
   }

   //
   // Thinker::unlink
   //
   void Thinker::unlink()
   {
      // Hold a reference to self while unlinking.
      Ref self{this};

      prev->next = next;
      next->prev = prev;

      next = nullptr;
      prev = nullptr;
   }

   //
   // Thinker::ThinkAll
   //
   void Thinker::ThinkAll()
   {
      Ref next(&ThinkerCap);

      // Process all active Thinkers.
      for(Thinker *th = ThinkerCap.next; th != &ThinkerCap; th = next)
      {
         next = static_cast<Ref>(th->next);
         th->think();
      }
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_Thinker_Head(void)
   //
   DGE_Code_NativeDefn(DGE_Thinker_Head)
   {
      task->dataStk.push(Thinker::ThinkerCap.id);
      return false;
   }

   //
   // void DGE_Thinker_Insert(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Thinker_Insert)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->insert();
      return false;
   }

   //
   // void DGE_Thinker_Unlink(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Thinker_Unlink)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->unlink();
      return false;
   }
}

// EOF

