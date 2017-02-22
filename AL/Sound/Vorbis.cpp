//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Ogg Vorbis sound loading using libvorbisfile.
//
//-----------------------------------------------------------------------------

#if DGE_Use_Vorbis
#include "AL/Sound.hpp"

#include "FS/File.hpp"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // SoundLoader_Vorbis
   //
   class SoundLoader_Vorbis : public SoundLoader
   {
   public:
      SoundLoader_Vorbis(FS::File *file_) :
         pos{},
         file{file_},
         vorbis{}
      {
         ov_open_callbacks(this, &vorbis, nullptr, 0, {Read});

         auto inf = ov_info(&vorbis, -1);

         stereo   = inf->channels > 1;
         samprate = inf->rate;
         samples  = ov_pcm_total(&vorbis, -1);
      }

      virtual ~SoundLoader_Vorbis()
      {
         ov_clear(&vorbis);
      }

      void data(SoundSample *buf);

      bool        isMono()     {return !stereo;}
      std::size_t numSamples() {return samples;}
      std::size_t sampleRate() {return samprate;}

   private:
      static std::size_t Read(void *out, std::size_t size, std::size_t nmemb,
         void *userdata);

      bool        stereo;
      std::size_t samprate;
      std::size_t samples;

      std::size_t    pos;
      FS::File      *file;
      OggVorbis_File vorbis;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // SoundLoader_Vorbis::data
   //
   void SoundLoader_Vorbis::data(SoundSample *buf)
   {
      float **buffer;
      int frame = 0;
      int channels = stereo ? 2 : 1;

      for(;;)
      {
         long blocksize = ov_read_float(&vorbis, &buffer, 1024, &frame);

         if(blocksize > 0)
         {
            for(long i = 0; i < blocksize; i++)
               for(int j = 0; j < channels; j++)
                  *buf++ = static_cast<SoundSample>(buffer[j][i] * 32767);
         }
         else if(blocksize < 0)
            throw SoundLoaderError("invalid vorbis stream");
         else
            break;
      }
   }

   //
   // SoundLoader_Vorbis::Read
   //
   std::size_t SoundLoader_Vorbis::Read(void *out, std::size_t size,
      std::size_t nmemb, void *userdata)
   {
      auto self = static_cast<SoundLoader_Vorbis *>(userdata);
      std::size_t read = size * nmemb;

      if(read)
      {
         if(self->pos + read >= self->file->size)
            read = self->file->size - self->pos;

         if(!read)
            return 0;

         std::memmove(out, self->file->data + self->pos, read);

         self->pos += read;
      }

      return read;
   }

   //
   // CreateSoundLoader_Vorbis
   //
   std::unique_ptr<SoundLoader> CreateSoundLoader_Vorbis(FS::File *file)
   {
      std::unique_ptr<SoundLoader_Vorbis> loader{new SoundLoader_Vorbis{file}};
      return std::unique_ptr<SoundLoader>{loader.release()};
   }
}

#endif//DGE_Use_Vorbis

