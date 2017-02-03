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
      using ForFunc = std::function<void(File *)>;


      Dir() : name{""} {}
      Dir(Dir const &) = delete;
      Dir(Dir &&) = default;
      virtual ~Dir() {}

      Dir &operator = (Dir const &) = delete;
      Dir &operator = (Dir &&) = default;

      virtual Dir *findDir(char const *name);

      virtual File *findFile(char const *name) = 0;

      virtual void forFile(ForFunc const &fn) = 0;

      // Frees unused file allocations.
      virtual void prune();

      char const *name;


      static bool AddRoot(char const *filename);

      static void ClearRoot();

      static File *FindFile(char       *path);
      static File *FindFile(char const *path);

      static void ForFile(char       *path, ForFunc const &fn);
      static void ForFile(char const *path, ForFunc const &fn);
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
   std::unique_ptr<Dir> CreateDir_Directory(char const *name);
   std::unique_ptr<Dir> CreateDir_Pak(DirData &&dd);
   std::unique_ptr<Dir> CreateDir_Wad(DirData &&dd);
   std::unique_ptr<Dir> CreateDir_Zip(DirData &&dd);
}

#endif//DGE__FS__Dir_H__

