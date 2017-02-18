//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Sound management.
//
//-----------------------------------------------------------------------------

#ifndef DGE__AL__Sound_H__
#define DGE__AL__Sound_H__

#include "AL/OpenAL.hpp"

#include <GDCC/Core/Array.hpp>

#include <exception>
#include <stdexcept>
#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class File;
}

namespace DGE::AL
{
   using SoundSample = ALshort;

   //
   // SoundData
   //
   class SoundData
   {
   public:
      SoundData() : buf{0} {}
      SoundData(SoundData const &) = delete;
      SoundData(SoundData &&s) : buf{s.buf} {s.buf = 0;}
      SoundData(std::size_t frequency, bool mono,
         GDCC::Core::Array<SoundSample> const &buffer);
      ~SoundData();

      SoundData &operator = (SoundData const &) = delete;
      SoundData &operator = (SoundData &&s)
         {std::swap(buf, s.buf); return *this;}

      ALuint buf;
   };

   //
   // SoundLoader
   //
   class SoundLoader
   {
   public:
      virtual ~SoundLoader() {}

      virtual void        data(SoundSample *buf) = 0;
      virtual bool        isMono()               = 0;
      virtual std::size_t numSamples()           = 0;
      virtual std::size_t sampleRate()           = 0;
   };

   //
   // SoundLoaderError
   //
   class SoundLoaderError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   std::unique_ptr<SoundLoader> CreateSoundLoader(FS::File *file);
}

#endif//DGE__AL__Sound_H__

