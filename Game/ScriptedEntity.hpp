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

#ifndef DGE__Game__ScriptedEntity_H__
#define DGE__Game__ScriptedEntity_H__

#include "../Game/Entity.hpp"

#include "../Code/FuncPtr.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_ScriptedEntity_GetMemberCases
//
#define DGE_Game_ScriptedEntity_GetMemberCases() \
   DGE_Game_Entity_GetMemberCases(); \
   case ObjectMember::collideF: return static_cast<Code::Word>(cbCollideF); \
   case ObjectMember::collideI: return static_cast<Code::Word>(cbCollideI); \
   case ObjectMember::think:    return static_cast<Code::Word>(cbThink)

//
// DGE_Game_ScriptedEntity_SetMemberCases
//
#define DGE_Game_ScriptedEntity_SetMemberCases() \
   DGE_Game_Entity_SetMemberCases(); \
   case ObjectMember::collideF: cbCollideF = val; break; \
   case ObjectMember::collideI: cbCollideI = val; break; \
   case ObjectMember::think:    cbThink    = val; break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // ScriptedEntity
   //
   class ScriptedEntity : public Entity
   {
      DGE_Game_ThinkerPreamble(ScriptedEntity, Entity);

   public:
      virtual bool collideFrom(PhysicsThinker *th);
      virtual bool collideInto(PhysicsThinker *th);
      virtual bool collideInto(Sector *sec);

      Code::FuncPtr<Code::Word(Code::Word, Code::Word)> cbCollideF;
      Code::FuncPtr<Code::Word(Code::Word, Code::Word)> cbCollideI;

      Code::FuncPtr<void(Code::Word)> cbThink;

   protected:
      virtual void think();
   };
}

#endif//DGE__Game__ScriptedEntity_H__

