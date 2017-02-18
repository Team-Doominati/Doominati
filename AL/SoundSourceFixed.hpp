//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Efficient fixed-size sound sources.
//
//-----------------------------------------------------------------------------

#ifndef DGE__AL__SoundSourceFixed_H__
#define DGE__AL__SoundSourceFixed_H__

#include "AL/SoundSource.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // SoundSourceFixed
   //
   template<std::size_t NumChannels>
   class SoundSourceFixed : public SoundSource
   {
   public:
      SoundSourceFixed(unsigned id_) :
         SoundSource(id_), cdata{}
         {channelC = NumChannels; channelV = cdata;}

      // requestChannels
      virtual bool requestChannels() {return false;}

   private:
      SoundChannel cdata[NumChannels];
   };
}

#endif//DGE__AL__SoundSourceFixed_H__

