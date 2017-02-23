//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Ogg Opus sound loading using libopusfile.
//
//-----------------------------------------------------------------------------

#if DGE_AL_Use_Opus
#include "AL/Sound.hpp"

#include "Core/Math.hpp"

#include "FS/File.hpp"

#include <opusfile.h>

#include <memory>
#include <iostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // SoundLoader_Opus
   //
   class SoundLoader_Opus : public SoundLoader
   {
   public:
      SoundLoader_Opus(FS::File *file) :
         opus{nullptr}
      {
         {
         auto *udata = reinterpret_cast<unsigned char const *>(file->data);
         opus = op_open_memory(udata, file->size, nullptr);
         }

         if(!opus)
            throw SoundLoaderError("invalid opus stream");

         auto inf = op_head(opus, -1);

         if(!inf)
            throw SoundLoaderError("couldn't get info from bitstream");

         stereo = inf->channel_count > 1;

         auto total = op_pcm_total(opus, -1);

         if(total == OP_EINVAL)
            throw SoundLoaderError("invalid stream length");
         else
            samples = total;
      }

      virtual ~SoundLoader_Opus()
      {
         op_free(opus);
      }

      void data(SoundSample *buf);

      bool        isMono()     {return !stereo;}
      std::size_t numSamples() {return samples;}
      std::size_t sampleRate() {return 48000;}

   private:
      bool        stereo;
      std::size_t samples;

      OggOpusFile *opus;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // SoundLoader_Opus::data
   //
   void SoundLoader_Opus::data(SoundSample *buf)
   {
      std::unique_ptr<float[]> buffer{new float[1024 * 12]};
      int    channels = stereo ? 2 : 1;

      for(std::size_t iter = 0; iter < samples * channels;)
      {
         int blocksize = op_read_float_stereo(opus, buffer.get(), 1024 * 12);

         if(blocksize > 0)
         {
            for(int i = 0; i < blocksize; i++)
               for(int j = 0; j < channels; j++)
            {
               // This is supposed to be clamped, but of course, it's not.
               auto clamped = Core::Clamp(buffer[(i * 2) + j], -1.0f, 1.0f);
               buf[iter++] = static_cast<SoundSample>(clamped * 32767);
            }
         }
         else if(blocksize < 0)
            throw SoundLoaderError("invalid opus stream");
         else
            break;
      }
   }

   //
   // CreateSoundLoader_Opus
   //
   std::unique_ptr<SoundLoader> CreateSoundLoader_Opus(FS::File *file)
   {
      std::unique_ptr<SoundLoader_Opus> loader{new SoundLoader_Opus{file}};
      return std::unique_ptr<SoundLoader>{loader.release()};
   }
}

#endif//DGE_AL_Use_Opus

