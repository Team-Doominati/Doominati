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

#include "AL/Sound.hpp"

#include "FS/File.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // SoundData constructor
   //
   SoundData::SoundData(std::size_t sampleRate, bool mono,
      GDCC::Core::Array<SoundSample> const &buffer)
   {
      // AUDIO_TODO: if we want stereo sounds to downmix in 3d space, we need
      //             to generate two buffers, one for the 2d sound and one
      //             for the 3d sound
      alGenBuffers(1, &buf);
      alBufferData(buf,
         mono ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
         buffer.data(),
         buffer.size() * sizeof(SoundSample),
         sampleRate);
   }

   //
   // SoundData destructor
   //
   SoundData::~SoundData()
   {
      // NB: 0 is a valid buffer, but we shouldn't try to delete it
      if(buf != 0 && alIsBuffer(buf))
         alDeleteBuffers(1, &buf);
   }

   std::unique_ptr<SoundLoader> CreateSoundLoader_WAVE(FS::File *file);

   //
   // CreateSoundLoader
   //
   std::unique_ptr<SoundLoader> CreateSoundLoader(FS::File *file)
   {
      switch(file->format)
      {
      case FS::Format::WAVE: return CreateSoundLoader_WAVE(file);

      default:
         throw SoundLoaderError("unknown format");
      }
   }
}

// EOF

