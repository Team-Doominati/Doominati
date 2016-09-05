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
         unsigned char life;
         
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
      };
      
      class ParticleSystem
      {
      public:
         Core::Vector2 position;
         std::vector<Particle> particles;
         
         ParticleSystem();
         ParticleSystem(float x, float y, std::size_t nparticles = 128);
         
         void update();
      };
   }
}

#endif//Doom__GL__Particle_H__

