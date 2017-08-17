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
   //
   // Team::getEntFirst
   //
   Code::Word Team::getEntFirst() const
   {
      return listEnt.next->obj ? listEnt.next->obj->id : 0;
   }

   //
   // Team::getEntLast
   //
   Code::Word Team::getEntLast() const
   {
      return listEnt.prev->obj ? listEnt.prev->obj->id : 0;
   }

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
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) Team)->id);
      return false;
   }
}

// EOF

