//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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
      case Format::Pak:
         dir = CreateDir_Pak(this);
         if(!dir) format = Format::Unknown;
         break;

      case Format::Wad:
         //dir = CreateDir_Wad(this));
         if(!dir) format = Format::Unknown;
         break;

      default: break;
      }

      return dir.get();
   }

   //
   // DetectFormat
   //
   Format DetectFormat(char const *data, std::size_t size)
   {
      if(size < 2) return Format::Unknown;

      if(data[0] == '\037' && data[1] == '\213')
         return Format::gzip;

      if(size < 3) return Format::Unknown;

      // Portable * Map
      if(data[0] == 'P' && std::isspace(data[2])) switch(data[1])
      {
      case '1': case '4': return Format::PBM;
      case '2': case '5': return Format::PGM;
      case '3': case '6': return Format::PPM;
      case '7':           return Format::PAM;
      }

      if(size < 8) return Format::Unknown;

      // Doominati Game Engine Null-Terminated Strings
      if(data[0] == 'D' && data[1] == 'G' && data[2] == 'E' && data[3] == '_' &&
         data[4] == 'N' && data[5] == 'T' && data[6] == 'S' && data[7] == '\0')
         return Format::DGE_NTS;

      // Portable Network Graphics
      if(data[0] == '\x89' && data[1] == 'P'  && data[2] == 'N'    && data[3] == 'G' &&
         data[4] == '\r'   && data[5] == '\n' && data[6] == '\x1a' && data[7] == '\n')
         return Format::PNG;

      if(size < 12) return Format::Unknown;

      // PAK archive
      if(data[0] == 'P' && data[1] == 'A' && data[2] == 'C' && data[3] == 'K')
         return Format::Pak;

      // WAD Archive
      if((data[0] == 'I' || data[0] == 'P') && data[1] == 'W' && data[2] == 'A' && data[3] == 'D')
         return Format::Wad;

      // Waveform Audio File Format
      if(data[0] == 'R' && data[1] == 'I' && data[ 2] == 'F' && data[ 3] == 'F' &&
         data[8] == 'W' && data[9] == 'A' && data[10] == 'V' && data[11] == 'E')
         return Format::WAVE;

      return Format::Unknown;
   }
}

// EOF

