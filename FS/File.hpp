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

#ifndef DGE__FS__File_H__
#define DGE__FS__File_H__

#include "Types.hpp"

#include "../Core/String.hpp"

#include <cstddef>
#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   enum class Format
   {
      None,

      DGE_NTS, // Doominati Game Engine Null-Terminated Strings
      FLAC,    // Free Lossless Audio Codec
      JPEG,    // Joint Photographic Experts Group Interchange Format
      Ogg,     // Ogg Container
      Pak,     // PAK archive
      PAM,     // Portable Arbitrary Map
      PBM,     // Portable Bit Map
      PGM,     // Portable Gray Map
      PNG,     // Portable Network Graphics
      PPM,     // Portable Pixel Map
      Wad,     // WAD archive
      WAVE,    // Waveform Audio File Format
      Zip,     // ZIP archive

      Unknown
   };

   //
   // File
   //
   class File
   {
   public:
      File();
      File(File const &) = delete;
      File(File &&) = default;
      File(char const *data, std::size_t size);
      File(Core::HashedStr name);
      virtual ~File();

      File &operator = (File const &) = delete;
      File &operator = (File &&) = default;

      Dir *findDir();

      virtual bool trunc(std::size_t len);

      virtual std::size_t write(std::size_t pos, char const *buf, std::size_t len);

      std::unique_ptr<Dir> dir;
      char const          *data;
      Core::HashedStr      name;
      std::size_t          refs;
      std::size_t          size;
      Format               format;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   Format DetectFormat(char const *data, std::size_t size);
}

#endif//DGE__FS__File_H__

