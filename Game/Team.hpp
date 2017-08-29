//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Entity teams.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Team_H__
#define DGE__Game__Team_H__

#include "Game/Object.hpp"
#include "Game/Types.hpp"

#include "Core/List.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Team_GetMemberCases
//
#define DGE_Game_Team_GetMemberCases() \
   DGE_Game_Object_GetMemberCases(); \
   case ObjectMember::entfi:  return listEnt.next->obj ? listEnt.next->obj->id : 0; \
   case ObjectMember::entla:  return listEnt.prev->obj ? listEnt.prev->obj->id : 0; \
   case ObjectMember::owner:  return owner ? owner->id : 0; \
   case ObjectMember::teamfi: return listTeam.next->obj ? listTeam.next->obj->id : 0; \
   case ObjectMember::teamla: return listTeam.prev->obj ? listTeam.prev->obj->id : 0; \
   case ObjectMember::teamne: return teamLink.next->obj ? teamLink.next->obj->id : 0; \
   case ObjectMember::teampr: return teamLink.prev->obj ? teamLink.prev->obj->id : 0

//
// DGE_Game_Team_SetMemberCases
//
#define DGE_Game_Team_SetMemberCases() \
   DGE_Game_Object_SetMemberCases(); \
   case ObjectMember::entfi:  (void)val;     break; \
   case ObjectMember::entla:  (void)val;     break; \
   case ObjectMember::owner:  setOwner(val); break; \
   case ObjectMember::teamfi: (void)val;     break; \
   case ObjectMember::teamla: (void)val;     break; \
   case ObjectMember::teamne: (void)val;     break; \
   case ObjectMember::teampr: (void)val;     break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // Team
   //
   class Team : public Object
   {
      DGE_Game_ObjectPreamble(Team, Object);

   public:
      Team() : teamLink{this} {}

      Team const *getOwnerTop() const;

      void setOwner(Code::Word id);

      bool operator == (Team const &team) const;

      Core::ListLink<Entity> listEnt;
      Core::ListLink<Team>   listTeam;
      Core::ListLink<Team>   teamLink;

      Ptr owner;
   };
}

#endif//DGE__Game__Team_H__

