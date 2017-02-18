//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dynamically-allocated sound sources.
//
//-----------------------------------------------------------------------------

#ifndef DGE__AL__SoundSourceDynamic_H__
#define DGE__AL__SoundSourceDynamic_H__

#include "AL/SoundSource.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // SoundSourceDynamic
   //
   class SoundSourceDynamic : public SoundSource
   {
   public:
      SoundSourceDynamic(unsigned id_) : SoundSource(id_) {}

      // requestChannels
      virtual bool requestChannels()
      {
         cdata.resize(cdata.size() + 32);

         channelV = cdata.data();
         channelC = cdata.size();

         return true;
      }

   private:
      std::vector<SoundChannel> cdata;
   };
}

#endif//DGE__AL__SoundSourceDynamic_H__

