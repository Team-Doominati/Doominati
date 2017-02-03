//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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
#include <tuple>
#include <cctype>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::FS
{
   static std::vector<std::unique_ptr<Dir>> Roots;
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::FS
{
   //
   // WalkPath
   //
   // Goes through a path and searches for sub-directories until the last path
   // part. Returns a pointer to the last path part, or nullptr if not found.
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

   //
   // ForFile
   //
   static void ForFile(Dir *dir, char *path, Dir::ForFunc const &func)
   {
      std::tie(dir, path) = WalkPath(dir, path);

      if(!path) return;

      if(Dir *searchDir = dir->findDir(path))
         searchDir->forFile(func);
      else if(File *file = dir->findFile(path))
         func(file);
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
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
         case Format::Pak: dir = CreateDir_Pak(std::move(file)); break;
       //case Format::Wad: dir = CreateDir_Wad(std::move(file)); break;
         case Format::Zip: dir = CreateDir_Zip(std::move(file)); break;

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
   // Dir::ForFile
   //
   void Dir::ForFile(char *path, ForFunc const &fn)
   {
      for(auto itr = Roots.rbegin(), end = Roots.rend(); itr != end; ++itr)
         FS::ForFile(itr->get(), path, fn);
   }

   //
   // Dir::ForFile
   //
   void Dir::ForFile(char const *path, ForFunc const &fn)
   {
      return ForFile(GDCC::Core::StrDup(path).get(), fn);
   }

   //
   // DirData default constructor
   //
   DirData::DirData() : fileFS{nullptr}, fileBlock{nullptr} {}

   //
   // DirData move constructor
   //
   DirData::DirData(DirData &&dd) = default;

   //
   // DirData constructor
   //
   DirData::DirData(File *file) : fileFS{file}
   {
      ++fileFS->refs;
   }

   //
   // DirData constructor
   //
   DirData::DirData(std::unique_ptr<GDCC::Core::FileBlock> &&file) :
      fileFS{nullptr}, fileBlock{std::move(file)}
   {
   }

   //
   // DirData destructor
   //
   DirData::~DirData()
   {
      if(fileFS) --fileFS->refs;
   }

   //
   // DirData::operator = DirData
   //
   DirData &DirData::operator = (DirData &&dd) = default;

   //
   // DirData::data
   //
   char const *DirData::data() const
   {
      if(fileFS)    return fileFS->data;
      if(fileBlock) return fileBlock->data();

      return nullptr;
   }

   //
   // DirData::size
   //
   std::size_t DirData::size() const
   {
      if(fileFS)    return fileFS->size;
      if(fileBlock) return fileBlock->size();

      return 0;
   }
}

// EOF

