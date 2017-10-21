//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// File access natives.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include "FS/Dir.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Code
{
   static std::vector<FS::File *> Files;
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   DGE_Code_NativeLoaderDefn(Native_File);

   //
   // void DGE_FileClose(int fd)
   //
   DGE_Code_NativeDefn(DGE_FileClose)
   {
      Word fd = argv[0];

      if(fd < Files.size())
      {
         FS::File *&file = Files[argv[0]];
         --file->refs;
         file = nullptr;
      }

      return false;
   }

   //
   // int DGE_FileGet(int fd, unsigned idx)
   //
   DGE_Code_NativeDefn(DGE_FileGet)
   {
      Word      fd  = argv[0];
      Word      idx = argv[1];
      FS::File *file;

      if(fd < Files.size() && (file = Files[fd]) && idx < file->size)
         task->dataStk.push(file->data[idx]);
      else
         task->dataStk.push(-1);

      return false;
   }

   //
   // int DGE_FileOpen(char const *name)
   //
   DGE_Code_NativeDefn(DGE_FileOpen)
   {
      MemPtr<Byte const> name = {&task->prog->memory, argv[0]};

      FS::File *file = FS::Dir::Root->findFilePath(MemStrDup(name).get());
      if(!file) {task->dataStk.push(-1); return false;}

      ++file->refs;

      for(auto &fp : Files)
      {
         if(!fp)
         {
            task->dataStk.push(&fp - Files.data());
            fp = file;
            return false;
         }
      }

      task->dataStk.push(Files.size());
      Files.push_back(file);
      return false;
   }

   //
   // int DGE_FileRead(int fd, unsigned idx, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_FileRead)
   {
      Word         fd  = argv[0];
      Word         idx = argv[1];
      MemPtr<Byte> buf = {&task->prog->memory, argv[2]};
      Word         len = argv[3];
      FS::File    *file;

      if(fd >= Files.size() || !(file = Files[fd]) || idx > file->size)
         {task->dataStk.push(-1); return false;}

      if(len > file->size - idx)
         len = file->size - idx;

      for(char const *itr = file->data + idx, *end = itr + len; itr != end; ++itr, ++buf)
         *buf = *itr;

      task->dataStk.push(len);

      return false;
   }

   //
   // unsigned DGE_FileSize(int fd)
   //
   DGE_Code_NativeDefn(DGE_FileSize)
   {
      Word      fd = argv[0];
      FS::File *file;

      if(fd < Files.size() && (file = Files[fd]))
         task->dataStk.push(file->size);
      else
         task->dataStk.push(0);

      return false;
   }
}

// EOF

