//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Sound sources.
//
//-----------------------------------------------------------------------------

#ifndef DGE__AL__SoundSource_H__
#define DGE__AL__SoundSource_H__

#include "../AL/Types.hpp"

#include "Core/List.hpp"

#include <utility>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // SoundChannel
   //
   struct SoundChannel
   {
      ALuint src;
      bool   bound;
      bool   alloc;
   };

   //
   // SoundSource
   //
   class SoundSource
   {
   public:
      virtual ~SoundSource() {}

      unsigned getFreeChannel();
      void  bind     (unsigned channel, SoundData *snd);
      void  play     (unsigned channel);
      void  stop     (unsigned channel);
      float getPitch (unsigned channel);
      void  setLoop  (unsigned channel, bool loop);
      void  setPitch (unsigned channel, float pitch);
      void  setVolume(unsigned channel, float volume);
      void  setPos   (unsigned channel, float x, float y, float z);
      void  setPos   (float x, float y, float z);
      void  setVel   (float x, float y, float z);

      unsigned id;
      Core::ListLink<SoundSource> link;

   protected:
      SoundSource(unsigned id_) :
         id{id_}, link{this}, channelV{nullptr}, channelC{0} {}

      void freeChannels();
      virtual bool requestChannels() = 0;

      SoundChannel *channelV;
      std::size_t   channelC;

   private:
      SoundChannel &getChannel(unsigned channel);
      SoundChannel &getChannelIdx(std::size_t idx);
   };
}

#endif//DGE__AL__SoundSource_H__

