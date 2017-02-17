//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Sound sources.
//
//-----------------------------------------------------------------------------

#include "AL/SoundSource.hpp"

#include "AL/AudioRenderer.hpp"
#include "AL/Sound.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // SoundSource constructor
   //
   SoundSource::SoundSource(unsigned id_) :
      id{id_},
      link{this}
   {
      alGenSources(MaxSourceChannels, sources.data());
   }

   //
   // SoundSource destructor
   //
   SoundSource::~SoundSource()
   {
      alDeleteSources(MaxSourceChannels, sources.data());
   }

   //
   // SoundSource::getFreeChannel
   //
   unsigned SoundSource::getFreeChannel()
   {
      for(unsigned i = 0; i < MaxSourceChannels; i++)
      {
         ALenum state;

         alGetSourcei(sources[i], AL_SOURCE_STATE, &state);

         if(state != AL_PLAYING)
            return i + 1;
      }

      return 0;
   }

   //
   // SoundSource::bind
   //
   void SoundSource::bind(unsigned channel, SoundData *snd)
   {
      alSourcei(getSource(channel), AL_BUFFER, snd->buf);
   }

   //
   // SoundSource::getPitch
   //
   float SoundSource::getPitch(unsigned channel)
   {
      ALfloat ret;
      alGetSourcef(getSource(channel), AL_PITCH, &ret);
      return ret;
   }

   //
   // SoundSource::play
   //
   void SoundSource::play(unsigned channel)
   {
      alSourcePlay(getSource(channel));
   }

   //
   // SoundSource::stop
   //
   void SoundSource::stop(unsigned channel)
   {
      alSourceStop(getSource(channel));
   }

   //
   // SoundSource::setLoop
   //
   void SoundSource::setLoop(unsigned channel, bool loop)
   {
      alSourcei(getSource(channel), AL_LOOPING, loop);
   }

   //
   // SoundSource::setPitch
   //
   void SoundSource::setPitch(unsigned channel, float pitch)
   {
      alSourcef(getSource(channel), AL_PITCH, pitch);
   }

   //
   // SoundSource::setPos
   //
   void SoundSource::setPos(float x, float y, float z)
   {
      for(unsigned i = 0; i < MaxSourceChannels; i++)
         alSource3f(sources[i], AL_POSITION, x, y, z);
   }

   //
   // SoundSource::setVel
   //
   void SoundSource::setVel(float x, float y, float z)
   {
      for(unsigned i = 0; i < MaxSourceChannels; i++)
         alSource3f(sources[i], AL_VELOCITY, x, y, z);
   }

   //
   // SoundSource::getSource
   //
   ALuint SoundSource::getSource(unsigned channel)
   {
      if(channel && channel <= MaxSourceChannels)
         return sources[channel - 1];
      else
         return sources[0];
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::AL
{
   // AUDIO_TODO: accum[gain] DGE_SoundChanGain(src, chan, minmax[, gain])
   //             for attenuation
   // AUDIO_TODO: accum[volume] DGE_SoundChanVolume(src, chan[, volume])
   //             NB: gain == volume in openal

   //
   // SrcBind
   //
   static auto SrcBind(unsigned srcId, unsigned sndIdx, unsigned channel,
      bool play)
   {
      auto *audio = AudioRenderer::GetCurrent();
      auto *src   = audio->soundSrcGet(srcId);
      auto *snd   = audio->soundGet(sndIdx);

      if(!src || !snd)
         return 0u;

      if(!channel && !(channel = src->getFreeChannel()))
         return 0u;

               src->bind(channel, snd);
      if(play) src->play(channel);

      return channel;
   }

   //
   // unsigned DGE_SoundSource(int x, int y, int z[, unsigned flags])
   //
   DGE_Code_NativeDefn(DGE_SoundSource)
   {
      auto *audio = AudioRenderer::GetCurrent();
      auto *src =
      #define Cast static_cast<int> // うざい
         audio->soundSrcCreate(Cast(argv[0]), Cast(argv[1]), Cast(argv[2]));
      #undef Cast

      if(src) task->dataStk.push(src->id);
      else    task->dataStk.push(0);

      return false;
   }

   //
   // unsigned DGE_SoundSrcBind(src, snd[, chan])
   //
   DGE_Code_NativeDefn(DGE_SoundSrcBind)
   {
      task->dataStk.push(SrcBind(argv[0], argv[1], argc > 2 ? argv[2] : 0, false));
      return false;
   }

   //
   // unsigned DGE_SoundSrcPlay(src, snd[, chan])
   //
   DGE_Code_NativeDefn(DGE_SoundSrcPlay)
   {
      task->dataStk.push(SrcBind(argv[0], argv[1], argc > 2 ? argv[2] : 0, true));
      return false;
   }

   //
   // void DGE_SoundSrcDestroy(src)
   //
   DGE_Code_NativeDefn(DGE_SoundSrcDestroy)
   {
      AudioRenderer::GetCurrent()->soundSrcDestroy(argv[0]);
      return false;
   }

   #define IfSrc() \
      if(auto *src = AudioRenderer::GetCurrent()->soundSrcGet(argv[0]))

   //
   // void DGE_SoundSrcPosition(src, short _Accum x, y, z)
   //
   DGE_Code_NativeDefn(DGE_SoundSrcPosition)
   {
      IfSrc() src->setPos(argv[1] / 127.0, argv[2] / 127.0, argv[3] / 127.0);
      return false;
   }

   //
   // void DGE_SoundSrcVelocity(src, short _Accum velx, vely, velz)
   //
   DGE_Code_NativeDefn(DGE_SoundSrcVelocity)
   {
      IfSrc() src->setVel(argv[1] / 127.0, argv[2] / 127.0, argv[3] / 127.0);
      return false;
   }

   //
   // void DGE_SoundChanPlay(src, chan)
   //
   DGE_Code_NativeDefn(DGE_SoundChanPlay)
   {
      IfSrc() src->play(argv[1]);
      return false;
   }

   //
   // void DGE_SoundChanStop(src, chan)
   //
   DGE_Code_NativeDefn(DGE_SoundChanStop)
   {
      IfSrc() src->stop(argv[1]);
      return false;
   }

   //
   // void DGE_SoundChanLoop(src, chan, _Bool on)
   //
   DGE_Code_NativeDefn(DGE_SoundChanLoop)
   {
      IfSrc() src->setLoop(argv[1], argv[2]);
      return false;
   }

   //
   // short _Accum DGE_SoundChanPitch(src, chan[, short _Accum pitch])
   //
   DGE_Code_NativeDefn(DGE_SoundChanPitch)
   {
      IfSrc()
      {
         task->dataStk.push(src->getPitch(argv[1]) * 127.0);

         if(argc > 2)
            src->setPitch(argv[1], argv[2] / 127.0);
      }
      else
         task->dataStk.push(0);

      return false;
   }

   #undef IfSrc
}

// EOF

