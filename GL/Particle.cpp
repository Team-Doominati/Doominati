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

#include <algorithm>


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
         int n = particles.size();
         
         for(int i = 0; i < n; i++)
         {
            auto &p = particles[i];
            
            if(p.colorspeed)
               p.color.interpolate(p.colordest, p.colorspeed);
            
            if(--p.life <= 0 || p.color.a <= 0.01f)
            {
               std::swap(particles[i], particles.back());
               particles.pop_back();
               --i, --n;
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

