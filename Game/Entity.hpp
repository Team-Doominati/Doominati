//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Reactive Thinkers.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Entity_H__
#define DGE__Game__Entity_H__

#include "Game/PhysicsThinker.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Entity_GetMemberCases
//
#define DGE_Game_Entity_GetMemberCases() \
   DGE_Game_PhysicsThinker_GetMemberCases()

//
// DGE_Game_Entity_SetMemberCases
//
#define DGE_Game_Entity_SetMemberCases() \
   DGE_Game_PhysicsThinker_SetMemberCases()


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // Entity
   //
   class Entity : public PhysicsThinker
   {
      DGE_Game_ThinkerPreamble(Entity, PhysicsThinker);

   public:
      Entity() {}

      virtual void think();
   };
}

#endif//DGE__Game__Entity_H__
