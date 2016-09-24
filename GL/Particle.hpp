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

#include "GL/Color.hpp"
#include "Core/Vector2.hpp"
#include "Core/Math.hpp"

#include <GDCC/Core/Array.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   class Particle
   {
      friend class ParticleSystem;

   public:
      Particle() = default;

      int life = 0;

      Core::Vector2 oldposition{};
      Core::Vector2 position{};

      Core::Vector2 velocity{};
      Core::Vector2 acceleration{};

      Core::Vector2 scale{};
      Core::Vector2 scaledest{};
      float scalespeed = 0.0f;

      float rot = 0.0f;
      float rotspeed = 0.0f;

      Color color{};
      Color colordest{};
      float colorspeed = 0.0f;

   protected:
      std::ptrdiff_t next = -1;
   };

   class ParticleSystem
   {
      friend class Window;

   public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW

      ParticleSystem();
      ParticleSystem(float x, float y, std::ptrdiff_t pnum = 128, char const *texture = "TEXNULL");

      Particle *create();
      void setPosition(float x, float y);
      void setTexture(char const *texture);
      void update();

   protected:
      Core::Matrix4 mat;
      GDCC::Core::Array<Particle> particles;
      std::unique_ptr<char[]> texname;

   private:
      std::ptrdiff_t pinactive, pactive;
   };
}

#endif//DGE__GL__Particle_H__

