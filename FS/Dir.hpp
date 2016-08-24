//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Filesystem directory abstraction.
//
//-----------------------------------------------------------------------------

#ifndef Doom__FS__Dir_H__
#define Doom__FS__Dir_H__

#include <cstddef>
#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   namespace Core
   {
      class FileBlock;
   }
}

namespace Doom
{
   namespace FS
   {
      class File;

      enum class Format
      {
         None,

         DGE_NTS, // Doominati Game Engine Null-Terminated Strings
         PAM,     // Portable Arbitrary Map
         Pak,     // PAK archive
         PBM,     // Portable Bit Map
         PGM,     // Portable Gray Map
         PPM,     // Portable Pixel Map
         Wad,     // WAD archive
         WAVE,    // Waveform Audio File Format

         Unknown
      };

      //
      // Dir
      //
      class Dir
      {
      public:
         Dir() : name{""} {}
         Dir(Dir const &) = delete;
         Dir(Dir &&) = default;
         virtual ~Dir() {}

         Dir &operator = (Dir const &) = delete;
         Dir &operator = (Dir &&) = default;

         virtual Dir *findDir(char const *name);

         virtual File *findFile(char const *name) = 0;

         // Frees unused file allocations.
         virtual void prune();

         char const *name;


         static bool AddRoot(char const *filename);

         static void ClearRoot();

         static File *FindFile(char       *path);
         static File *FindFile(char const *path);
      };

      //
      // File
      //
      class File
      {
      public:
         File() :
            data{nullptr},
            name{""},
            refs{0},
            size{0},
            format{Format::None}
         {
         }

         File(char const *data_, std::size_t size_) :
            data{data_},
            name{""},
            refs{0},
            size{size_},
            format{Format::None}
         {
         }

         Dir *findDir();

         std::unique_ptr<Dir> dir;
         char const          *data;
         char const          *name;
         std::size_t          refs;
         std::size_t          size;
         Format               format;
      };
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace FS
   {
      std::unique_ptr<Dir> CreateDir_Directory(char const *name);
      std::unique_ptr<Dir> CreateDir_Pak(File *file);
      std::unique_ptr<Dir> CreateDir_Pak(std::unique_ptr<GDCC::Core::FileBlock> &&file);
      std::unique_ptr<Dir> CreateDir_Wad(File *file);
      std::unique_ptr<Dir> CreateDir_Wad(std::unique_ptr<GDCC::Core::FileBlock> &&file);

      Format DetectFormat(char const *data, std::size_t size);
   }
}

#endif//Doom__FS__Dir_H__

