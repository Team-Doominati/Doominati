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
   // SoundSource destructor
   //
   SoundSource::~SoundSource()
   {
      for(std::size_t i = 0; i < channelC; i++)
         if(channelV[i].alloc)
            alDeleteSources(1, &channelV[i].src);
   }

   //
   // SoundSource::getFreeChannel
   //
   unsigned SoundSource::getFreeChannel()
   {
      for(std::size_t i = 0; i < channelC; i++)
      {
         auto &channel = getChannelIdx(i);

         if(channel.bound) continue;

         ALenum state;

         alGetSourcei(channel.src, AL_SOURCE_STATE, &state);

         if(state != AL_PLAYING)
            return i + 1;
      }

      return requestChannels() && getFreeChannel();
   }

   //
   // SoundSource::bind
   //
   void SoundSource::bind(unsigned channel, SoundData *snd)
   {
      auto &chan = getChannel(channel);
      chan.bound = true;
      alSourcei(chan.src, AL_BUFFER, snd->buf);
   }

   //
   // SoundSource::play
   //
   void SoundSource::play(unsigned channel)
   {
      auto &chan = getChannel(channel);
      chan.bound = false;
      alSourcePlay(chan.src);
   }

   //
   // SoundSource::stop
   //
   void SoundSource::stop(unsigned channel)
   {
      alSourceStop(getChannel(channel).src);
   }

   //
   // SoundSource::getPitch
   //
   float SoundSource::getPitch(unsigned channel)
   {
      ALfloat ret;
      alGetSourcef(getChannel(channel).src, AL_PITCH, &ret);
      return ret;
   }

   //
   // SoundSource::setLoop
   //
   void SoundSource::setLoop(unsigned channel, bool loop)
   {
      alSourcei(getChannel(channel).src, AL_LOOPING, loop);
   }

   //
   // SoundSource::setPitch
   //
   void SoundSource::setPitch(unsigned channel, float pitch)
   {
      alSourcef(getChannel(channel).src, AL_PITCH, pitch);
   }

   //
   // SoundSource::setVolume
   //
   void SoundSource::setVolume(unsigned channel, float volume)
   {
      alSourcef(getChannel(channel).src, AL_GAIN, volume);
   }

   //
   // SoundSource::setPos
   //
   void SoundSource::setPos(unsigned channel, float x, float y, float z)
   {
      alSource3f(getChannel(channel).src, AL_POSITION, x, y, z);
   }

   //
   // SoundSource::setPos
   //
   void SoundSource::setPos(float x, float y, float z)
   {
      for(std::size_t i = 0; i < channelC; i++)
         alSource3f(getChannelIdx(i).src, AL_POSITION, x, y, z);
   }

   //
   // SoundSource::setVel
   //
   void SoundSource::setVel(float x, float y, float z)
   {
      for(std::size_t i = 0; i < channelC; i++)
         alSource3f(getChannelIdx(i).src, AL_VELOCITY, x, y, z);
   }

   //
   // SoundSource::getChannel
   //
   SoundChannel &SoundSource::getChannel(unsigned channel)
   {
      if(!channelV)
         requestChannels();

      return getChannelIdx(channel && channel <= channelC ? channel - 1 : 0);
   }

   //
   // SoundSource::getChannelIdx
   //
   SoundChannel &SoundSource::getChannelIdx(std::size_t idx)
   {
      auto &channel = channelV[idx];

      if(!channel.alloc)
      {
         alGenSources(1, &channel.src);
         channel.alloc = true;
      }

      return channel;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::AL
{
   // AUDIO_TODO: accum[gain] DGE_SoundChanGain(src, chan, minmax[, gain])
   //             for attenuation

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
      #define Cast static_cast<int> /* うざい */
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
      IfSrc() src->setPos(argv[1] / 128.0, argv[2] / 128.0, argv[3] / 128.0);
      return false;
   }

   //
   // void DGE_SoundSrcVelocity(src, short _Accum velx, vely, velz)
   //
   DGE_Code_NativeDefn(DGE_SoundSrcVelocity)
   {
      IfSrc() src->setVel(argv[1] / 128.0, argv[2] / 128.0, argv[3] / 128.0);
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
   // void DGE_SoundChanPosition(src, chan, short _Accum x, y, z)
   //
   DGE_Code_NativeDefn(DGE_SoundChanPosition)
   {
      IfSrc() src->setPos(argv[1], argv[2] / 128.0, argv[3] / 128.0, argv[4] / 128.0);
      return false;
   }

   //
   // short _Accum DGE_SoundChanPitch(src, chan[, short _Accum pitch])
   //
   DGE_Code_NativeDefn(DGE_SoundChanPitch)
   {
      IfSrc()
      {
         task->dataStk.push(src->getPitch(argv[1]) * 128.0);

         if(argc > 2)
            src->setPitch(argv[1], argv[2] / 128.0);
      }
      else
         task->dataStk.push(0);

      return false;
   }

   //
   // void DGE_SoundChanVolume(src, chan, unsigned long _Fract volume)
   //
   DGE_Code_NativeDefn(DGE_SoundChanVolume)
   {
      IfSrc() src->setVolume(argv[1], argv[2] / 4294967295.0);
      return false;
   }

   #undef IfSrc
}

// EOF

