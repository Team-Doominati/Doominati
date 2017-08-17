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

#include "Game/Team.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Entity_GetMemberCases
//
#define DGE_Game_Entity_GetMemberCases() \
   DGE_Game_PhysicsThinker_GetMemberCases(); \
   case ObjectMember::health: return health; \
   case ObjectMember::team:   return team ? team->id : 0; \
   case ObjectMember::teamne: return teamLink.next->obj ? teamLink.next->obj->id : 0; \
   case ObjectMember::teampr: return teamLink.prev->obj ? teamLink.prev->obj->id : 0

//
// DGE_Game_Entity_SetMemberCases
//
#define DGE_Game_Entity_SetMemberCases() \
   DGE_Game_PhysicsThinker_SetMemberCases(); \
   case ObjectMember::health: health = val; break; \
   case ObjectMember::team:   setTeam(val); break; \
   case ObjectMember::teamne: (void)val;    break; \
   case ObjectMember::teampr: (void)val;    break


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
      Entity() : teamLink{this}, health{0} {}

      void setTeam(Code::Word id);

      Core::ListLink<Entity> teamLink;

      Team::Ptr team;

      std::int32_t health;

   protected:
      virtual void think();
   };
}

#endif//DGE__Game__Entity_H__

