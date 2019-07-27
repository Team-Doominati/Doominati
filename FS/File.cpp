//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Filesystem file abstraction.
//
//-----------------------------------------------------------------------------

#include "FS/File.hpp"

#include "FS/Dir.hpp"

#include <cctype>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // File constructor
   //
   File::File() :
      data{nullptr},
      name{""},
      refs{0},
      size{0},
      format{Format::None}
   {
   }

   //
   // File constructor
   //
   File::File(char const *data_, std::size_t size_) :
      data{data_},
      name{""},
      refs{0},
      size{size_},
      format{Format::None}
   {
   }

   //
   // File constructor
   //
   File::File(Core::HashedStr name_) :
      data{nullptr},
      name{name_},
      refs{0},
      size{0},
      format{Format::None}
   {
   }

   //
   // File destructor
   //
   File::~File()
   {
   }

   //
   // File::findDir
   //
   Dir *File::findDir()
   {
      if(!dir) switch(format)
      {
      case Format::Pak: dir = CreateDir_Pak(this); break;
    //case Format::Wad: dir = CreateDir_Wad(this); break;
      case Format::Zip: dir = CreateDir_Zip(this); break;

      default: return dir.get();
      }

      if(!dir) format = Format::Unknown;

      return dir.get();
   }

   //
   // File::trunc
   //
   bool File::trunc(std::size_t)
   {
      return false;
   }

   //
   // File::write
   //
   std::size_t File::write(std::size_t, char const *, std::size_t)
   {
      return 0;
   }

   //
   // DetectFormat
   //
   Format DetectFormat(char const *data, std::size_t size)
   {
      if(size < 3) return Format::Unknown;

      // Portable * Map
      // Identifier: "P" <type> <whitespace>
      if(data[0] == 'P' && std::isspace(data[2])) switch(data[1])
      {
      case '1': case '4': return Format::PBM;
      case '2': case '5': return Format::PGM;
      case '3': case '6': return Format::PPM;
      case '7':           return Format::PAM;
      }

      if(size < 8) return Format::Unknown;

      // Doominati Game Engine Null-Terminated Strings
      // Identifier: "DGE_NTS" \0
      if(data[0] == 'D' && data[1] == 'G' && data[2] == 'E' && data[3] == '_' &&
         data[4] == 'N' && data[5] == 'T' && data[6] == 'S' && data[7] == '\0')
         return Format::DGE_NTS;

      // Free Lossless Audio Codec
      // Identifier: "fLaC"
      if(data[0] == 'f' && data[1] == 'L' && data[2] == 'a' && data[3] == 'C')
         return Format::FLAC;

      // Joint Photographic Experts Group Interchange Format
      // Identifier: FF D8 FF <APP number starting at E0>
      if(data[0] == '\xFF' && data[1] == '\xD8' && data[2] == '\xFF' && data[3] >= '\xE0')
         return Format::JPEG;

      // Portable Network Graphics
      // Identifier: 89 "PNG" \r \n 1A \n
      if(data[0] == '\x89' && data[1] == 'P'  && data[2] == 'N'    && data[3] == 'G' &&
         data[4] == '\r'   && data[5] == '\n' && data[6] == '\x1a' && data[7] == '\n')
         return Format::PNG;

      if(size < 12) return Format::Unknown;

      // PAK archive
      // Identifier: "PACK"
      if(data[0] == 'P' && data[1] == 'A' && data[2] == 'C' && data[3] == 'K')
         return Format::Pak;

      // WAD archive
      // Identifier: 'I'/'P' "WAD"
      if((data[0] == 'I' || data[0] == 'P') && data[1] == 'W' && data[2] == 'A' && data[3] == 'D')
         return Format::Wad;

      // Waveform Audio File Format
      // Identifier: "RIFF" <size> "WAVE"
      if(data[0] == 'R' && data[1] == 'I' && data[ 2] == 'F' && data[ 3] == 'F' &&
         data[8] == 'W' && data[9] == 'A' && data[10] == 'V' && data[11] == 'E')
         return Format::WAVE;

      if(size < 22) return Format::Unknown;

      // ZIP archive
      // Identifier: <file header signature starting with "PK">
      if(data[0] == 'P' && data[1] == 'K' &&
         ((data[2] == '\x01' && data[3] == '\x02') ||
          (data[2] == '\x03' && data[3] == '\x04') ||
          (data[2] == '\x05' && data[3] == '\x06')))
         return Format::Zip;

      if(size < 47) return Format::Unknown;

      // Ogg
      if(data[0] == 'O' && data[1] == 'g' && data[2] == 'g' && data[3] == 'S')
         return Format::Ogg;

      return Format::Unknown;
   }
}

// EOF

