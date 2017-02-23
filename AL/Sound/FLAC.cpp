//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// FLAC sound loading using libFLAC & libogg.
//
//-----------------------------------------------------------------------------

#if DGE_AL_Use_FLAC
#include "AL/Sound.hpp"

#include "FS/File.hpp"

#include <FLAC/stream_decoder.h>

#include <memory>
#include <cstring>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // SoundLoader_FLAC
   //
   class SoundLoader_FLAC : public SoundLoader
   {
   public:
      SoundLoader_FLAC(FS::File *file_) :
         stereo{},
         samprate{},
         samples{},
         bitsamp{},

         file{file_},
         pos{},
         buf{},

         decoder{FLAC__stream_decoder_new()}
      {
         FLAC__stream_decoder_set_md5_checking(decoder, true);
         FLAC__stream_decoder_init_stream(decoder, Read, Seek, Tell, Length,
            Eof, Write, Metadata, Error, this);
         FLAC__stream_decoder_process_until_end_of_metadata(decoder);
      }

      virtual ~SoundLoader_FLAC()
      {
         FLAC__stream_decoder_delete(decoder);
      }

      void data(SoundSample *buf_);

      bool        isMono()     {return !stereo;}
      std::size_t numSamples() {return samples;}
      std::size_t sampleRate() {return samprate;}

   private:
      static FLAC__StreamDecoderReadStatus Read(FLAC__StreamDecoder const *,
         FLAC__byte buffer[], std::size_t *bytes, void *userdata);

      static FLAC__StreamDecoderSeekStatus Seek(FLAC__StreamDecoder const *,
         FLAC__uint64 offs, void *userdata);

      static FLAC__StreamDecoderTellStatus Tell(FLAC__StreamDecoder const *,
         FLAC__uint64 *offs, void *userdata);

      static FLAC__StreamDecoderLengthStatus Length(FLAC__StreamDecoder const *,
         FLAC__uint64 *out, void *userdata);

      static FLAC__bool Eof(FLAC__StreamDecoder const *, void *userdata);

      static FLAC__StreamDecoderWriteStatus Write(FLAC__StreamDecoder const *,
         FLAC__Frame const *frame, FLAC__int32 const *const buffer[],
         void *userdata);

      static void Metadata(FLAC__StreamDecoder const *,
         FLAC__StreamMetadata const *meta, void *userdata);

      [[noreturn]] static void Error(FLAC__StreamDecoder const *,
         FLAC__StreamDecoderErrorStatus status, void *);

      bool stereo;
      std::size_t samprate;
      std::size_t samples;
      std::size_t bitsamp;

      FS::File    *file;
      FLAC__uint64 pos;
      SoundSample *buf; // unfortunate

      FLAC__StreamDecoder *decoder;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // SoundLoader_FLAC::data
   //
   void SoundLoader_FLAC::data(SoundSample *buf_)
   {
      buf = buf_;
      FLAC__stream_decoder_process_until_end_of_stream(decoder);
      FLAC__stream_decoder_finish(decoder);
   }

   //
   // SoundLoader_FLAC::Read
   //
   FLAC__StreamDecoderReadStatus SoundLoader_FLAC::Read(
      FLAC__StreamDecoder const *, FLAC__byte buffer[], std::size_t *bytes,
      void *userdata)
   {
      auto *self = static_cast<SoundLoader_FLAC *>(userdata);

      if(*bytes)
      {
         if(self->pos + *bytes >= self->file->size)
            *bytes = self->file->size - self->pos;

         if(!*bytes)
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

         std::memmove(buffer, self->file->data + self->pos, *bytes);

         self->pos += *bytes;
      }

      return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
   }

   //
   // SoundLoader_FLAC::Seek
   //
   FLAC__StreamDecoderSeekStatus SoundLoader_FLAC::Seek(
      FLAC__StreamDecoder const *, FLAC__uint64 offs, void *userdata)
   {
      auto *self = static_cast<SoundLoader_FLAC *>(userdata);

      if(offs >= self->file->size)
         return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;

      self->pos = offs;

      return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
   }

   //
   // SoundLoader_FLAC::Tell
   //
   FLAC__StreamDecoderTellStatus SoundLoader_FLAC::Tell(
      FLAC__StreamDecoder const *, FLAC__uint64 *offs, void *userdata)
   {
      *offs = (FLAC__uint64)((static_cast<SoundLoader_FLAC *>(userdata))->pos);
      return FLAC__STREAM_DECODER_TELL_STATUS_OK;
   }

   //
   // SoundLoader_FLAC::Length
   //
   FLAC__StreamDecoderLengthStatus SoundLoader_FLAC::Length(
      FLAC__StreamDecoder const *, FLAC__uint64 *out, void *userdata)
   {
      *out = (FLAC__uint64)((static_cast<SoundLoader_FLAC *>(userdata))->file->size);
      return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
   }

   //
   // SoundLoader_FLAC::Eof
   //
   FLAC__bool SoundLoader_FLAC::Eof(FLAC__StreamDecoder const *,
      void *userdata)
   {
      auto self = static_cast<SoundLoader_FLAC *>(userdata);
      return self->pos >= self->file->size;
   }

   //
   // SoundLoader_FLAC::Write
   //
   FLAC__StreamDecoderWriteStatus SoundLoader_FLAC::Write(
      FLAC__StreamDecoder const *, FLAC__Frame const *frame,
      FLAC__int32 const *const buffer[], void *userdata)
   {
      auto *self = static_cast<SoundLoader_FLAC *>(userdata);
      int channels = self->stereo ? 2 : 1;

      for(std::size_t i = 0; i < frame->header.blocksize; i++)
         for(int j = 0; j < channels; j++)
      {
         if(self->bitsamp < 16)
            *self->buf++ = buffer[j][i] << (16 - self->bitsamp);
         else
            *self->buf++ = buffer[j][i] >> (self->bitsamp - 16);
      }

      return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
   }

   //
   // SoundLoader_FLAC::Metadata
   //
   void SoundLoader_FLAC::Metadata(FLAC__StreamDecoder const *,
      FLAC__StreamMetadata const *meta, void *userdata)
   {
      auto *self = static_cast<SoundLoader_FLAC *>(userdata);

      if(meta->type == FLAC__METADATA_TYPE_STREAMINFO)
      {
         self->stereo   = meta->data.stream_info.channels > 1;
         self->samprate = meta->data.stream_info.sample_rate;
         self->samples  = meta->data.stream_info.total_samples;
         self->bitsamp  = meta->data.stream_info.bits_per_sample;
      }
   }

   //
   // SoundLoader_FLAC::Error
   //
   [[noreturn]]
   void SoundLoader_FLAC::Error(FLAC__StreamDecoder const *,
      FLAC__StreamDecoderErrorStatus status, void *)
   {
      throw SoundLoaderError(FLAC__StreamDecoderErrorStatusString[status]);
   }

   //
   // CreateSoundLoader_FLAC
   //
   std::unique_ptr<SoundLoader> CreateSoundLoader_FLAC(FS::File *file)
   {
      std::unique_ptr<SoundLoader_FLAC> loader{new SoundLoader_FLAC{file}};
      return std::unique_ptr<SoundLoader>{loader.release()};
   }
}

#endif//DGE_AL_Use_FLAC

