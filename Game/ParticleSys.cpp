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

#include "Game/ParticleSys.hpp"

#include "Code/ConvertStruct.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Code_NativeLoaderDefn(ParticleSys);
   DGE_Game_ThinkerImplement(ParticleSys);

   //
   // ParticleSys constructor
   //
   ParticleSys::ParticleSys(std::size_t npr) :
      x{0}, y{0},
      tex{0},
      particles(npr),
      pinactive{0}, pactive{-1}
   {
      std::ptrdiff_t i = 0;

      for(auto &prt : particles)
         prt.next = ++i;

      particles.back().next = -1;
   }

   //
   // ParticleSys::add
   //
   Particle *ParticleSys::add()
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
   // ParticleSys::think
   //
   void ParticleSys::think()
   {
      Super::think();

      std::ptrdiff_t prev = -1;

      for(std::ptrdiff_t i = pactive; i != -1;)
      {
         auto cur = i;
         auto &p = particles.at(i);

         i = p.next;

         if(p.colorspeed)
            p.color.interpolate(p.colordest, p.colorspeed);

         if(--p.life <= 0 || p.color.a < 0.01f)
         {
            p = Particle();

            if(prev != -1) particles.at(prev).next = i;
            else           pactive = i;

            p.next = pinactive;
            pinactive = cur;
            continue;
         }

         if(p.vel) {p.oldpos = p.pos; p.pos += p.vel;}
         if(p.accel) p.vel += p.accel;
         if(p.sizespeed) p.size.interpolate(p.sizedest, p.sizespeed);
         if(p. rotspeed) p.rot += p.rotspeed;

         prev = cur;
      }
   }
}

//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // void DGE_ParticleSys_Add(unsigned id, DGE_Particle prt)
   //
   DGE_Code_NativeDefn(DGE_ParticleSys_Add)
   {
      auto th = Game::ParticleSys::Get(argv[0]);

      if(th) if(auto prt = th->add())
      {
         prt->life       = argv[1];
         prt->oldpos     =
         prt->pos        = Code::Point2ToHost(argv[2 ], argv[3 ]);
         prt->vel        = Code::Point2ToHost(argv[4 ], argv[5 ]);
         prt->accel      = Code::Point2ToHost(argv[6 ], argv[7 ]);
         prt->size       = Code::Point2ToHost(argv[8 ], argv[9 ]);
         prt->sizedest   = Code::Point2ToHost(argv[10], argv[11]);
         prt->sizespeed  = Code::SAccumToHost(argv[12]);
         prt->color      = Code::ColorToHost (argv[13], argv[14], argv[15], argv[16]);
         prt->colordest  = Code::ColorToHost (argv[17], argv[18], argv[19], argv[20]);
         prt->colorspeed = Code::SAccumToHost(argv[21]);
         prt->rot        = Code::SAccumToHost(argv[22]);
         prt->rotspeed   = Code::SAccumToHost(argv[23]);
      }

      return false;
   }

   //
   // unsigned DGE_ParticleSys_Create(unsigned ext, unsigned npr)
   //
   DGE_Code_NativeDefn(DGE_ParticleSys_Create)
   {
      std::size_t ext = argv[0];
      std::size_t npr = argv[1];

      task->dataStk.push((new(ext) ParticleSys(npr))->id);
      return false;
   }
}

// EOF

