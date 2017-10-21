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

#ifndef DGE__FS__Dir_H__
#define DGE__FS__Dir_H__

#include "File.hpp"

#include <GDCC/Core/Array.hpp>
#include <GDCC/Core/Deleter.hpp>

#include <functional>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC::Core
{
   class FileBlock;
}

namespace DGE::FS
{
   //
   // Dir
   //
   class Dir
   {
   public:
      using DirFunc  = std::function<void(Dir *)>;
      using DirPtr   = std::unique_ptr<Dir, GDCC::Core::ConditionalDeleter<Dir>>;
      using FileFunc = std::function<void(File *)>;
      using FilePtr  = File *;
      using IterData = std::size_t;

      //
      // IterRes
      //
      class IterRes
      {
      public:
         IterRes() : dir{nullptr, false}, file{nullptr} {}
         IterRes(DirPtr &&dir_) : dir{std::move(dir_)}, file{nullptr} {}
         IterRes(DirPtr &&dir_, File *file_) : dir{std::move(dir_)}, file{file_} {}
         IterRes(File *file_) : dir{nullptr, false}, file{file_} {}

         Core::HashedStr name() {return dir ? dir->name : file ? file->name : nullptr;}

         DirPtr  dir;
         FilePtr file;
      };

      //
      // Iter
      //
      class Iter
      {
      public:
         Iter(Dir *dir_, IterData iter_) : dir{dir_}, iter{iter_} {}

         explicit operator IterData () const {return iter;}

         Iter &operator ++ () {iter = dir->iterNext(iter); return *this;}
         IterRes operator * () const {return dir->iterGet(iter);}

         bool operator == (Iter const &r) const {return iter == r.iter;}
         bool operator != (Iter const &r) const {return iter != r.iter;}

         DirPtr          getDir()  const {return dir->iterGetDir(iter);}
         FilePtr         getFile() const {return dir->iterGetFile(iter);}
         Core::HashedStr getName() const {return dir->iterGetName(iter);}

      private:
         Dir     *dir;
         IterData iter;
      };


      Dir() : name{""} {}
      Dir(Core::HashedStr name_) : name{name_} {}
      Dir(Dir const &) = delete;
      Dir(Dir &&) = default;
      virtual ~Dir() {}

      Dir &operator = (Dir const &) = delete;
      Dir &operator = (Dir &&) = default;

      Iter begin() {return {this, 0};}

      Iter end() {return {this, iterEnd()};}

      virtual DirPtr findDir(Core::HashedStr name);
      virtual DirPtr findDirPath(Core::HashedStr path);

      virtual FilePtr findFile(Core::HashedStr name) = 0;
      virtual FilePtr findFilePath(Core::HashedStr path);

      void forFile(FileFunc const &fn);
      void forFilePath(Core::HashedStr path, FileFunc const &fn);

      virtual bool hasName(Core::HashedStr name);

      // Accesses entries using an opaque iterator.
      virtual IterData        iterEnd() = 0;
      virtual IterRes         iterGet(IterData iter) = 0;
      virtual DirPtr          iterGetDir(IterData iter);
      virtual FilePtr         iterGetFile(IterData iter);
      virtual Core::HashedStr iterGetName(IterData iter);
      virtual IterData        iterNext(IterData iter);

      // Frees unused file allocations.
      virtual void prune();

      Core::HashedStr name;


      static std::unique_ptr<Dir> Root;
   };

   //
   // DirData
   //
   class DirData
   {
   public:
      DirData();
      DirData(DirData const &) = delete;
      DirData(DirData &&dd);
      DirData(File *file);
      DirData(std::unique_ptr<GDCC::Core::FileBlock> &&file);
      ~DirData();

      DirData &operator = (DirData const &) = delete;
      DirData &operator = (DirData &&dd);

      char const *data() const;

      std::size_t size() const;

   private:
      File                                  *fileFS;
      std::unique_ptr<GDCC::Core::FileBlock> fileBlock;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   std::unique_ptr<Dir> CreateDir(char const *filename);
   std::unique_ptr<Dir> CreateDir_Directory(char const *name);
   std::unique_ptr<Dir> CreateDir_Pak(DirData &&dd);
   std::unique_ptr<Dir> CreateDir_Union(GDCC::Core::Array<Dir *> &&dirs);
   std::unique_ptr<Dir> CreateDir_Wad(DirData &&dd);
   std::unique_ptr<Dir> CreateDir_Zip(DirData &&dd);
}

#endif//DGE__FS__Dir_H__

