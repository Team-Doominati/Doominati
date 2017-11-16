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

#include "Game/Team.hpp"

#include "Game/Entity.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ObjectImplement(Team);

   //
   // Team::getOwnerTop
   //
   Team const *Team::getOwnerTop() const
   {
      Team const *team = this;
      while(team->owner) team = team->owner;
      return team;
   }

   //
   // Team::operator == Team
   //
   bool Team::operator == (Team const &team) const
   {
      return this == &team || getOwnerTop() == team.getOwnerTop();
   }

   //
   // Team::setOwner
   //
   void Team::setOwner(Code::Word teamID)
   {
      teamLink.unlink();
      if((owner = Team::Get(teamID)))
         teamLink.insert(&owner->listTeam);
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_Team_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_Team_Create)
   {
      task->dataStk.push(Team::Create(argv[0])->id);
      return false;
   }
}

// EOF

