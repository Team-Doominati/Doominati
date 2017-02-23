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
   // Thinker::getMember
   //
   Code::Word Thinker::getMember(ThinkerMember mem)
   {
      switch(mem)
      {
         DGE_Game_ThinkerMemberCases();

      default:
         return 0;
      }
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

// EOF

