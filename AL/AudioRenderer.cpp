//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Audio renderer handling.
//
//-----------------------------------------------------------------------------

#include "AL/AudioRenderer.hpp"

#include "AL/Sound.hpp"
#include "AL/SoundSourceFixed.hpp"

#include "Code/Convert.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include <cmath>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::AL
{
   static AudioRenderer *CurrentAudioRenderer;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // AudioRenderer::AudioRenderer
   //
   AudioRenderer::AudioRenderer() :
      sndMan{},

      sndSrcGbl{DGE_AL_GlobalSound},
      sndSrcMap{},
      sndSrcId{DGE_AL_GlobalSound + 1},

      device{},
      context{}
   {
      // AUDIO_TODO: multiple device detection and setting with enumeration ext
      device = alcOpenDevice(nullptr);

      // We have to have a device, but any other errors can be ignored.
      if(!device)
         throw AudioInitError("AudioRenderer: Couldn't open device.");

      context = alcCreateContext(device, nullptr);
      alcMakeContextCurrent(context);

      // Set up a null sound.
      soundGet_None("SNDNULL");
   }

   //
   // AudioRenderer::~AudioRenderer
   //
   AudioRenderer::~AudioRenderer()
   {
      if(CurrentAudioRenderer == this)
         alcMakeContextCurrent(nullptr);

      alcDestroyContext(context);
      alcCloseDevice(device);
   }

   //
   // AudioRenderer::listenerPos
   //
   void AudioRenderer::listenerPos(float x, float y, float z)
   {
      alListener3f(AL_POSITION, x, y, z);
   }

   //
   // AudioRenderer::listenerVel
   //
   void AudioRenderer::listenerVel(float x, float y, float z)
   {
      alListener3f(AL_VELOCITY, x, y, z);
   }

   //
   // AudioRenderer::listenerAng
   //
   void AudioRenderer::listenerAng(float ang)
   {
      ALfloat orientation[6] = {std::cos(ang), 0, -std::sin(ang), 0, 1, 0};
      alListenerfv(AL_ORIENTATION, orientation);
   }

   //
   // AudioRenderer::soundSrcCreate
   //
   SoundSource *AudioRenderer::soundSrcCreate(float x, float y, float z)
   {
      auto src = new SoundSourceFixed<8>(sndSrcId++);
      src->setPos(x, y, z);
      sndSrcMap.insert(src);
      return src;
   }

   //
   // AudioRenderer::soundSrcDestroy
   //
   void AudioRenderer::soundSrcDestroy(unsigned id)
   {
      if(auto *src = sndSrcMap.find(id))
      {
         sndSrcMap.unlink(src);
         delete src;
      }
   }

   //
   // AudioRenderer::soundSrcGet
   //
   SoundSource *AudioRenderer::soundSrcGet(unsigned id)
   {
      if(id == DGE_AL_GlobalSound) return &sndSrcGbl;
      else                         return sndSrcMap.find(id);
   }

   //
   // AudioRenderer::dopplerSpeed
   //
   void AudioRenderer::dopplerSpeed(float speed)
   {
      alSpeedOfSound(speed);
   }

   //
   // AudioRenderer::GetCurrent
   //
   AudioRenderer *AudioRenderer::GetCurrent()
   {
      return CurrentAudioRenderer;
   }

   //
   // AudioRenderer::SetCurrent
   //
   void AudioRenderer::SetCurrent(AudioRenderer *audio)
   {
      CurrentAudioRenderer = audio;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::AL
{
   // AUDIO_TODO: unsigned DGE_SoundPlayingNum(name)
   //             gets from all sound sources, may be bad on performance
   // AUDIO_TODO: think of something for reverb and sound modulation

   //
   // void DGE_Audio_SetDopplerSpeed(short _Accum meterspersecond)
   //
   DGE_Code_NativeDefn(DGE_Audio_SetDopplerSpeed)
   {
      AudioRenderer::GetCurrent()->dopplerSpeed(Code::SAccumToHost(argv[0]));
      return false;
   }

   //
   // void DGE_Audio_SetListener(short _Accum x, y, z[, velx, vely, velz, angle])
   //
   DGE_Code_NativeDefn(DGE_Audio_SetListener)
   {
      auto *audio = AudioRenderer::GetCurrent();

      auto x = Code::SAccumToHost(argv[0]);
      auto y = Code::SAccumToHost(argv[1]);
      auto z = Code::SAccumToHost(argv[2]);

      audio->listenerPos(x, y, z);

      if(argc > 3)
      {
         auto velx =            Code::SAccumToHost(argv[3]);
         auto vely = argc > 4 ? Code::SAccumToHost(argv[4]) : 0;
         auto velz = argc > 5 ? Code::SAccumToHost(argv[5]) : 0;

         audio->listenerVel(velx, vely, velz);
      }

      if(argc > 6)
         audio->listenerAng(Code::SAccumToHost(argv[6]));

      return false;
   }

   //
   // unsigned DGE_Sound_Get(__str_ent *name)
   //
   DGE_Code_NativeDefn(DGE_Sound_Get)
   {
      GDCC::Core::String str{argv[0]};
      task->dataStk.push(AudioRenderer::GetCurrent()->soundGetIdx(str));
      return false;
   }
}

// EOF

