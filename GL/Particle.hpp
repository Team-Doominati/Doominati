//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Particle systems.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Particle_H__
#define DGE__GL__Particle_H__

#include "Core/Color.hpp"
#include "Core/Vector2.hpp"
#include "Core/Math.hpp"

#include <GDCC/Core/Array.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
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

      float rot = 0;
      float rotspeed = 0;

      Core::Color color{};
      Core::Color colordest{};
      float       colorspeed = 0;

      std::ptrdiff_t next = -1;
   };

   class ParticleSystem
   {
   public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW

      ParticleSystem() : ParticleSystem{0, 0, 128, 0} {}
      ParticleSystem(float x, float y, std::ptrdiff_t pnum, std::size_t tex_);

      Particle *create();
      void setPosition(float x, float y);
      void update();

      std::size_t tex;
      Core::Matrix4 mat;
      GDCC::Core::Array<Particle> particles;

   private:
      std::ptrdiff_t pinactive, pactive;
   };
}

#endif//DGE__GL__Particle_H__

