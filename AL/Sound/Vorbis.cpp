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

#if DGE_AL_Use_Vorbis
#include "AL/Sound.hpp"

#include "Core/Math.hpp"

#include "FS/File.hpp"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <memory>
#include <cstring>
#include <algorithm>


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
         ov_open_callbacks(this, &vorbis, nullptr, 0, {Read, Seek, nullptr, Tell});

         auto inf = ov_info(&vorbis, -1);

         if(!inf)
            throw SoundLoaderError("couldn't get info from bitstream");

         stereo   = inf->channels > 1;
         samprate = inf->rate;

         auto total = ov_pcm_total(&vorbis, -1);

         if(total == OV_EINVAL)
            throw SoundLoaderError("invalid stream length");
         else
            samples = total;
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

      static int Seek(void *userdata, ogg_int64_t offset, int whence);

      static long Tell(void *userdata);

      bool        stereo;
      std::size_t samprate;
      std::size_t samples;

      ogg_int64_t    pos;
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
      float **buffer   = nullptr;
      int     channels = stereo ? 2 : 1;

      for(std::size_t iter = 0; iter < samples * channels;)
      {
         long blocksize = ov_read_float(&vorbis, &buffer, 1024, nullptr);

         if(blocksize > 0)
         {
            for(long i = 0; i < blocksize; i++)
               for(int j = 0; j < channels; j++)
            {
               float clamped = Core::Clamp(buffer[j][i], -1.0f, 1.0f);
               buf[iter++] = static_cast<SoundSample>(clamped * 32767);
            }
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
   // SoundLoader_Vorbis::Seek
   //
   int SoundLoader_Vorbis::Seek(void *userdata, ogg_int64_t offset, int whence)
   {
      auto self = static_cast<SoundLoader_Vorbis *>(userdata);

      switch(whence)
      {
      default: return -1;
      case SEEK_END: self->pos  = self->file->size + offset; return 0;
      case SEEK_SET: self->pos  = offset; return 0;
      case SEEK_CUR: self->pos += offset; return 0;
      }
   }

   //
   // SoundLoader_Vorbis::Tell
   //
   long SoundLoader_Vorbis::Tell(void *userdata)
   {
      auto self = static_cast<SoundLoader_Vorbis *>(userdata);

      if(self->pos > static_cast<ogg_int64_t>(self->file->size))
         return EOF;
      else
         return self->pos;
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

#endif//DGE_AL_Use_Vorbis

