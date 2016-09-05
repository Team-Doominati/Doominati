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

#include "GL/Particle.hpp"

#include <stdio.h>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // ParticleSystem constructor
      //
      
      ParticleSystem::ParticleSystem() :
         ParticleSystem{0.0f, 0.0f, 128}
      {
      }
      
      ParticleSystem::ParticleSystem(float x, float y, std::size_t nparticles) :
         position{x, y},
         particles{}
      {
         particles.reserve(nparticles);
      }
      
      //
      // ParticleSystem::update
      //
      
      void ParticleSystem::update()
      {
         for(auto it = particles.begin(); it != particles.end(); ++it)
         {
            auto &p = *it;
            
            if(p.colorspeed)
               p.color.interpolate(p.colordest, p.colorspeed);
            
            if(--p.life <= 0 || p.color.a <= 0.01f)
            {
               it = particles.erase(it);
               continue;
            }
            
            if(p.acceleration)
            {
               p.oldposition = p.position;
               p.position += p.velocity;
               p.velocity += p.acceleration;
            }
            
            if(p.scalespeed)
               p.scale.interpolate(p.scaledest, p.scalespeed);
            
            if(p.rotspeed)
               p.rot += p.rotspeed;
         }
      }
   }
}

// EOF

