//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// File access natives.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/Convert.hpp"
#include "Code/MemStr.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include "FS/Dir.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // OpenDir
   //
   class OpenDir
   {
   public:
      OpenDir() : dir{nullptr, false}, iter{0}, iterEnd{0} {}

      void close()
      {
         dir = nullptr;
      }

      void open(FS::Dir::DirPtr &&dir_)
      {
         dir     = std::move(dir_);
         iter    = 0;
         iterEnd = dir->iterEnd();
      }

      bool read()
      {
         if(iter == iterEnd)
            return false;

         iterRes = dir->iterGet(iter);
         iter    = dir->iterNext(iter);
         return true;
      }

      void rewind()
      {
         iterRes = {};
         iter    = 0;
      }

      FS::Dir::IterRes  iterRes;
      FS::Dir::DirPtr   dir;
      FS::Dir::IterData iter;
      FS::Dir::IterData iterEnd;


      static OpenDir *GetFree()
      {
         for(auto &od : Dirs)
            if(!od.dir) return &od;

         return Dirs.emplace_back(), &Dirs.back();
      }

      static std::size_t Open(FS::Dir::DirPtr &&dir)
      {
         OpenDir *od = OpenDir::GetFree();
         od->open(std::move(dir));
         return od - Dirs.data();
      }

      static std::vector<OpenDir> Dirs;
   };

   //
   // OpenFile
   //
   class OpenFile
   {
   public:
      OpenFile() : file{nullptr} {}

      void close()
      {
         --file->refs;
         file = nullptr;
      }

      void open(FS::Dir::FilePtr file_)
      {
         file = file_;
         ++file->refs;
      }

      FS::Dir::FilePtr file;


      static OpenFile *GetFree()
      {
         for(auto &of : Files)
            if(!of.file) return &of;

         return Files.emplace_back(), &Files.back();
      }

      static std::size_t Open(FS::Dir::FilePtr file)
      {
         OpenFile *of = OpenFile::GetFree();
         of->open(file);
         return of - Files.data();
      }

      static std::vector<OpenFile> Files;
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   std::vector<OpenDir> OpenDir::Dirs;
   std::vector<OpenFile> OpenFile::Files;
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   DGE_Code_NativeLoaderDefn(Native_File);

   //
   // void DGE_Dir_Close(int fd)
   //
   DGE_Code_NativeDefn(DGE_Dir_Close)
   {
      OpenDir::Dirs[argv[0]].close();
      return false;
   }

   //
   // unsigned DGE_Dir_GetWork(char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Dir_GetWork)
   {
      auto work = task->proc->workDir;

      MemStrNCpy({&task->prog->memory, argv[0]}, argv[1], work.str, work.len);

      task->dataStk.push(work.len);
      return false;
   }

   //
   // unsigned DGE_Dir_Name(int fd, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Dir_Name)
   {
      Core::HashedStr name = OpenDir::Dirs[argv[0]].dir->name;

      MemStrNCpy({&task->prog->memory, argv[1]}, argv[2], name.str, name.len);

      task->dataStk.push(name.len);
      return false;
   }

   //
   // int DGE_Dir_Open(char const *name)
   //
   DGE_Code_NativeDefn(DGE_Dir_Open)
   {
      MemPtr<Byte const> name = {&task->prog->memory, argv[0]};
      FS::Dir::DirPtr    dir  = {nullptr, false};

      if(*name == '/')
         dir = FS::Dir::Root->findDirPath(MemStrDup(name + 1).get());
      else
         dir = task->proc->getWorkDir()->findDirPath(MemStrDup(name).get());

      if(!dir) {task->dataStk.push(-1); return false;}

      task->dataStk.push(OpenDir::Open(std::move(dir)));
      return false;
   }

   //
   // int DGE_Dir_Read(int fd)
   //
   DGE_Code_NativeDefn(DGE_Dir_Read)
   {
      task->dataStk.push(OpenDir::Dirs[argv[0]].read());
      return false;
   }

   //
   // int DGE_Dir_ReadDir(int fd)
   //
   DGE_Code_NativeDefn(DGE_Dir_ReadDir)
   {
      auto &dir = OpenDir::Dirs[argv[0]].iterRes.dir;
      if(!dir) {task->dataStk.push(-1); return false;}

      task->dataStk.push(OpenDir::Open(std::move(dir)));
      return false;
   }

   //
   // int DGE_Dir_ReadFile(int fd)
   //
   DGE_Code_NativeDefn(DGE_Dir_ReadFile)
   {
      auto &file = OpenDir::Dirs[argv[0]].iterRes.file;
      if(!file) {task->dataStk.push(-1); return false;}

      task->dataStk.push(OpenFile::Open(file));
      return false;
   }

   //
   // unsigned DGE_Dir_ReadName(int fd, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Dir_ReadName)
   {
      Core::HashedStr name = OpenDir::Dirs[argv[0]].iterRes.name();

      MemStrNCpy({&task->prog->memory, argv[1]}, argv[2], name.str, name.len);

      task->dataStk.push(name.len);
      return false;
   }

   //
   // void DGE_Dir_Rewind(int fd)
   //
   DGE_Code_NativeDefn(DGE_Dir_Rewind)
   {
      OpenDir::Dirs[argv[0]].rewind();
      return false;
   }

   //
   // int DGE_Dir_SetWork(char *path)
   //
   DGE_Code_NativeDefn(DGE_Dir_SetWork)
   {
      MemPtr<Byte const> path = {&task->prog->memory, argv[0]};

      std::size_t             workLen;
      std::unique_ptr<char[]> workStr;

      if(*path == '/')
      {
         std::tie(workStr, workLen) = MemStrDupLen(path + 1);
      }
      else
      {
         auto work    = task->proc->workDir;
         auto pathLen = MemStrLen(path);

         workLen = work.len + 1 + pathLen;
         workStr.reset(new char[workLen]);

         std::memcpy(&workStr[0], work.str, work.len);
         workStr[work.len] = '/';
         MemMemCpy(&workStr[work.len + 1], path, pathLen);
      }

      if(FS::Dir::Root->findDirPath({workStr.get(), workLen}))
      {
         task->proc->setWorkDir(std::move(workStr), workLen);
         task->dataStk.push(0);
      }
      else
         task->dataStk.push(-1);

      return false;
   }

   //
   // void DGE_File_Close(int fd)
   //
   DGE_Code_NativeDefn(DGE_File_Close)
   {
      OpenFile::Files[argv[0]].close();
      return false;
   }

   //
   // int DGE_File_Get(int fd, unsigned long idx)
   //
   DGE_Code_NativeDefn(DGE_File_Get)
   {
      auto &file = OpenFile::Files[argv[0]].file;
      DWord idx  = WordToDWord(argv+1);

      if(idx < file->size)
         task->dataStk.push(static_cast<Byte>(file->data[idx]));
      else
         task->dataStk.push(-1);

      return false;
   }

   //
   // unsigned DGE_File_Name(int fd, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_File_Name)
   {
      Core::HashedStr name = OpenFile::Files[argv[0]].file->name;

      MemStrNCpy({&task->prog->memory, argv[1]}, argv[2], name.str, name.len);

      task->dataStk.push(name.len);
      return false;
   }

   //
   // int DGE_File_Open(char const *name)
   //
   DGE_Code_NativeDefn(DGE_File_Open)
   {
      MemPtr<Byte const> name = {&task->prog->memory, argv[0]};
      FS::Dir::FilePtr   file;

      if(*name == '/')
         file = FS::Dir::Root->findFilePath(MemStrDup(name + 1).get());
      else
         file = task->proc->getWorkDir()->findFilePath(MemStrDup(name).get());

      if(!file) {task->dataStk.push(-1); return false;}

      task->dataStk.push(OpenFile::Open(file));
      return false;
   }

   //
   // int DGE_File_Read(int fd, unsigned long idx, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_File_Read)
   {
      auto        &file = OpenFile::Files[argv[0]].file;
      DWord        idx  = WordToDWord(argv+1);
      MemPtr<Byte> buf  = {&task->prog->memory, argv[3]};
      Word         len  = argv[4];

      if(idx > file->size)
         {task->dataStk.push(-1); return false;}

      if(len > file->size - idx)
         len = file->size - idx;

      for(char const *itr = file->data + idx, *end = itr + len; itr != end; ++itr, ++buf)
         *buf = *itr;

      task->dataStk.push(len);

      return false;
   }

   //
   // unsigned long DGE_File_Size(int fd)
   //
   DGE_Code_NativeDefn(DGE_File_Size)
   {
      DWord size = OpenFile::Files[argv[0]].file->size;
      task->dataStk.push(static_cast<Word>(size >>  0));
      task->dataStk.push(static_cast<Word>(size >> 32));
      return false;
   }
}

// EOF

