//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Playsim state advancement.
//
//-----------------------------------------------------------------------------

#include "Game/Thinker.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Game
{
   Core::IDAllocator<Thinker, Code::Word> Thinker::ThinkerVec;

   Thinker Thinker::ThinkerCap{0};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // Thinker constructor
   //
   Thinker::Thinker() :
      next{ThinkerCap.next},
      prev{&ThinkerCap},
      id{ThinkerVec.alloc(this)}
   {
      prev->next = next->prev = this;
   }

   //
   // ThinkerCap constructor
   //
   Thinker::Thinker(int) :
      next{this},
      prev{this},
      id{ThinkerVec.alloc(this)} // This should give TID 0.
   {
      ++refCount; // Must never free ThinkerCap.
   }

   //
   // Thinker destructor
   //
   Thinker::~Thinker()
   {
      ThinkerVec.free(id);
   }

   //
   // Thinker::unlink
   //
   void Thinker::unlink()
   {
      // Hold a reference to self while unlinking,
      Ref self(this);

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
   // T DGE_ThinkerMemberGet*(unsigned id, unsigned mem)
   //
   #define DGE_Game_ThinkerMemberGetDefn(suffix) \
      DGE_Code_NativeDefn(DGE_ThinkerMemberGet##suffix) \
      { \
         Thinker *th  = Thinker::Get(argv[0]); \
         auto     mem = static_cast<ThinkerMember>(argv[1]); \
         \
         task->dataStk.push(th ? th->getMember(mem) : 0); \
         return false; \
      }

   DGE_Game_ThinkerMemberGetDefn(LA)
   DGE_Game_ThinkerMemberGetDefn(U)
   DGE_Game_ThinkerMemberGetDefn(X)

   //
   // void DGE_ThinkerMemberSet*(unsigned id, unsigned mem, T val)
   //
   #define DGE_Game_ThinkerMemberSetDefn(suffix) \
      DGE_Code_NativeDefn(DGE_ThinkerMemberSet##suffix) \
      { \
         Thinker *th  = Thinker::Get(argv[0]); \
         auto     mem = static_cast<ThinkerMember>(argv[1]); \
         \
         if(th) th->setMember(mem, argv[2]); \
         return false; \
      }

   DGE_Game_ThinkerMemberSetDefn(LA)
   DGE_Game_ThinkerMemberSetDefn(U)
   DGE_Game_ThinkerMemberSetDefn(X)

   //
   // void DGE_ThinkerRefAdd(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ThinkerRefAdd)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->refAdd();
      return false;
   }

   //
   // void DGE_ThinkerRefSub(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ThinkerRefSub)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->refSub();
      return false;
   }

   //
   // void DGE_ThinkerUnlink(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ThinkerUnlink)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->unlink();
      return false;
   }
}

// EOF

