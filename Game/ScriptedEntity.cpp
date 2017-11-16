//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script-controlled Entities.
//
//-----------------------------------------------------------------------------

#include "Game/ScriptedEntity.hpp"

#include "Game/Sector.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Code_NativeLoaderDefn(ScriptedEntity);
   DGE_Game_ThinkerImplement(ScriptedEntity);

   //
   // ScriptedEntity::collideFrom
   //
   bool ScriptedEntity::collideFrom(PhysicsThinker *th)
   {
      return cbCollideF ? cbCollideF(id, th->id) : Super::collideFrom(th);
   }

   //
   // ScriptedEntity::collideInto
   //
   bool ScriptedEntity::collideInto(PhysicsThinker *th)
   {
      return cbCollideI ? cbCollideI(id, th->id) : Super::collideInto(th);
   }

   //
   // ScriptedEntity::collideInto
   //
   bool ScriptedEntity::collideInto(Sector *sec)
   {
      return cbCollideI ? cbCollideI(id, sec->id) : Super::collideInto(sec);
   }

   //
   // ScriptedEntity::think
   //
   void ScriptedEntity::think()
   {
      if(cbThink)
      {
         Super::Super::think();
         cbThink(id);
      }
      else
         Super::think();
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_ScriptedEntity_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_ScriptedEntity_Create)
   {
      task->dataStk.push(ScriptedEntity::Create(argv[0])->id);
      return false;
   }
}

// EOF

