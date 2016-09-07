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
#include <cstring>


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

      ParticleSystem::ParticleSystem(float x, float y, std::ptrdiff_t pnum) :
         mat{},
         particles{},
         pinactive{0}, pactive{-1}
      {
         setPosition(x, y);

         particles.reserve(particles.capacity() + pnum);

         for(std::ptrdiff_t i = 0; i < pnum - 1; i++)
            particles.emplace_back(i + 1);

         particles.emplace_back(-1);
      }

      //
      // ParticleSystem::setPosition
      //

      void ParticleSystem::setPosition(float x, float y)
      {
         mat.translate(x, y);
      }

      //
      // ParticleSystem::create
      //

      Particle *ParticleSystem::create()
      {
         std::ptrdiff_t i = pinactive;

         if(i != -1)
         {
            auto &pp = particles[i];

            pinactive = pp.next;
            pp.next   = pactive;
            pactive   = i;

            return &pp;
         }

         return nullptr;
      }

      //
      // ParticleSystem::update
      //

      void ParticleSystem::update()
      {
         std::ptrdiff_t prev = -1;

         for(std::ptrdiff_t i = pactive; i != -1;)
         {
            auto cur = i;
            auto &p = particles[i];

            i = p.next;

            if(p.colorspeed)
               p.color.interpolate(p.colordest, p.colorspeed);

            if(--p.life <= 0 || p.color.a <= 0.01f)
            {
               std::memset(&p, 0, sizeof(p));

               if(prev != -1)
                  particles[prev].next = i;
               else
                  pactive = i;

               p.next = pinactive;
               pinactive = cur;

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

            prev = cur;
         }
      }
   }
}

// EOF

