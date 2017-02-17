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

#ifndef DGE__AL__SoundSource_H__
#define DGE__AL__SoundSource_H__

#include "AL/OpenAL.hpp"

#include "Core/List.hpp"

#include <array>
#include <utility>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::AL
{
   static constexpr unsigned MaxSourceChannels = 8;
}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   class SoundData;

   //
   // SoundSource
   //
   class SoundSource
   {
   public:
      SoundSource(unsigned id_);
      SoundSource(SoundSource const &) = delete;
      SoundSource(SoundSource &&src) :
         id{src.id}, link{this, std::move(src.link)},
         sources{std::move(src.sources)}
         {src.id = 0;}
      ~SoundSource();

      unsigned getFreeChannel();
      void  bind    (unsigned channel, SoundData *snd);
      float getPitch(unsigned channel);
      void  play    (unsigned channel);
      void  stop    (unsigned channel);
      void  setLoop (unsigned channel, bool loop);
      void  setPitch(unsigned channel, float pitch);
      void  setPos  (float x, float y, float z);
      void  setVel  (float x, float y, float z);

      unsigned id;
      Core::ListLink<SoundSource> link;

   private:
      ALuint getSource(unsigned channel);

      std::array<ALuint, MaxSourceChannels> sources;
   };
}

#endif//DGE__AL__SoundSource_H__

