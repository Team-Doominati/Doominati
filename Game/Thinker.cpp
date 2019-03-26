//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
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
   Thinker::Ref const Thinker::Head{new Thinker};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ThinkerImplementCommon(Thinker);

   //
   // Head constructor
   //
   Thinker::Thinker() :
      Object{nullptr, 0},
      next{this},
      prev{this}
   {
   }

   //
   // Thinker constructor
   //
   Thinker::Thinker(Code::Word *emv_, std::size_t emc_) :
      Object{emv_, emc_},
      next{Head->next},
      prev{Head}
   {
      prev->next = next->prev = this;
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
      next = Head->next;
      prev = Head;

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
      task->dataStk.push(Thinker::Head->id);
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
   // void DGE_Thinker_Think(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Thinker_Think)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->think();
      return false;
   }

   //
   // void DGE_Thinker_ThinkAll(void)
   //
   DGE_Code_NativeDefn(DGE_Thinker_ThinkAll)
   {
      Thinker::Ref next{Thinker::Head};

      // Process all active Thinkers.
      for(Thinker *th = next->next; th != Thinker::Head; th = next)
      {
         next = static_cast<Thinker::Ref>(th->next);
         th->think();
      }

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

