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

#ifndef Doom__GL__Particle_H__
#define Doom__GL__Particle_H__

#include "GL/Color.hpp"
#include "Core/Vector2.hpp"
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace GL
   {
      class Particle
      {
      public:
         Particle() = default;
         Particle(std::ptrdiff_t next_) : next{next_} {}

         int life;

         Core::Vector2 oldposition;
         Core::Vector2 position;
         Core::Vector2 velocity;
         Core::Vector2 acceleration;

         Core::Vector2 scale;
         Core::Vector2 scaledest;
         float scalespeed;

         float rot;
         float rotspeed;

         Color color;
         Color colordest;
         float colorspeed;

         std::ptrdiff_t next = -1;
      };

      class ParticleSystem
      {
         friend class Window;

      public:
         Core::Vector2 position;

         ParticleSystem();
         ParticleSystem(float x, float y, std::ptrdiff_t pnum = 128);

         void expand();
         Particle *create();
         void update();

      protected:
         std::vector<Particle> particles;

      private:
         std::ptrdiff_t pinactive, pactive;
      };
   }
}

#endif//Doom__GL__Particle_H__

