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

#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // ParticleSystem constructor
   //

   ParticleSystem::ParticleSystem() :
      ParticleSystem{0.0f, 0.0f}
   {
   }

   ParticleSystem::ParticleSystem(float x, float y, std::ptrdiff_t pnum, char const *texture) :
      mat{},
      particles{static_cast<decltype(particles)::size_type>(pnum)},
      texname{nullptr},
      pinactive{0}, pactive{-1}
   {
      setPosition(x, y);
      setTexture(texture);

      std::ptrdiff_t i = 0;
      for(auto it = particles.begin(); it != particles.end(); ++it)
         it->next = ++i;

      particles.back().next = -1;
   }

   //
   // ParticleSystem::create
   //

   Particle *ParticleSystem::create()
   {
      std::ptrdiff_t i = pinactive;

      if(i != -1)
      {
         auto &pp = particles.at(i);

         pinactive = pp.next;
         pp.next   = pactive;
         pactive   = i;

         return &pp;
      }

      return nullptr;
   }

   //
   // ParticleSystem::setPosition
   //

   void ParticleSystem::setPosition(float x, float y)
   {
      mat = Eigen::Affine3f{Eigen::Translation3f(x, y, 0.0f)}.matrix();
   }

   //
   // ParticleSystem::setTexture
   //

   void ParticleSystem::setTexture(char const *texture)
   {
      texname.reset(GDCC::Core::StrDup(texture).release());
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
         auto &p = particles.at(i);

         i = p.next;

         if(p.colorspeed)
            p.color.interpolate(p.colordest, p.colorspeed);

         if(--p.life <= 0 || p.color.a <= 0.01f)
         {
            std::memset(&p, 0, sizeof(Particle));

            if(prev != -1)
               particles.at(prev).next = i;
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

// EOF

