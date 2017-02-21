//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// RIFF WAVE loading.
//
//-----------------------------------------------------------------------------

#include "AL/Sound.hpp"

#include "Core/Types.hpp"

#include "FS/File.hpp"

#include <cstring>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // IFFChunk
   //
   struct IFFChunk
   {
      char const *data;
      std::uint32_t size;
   };

   //
   // SoundLoader_WAVE
   //
   class SoundLoader_WAVE : public SoundLoader
   {
   public:
      SoundLoader_WAVE(FS::File *file_) :
         dataChunk{},

         samprate{},
         blksize{},
         bitsamp{},
         samples{},

         file{file_}
      {
      }

      void data(SoundSample *buf);

      bool        isMono()     {return true;}
      std::size_t numSamples() {return samples;}
      std::size_t sampleRate() {return samprate;}

      void loadHeader();

   private:
      IFFChunk findChunk(char const *name);

      std::uint16_t readU16_LE(char const *data);
      std::uint32_t readU32_LE(char const *data);

      IFFChunk dataChunk;

      std::size_t samprate;
      std::size_t blksize;
      std::size_t bitsamp;
      std::size_t samples;

      FS::File *file;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // SoundLoader_WAVE::data
   //
   void SoundLoader_WAVE::data(SoundSample *buf)
   {
      for(std::size_t i = 0; i < samples; i++)
      {
         auto sample = readU32_LE(dataChunk.data + i * blksize);

         switch(bitsamp)
         {
         case 16:
            if(sample == 0x8000)
               buf[i] = -0x7FFF;
            else if(sample & 0x8000)
               buf[i] = -static_cast<SoundSample>((~sample & 0x7FFF) + 1);
            else
               buf[i] =  static_cast<SoundSample>(  sample & 0x7FFF);
            break;
         case 8:
            buf[i++] = ((sample & 0x000000FF) >> 0 ) << 8;
            buf[i++] = ((sample & 0x0000FF00) >> 8 ) << 8;
            buf[i++] = ((sample & 0x00FF0000) >> 16) << 8;
            buf[i  ] = ((sample & 0xFF000000) >> 24) << 8;
            break;
         }
      }
   }

   //
   // SoundLoader_WAVE::findChunk
   //
   IFFChunk SoundLoader_WAVE::findChunk(char const *name)
   {
      auto end = file->data + file->size;

      // We just look over the whole file here, skipping the RIFF form.
      for(auto itr = file->data + 12; itr < end;)
      {
         auto size = readU32_LE(itr + 4);

         if(std::memcmp(name, itr, 4) == 0)
            return {itr + 8, size};
         else
            itr += size + 8;
      }

      return {};
   }

   //
   // SoundLoader_WAVE::loadHeader
   //
   void SoundLoader_WAVE::loadHeader()
   {
      // Parse format chunk.
      auto chunk = findChunk("fmt ");

      if(!chunk.data)                     throw SoundLoaderError("no format chunk");
      if(readU16_LE(chunk.data + 0) != 1) throw SoundLoaderError("unsupported format type");

      samprate = readU32_LE(chunk.data +  4);
      blksize  = readU16_LE(chunk.data + 12);
      bitsamp  = readU16_LE(chunk.data + 14);

      if(bitsamp != 8 && bitsamp != 16) throw SoundLoaderError("invalid bits per sample");

      // Parse data chunk.
      dataChunk = findChunk("data");
      if(!dataChunk.data) throw SoundLoaderError("no data chunk");

      samples = dataChunk.size / blksize;
   }

   //
   // SoundLoader_WAVE::readU16_LE
   //
   std::uint16_t SoundLoader_WAVE::readU16_LE(char const *data)
   {
      auto udata = reinterpret_cast<Core::Byte const *>(data);
      return (udata[1] << 8) | udata[0];
   }

   //
   // SoundLoader_WAVE::readU32_LE
   //
   std::uint32_t SoundLoader_WAVE::readU32_LE(char const *data)
   {
      auto udata = reinterpret_cast<Core::Byte const *>(data);
      return (udata[3] << 24) | (udata[2] << 16) | (udata[1] << 8) | udata[0];
   }

   //
   // CreateSoundLoader_WAVE
   //
   std::unique_ptr<SoundLoader> CreateSoundLoader_WAVE(FS::File *file)
   {
      std::unique_ptr<SoundLoader_WAVE> loader{new SoundLoader_WAVE{file}};
      loader->loadHeader();
      return std::unique_ptr<SoundLoader>{loader.release()};
   }
}

// EOF

