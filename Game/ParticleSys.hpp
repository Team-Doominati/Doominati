//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Particle systems.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__ParticleSys_H__
#define DGE__Game__ParticleSys_H__

#include "Game/Thinker.hpp"
#include "Game/Types.hpp"

#include "Code/Convert.hpp"

#include "Core/Color.hpp"
#include "Core/Math.hpp"
#include "Core/Vector2.hpp"

#include <GDCC/Core/Array.hpp>

//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_ParticleSys_GetMemberCases
//
#define DGE_Game_ParticleSys_GetMemberCases() \
   DGE_Game_Thinker_GetMemberCases(); \
   case ObjectMember::sprite: return tex; \
   case ObjectMember::x:      return x.raw(); \
   case ObjectMember::y:      return y.raw()

//
// DGE_Game_ParticleSys_SetMemberCases
//
#define DGE_Game_ParticleSys_SetMemberCases() \
   DGE_Game_Thinker_SetMemberCases(); \
   case ObjectMember::sprite: tex = val;  break; \
   case ObjectMember::x:      x.raw(val); break; \
   case ObjectMember::y:      y.raw(val); break

//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // Particle
   //
   struct Particle
   {
      Particle() = default;

      int life = 0;

      Core::Vector2 oldpos{};
      Core::Vector2 pos{};

      Core::Vector2 vel{};
      Core::Vector2 accel{};

      Core::Vector2 size{};
      Core::Vector2 sizedest{};
      float         sizespeed = 0;

      Core::Color color{};
      Core::Color colordest{};
      float       colorspeed = 0;

      float rot = 0;
      float rotspeed = 0;

      std::ptrdiff_t next = -1;
   };

   //
   // ParticleSys
   //
   class ParticleSys : public Thinker
   {
      DGE_Game_ThinkerPreamble(ParticleSys, Thinker);

   public:
      ParticleSys() = delete;
      ParticleSys(std::size_t npr);

      Particle *add();

      Coord x, y;
      std::size_t tex;
      GDCC::Core::Array<Particle> particles;

   protected:
      virtual void think();

   private:
      std::ptrdiff_t pinactive, pactive;
   };
}

#endif//DGE__Game__ParticleSys_H__

