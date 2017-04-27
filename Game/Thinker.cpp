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
   //
   // Thinker constructor
   //
   Thinker::Thinker() :
      next{ThinkerCap.next},
      prev{&ThinkerCap}
   {
      prev->next = next->prev = this;
   }

   //
   // ThinkerCap constructor
   //
   Thinker::Thinker(int) :
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
   // void DGE_ThinkerUnlink(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ThinkerUnlink)
   {
      if(Thinker *th = Thinker::Get(argv[0])) th->unlink();
      return false;
   }
}

// EOF

