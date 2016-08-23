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

#include "FS/Dir.hpp"

#include <GDCC/Core/Dir.hpp>
#include <GDCC/Core/File.hpp>
#include <GDCC/Core/String.hpp>

#include <vector>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

namespace Doom
{
   namespace FS
   {
      static std::vector<std::unique_ptr<Dir>> Roots;
   }
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace Doom
{
   namespace FS
   {
      //
      // WalkPath
      //
      // Goes through a path and searches for sub-directories until the last
      // path part. Returns a pointer to the last path part, or nullptr if path
      // not valid.
      //
      static std::pair<Dir *, char *> WalkPath(Dir *dir, char *path)
      {
         char *pathBase = path;

         for(; *path; ++path)
         {
            if(*path == '/')
            {
               *path = 0;
               dir = dir->findDir(pathBase);
               *path = '/';

               if(!dir) return {nullptr, nullptr};

               pathBase = path + 1;
            }
         }

         return {dir, pathBase};
      }

      //
      // FindFile
      //
      static File *FindFile(Dir *dir, char *path)
      {
         std::tie(dir, path) = WalkPath(dir, path);

         return path ? dir->findFile(path) : nullptr;
      }
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace FS
   {
      //
      // Dir::findDir
      //
      Dir *Dir::findDir(char const *dirname)
      {
         File *file = findFile(dirname);
         return file ? file->findDir() : nullptr;
      }

      //
      // Dir::prune
      //
      void Dir::prune()
      {
      }

      //
      // Dir::AddRoot
      //
      bool Dir::AddRoot(char const *name)
      {
         std::unique_ptr<Dir> dir;

         if(!GDCC::Core::IsDir(name))
         {
            auto file = GDCC::Core::FileOpenBlock(name);

            if(file) switch(DetectFormat(file->data(), file->size()))
            {
            case Format::Pak:
             //dir = DirCreate_Pak(std::move(file));
               break;

            case Format::Wad:
             //dir = DirCreate_Wad(std::move(file));
               break;

            default:
               break;
            }
         }
         else
            dir = CreateDir_Directory(name);

         if(dir)
         {
            Roots.push_back(std::move(dir));
            return true;
         }
         else
            return false;
      }

      //
      // Dir::ClearRoot
      //
      void Dir::ClearRoot()
      {
         Roots.clear();
      }

      //
      // Dir::FindFile
      //
      File *Dir::FindFile(char *path)
      {
         for(auto itr = Roots.rbegin(), end = Roots.rend(); itr != end; ++itr)
            if(auto file = FS::FindFile(itr->get(), path)) return file;

         return nullptr;
      }

      //
      // Dir::FindFile
      //
      File *Dir::FindFile(char const *path)
      {
         return FindFile(GDCC::Core::StrDup(path).get());
      }

      //
      // File::findDir
      //
      Dir *File::findDir()
      {
         if(!dir) switch(format)
         {
         case Format::Pak:
          //dir = DirCreate_Pak(this);
            if(!dir) format = Format::Unknown;
            break;

         case Format::Wad:
          //dir = DirCreate_Wad(this));
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
}

// EOF

