//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dirs backed by native directories.
//
//-----------------------------------------------------------------------------

#include "FS/Dir.hpp"

#include "Core/Search.hpp"

#include <GDCC/Core/Dir.hpp>
#include <GDCC/Core/File.hpp>
#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace FS
   {
      class File_Directory;

      //
      // Dir_Directory
      //
      class Dir_Directory : public Dir
      {
      public:
         virtual Dir *findDir(char const *name);

         virtual File *findFile(char const *name);


         friend std::unique_ptr<Dir> CreateDir_Directory(char const *name);

      protected:
         Dir_Directory() : dirC{0}, fileC{0} {}

         void buildTables(char const *dirname);

         bool loadLump(File_Directory *file);

         std::unique_ptr<char[]> dirName;

         std::unique_ptr<Dir_Directory[]> dirs;
         std::size_t dirC;

         std::unique_ptr<File_Directory[]> files;
         std::size_t fileC;
      };

      //
      // File_Directory
      //
      class File_Directory : public File
      {
      public:
         bool readFile();

         std::unique_ptr<GDCC::Core::FileBlock> file;
         std::unique_ptr<char[]>                fileName;
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
      //
      // Dir_Directory::buildTables
      //
      void Dir_Directory::buildTables(char const *dirname)
      {
         // Count files and dirs.
         for(auto dir = GDCC::Core::DirOpenStream(dirname); dir->next();)
         {
            if(GDCC::Core::IsDir(dir->getFull()))
               ++dirC;
            else
               ++fileC;
         }

         if(dirC) dirs.reset(new Dir_Directory[dirC]);
         if(fileC) files.reset(new File_Directory[fileC]);

         Dir_Directory *dirItr = dirs.get();
         File_Directory *fileItr = files.get();

         // Set names. Load them as-needed.
         for(auto dir = GDCC::Core::DirOpenStream(dirname); dir->next();)
         {
            char const *strFull = dir->getFull();
            std::size_t lenFull = std::strlen(strFull);
            std::size_t lenPart = std::strlen(dir->getPart());

            if(GDCC::Core::IsDir(strFull))
            {
               dirItr->dirName = GDCC::Core::StrDup(strFull, lenFull);
               dirItr->name    = &dirItr->dirName[lenFull - lenPart];
               ++dirItr;
            }
            else
            {
               fileItr->fileName = GDCC::Core::StrDup(strFull, lenFull);
               fileItr->name     = &fileItr->fileName[lenFull - lenPart];
               ++fileItr;
            }
         }

         // Sort the lists for binary search.
         std::sort(&files[0], &files[fileC],
            [](File_Directory const &l, File_Directory const &r)
               {return std::strcmp(l.name, r.name) < 0;});

         std::sort(&dirs[0], &dirs[dirC],
            [](Dir_Directory const &l, Dir_Directory const &r)
               {return std::strcmp(l.name, r.name) < 0;});
      }

      //
      // Dir_Directory::findDir
      //
      Dir *Dir_Directory::findDir(char const *dirname)
      {
         if(auto dir = Core::BSearchStr(&dirs[0], &dirs[dirC], dirname))
         {
            if(!dir->files && ! dir->dirs)
               dir->buildTables(dir->dirName.get());

            return dir;
         }

         return Dir::findDir(dirname);
      }

      //
      // Dir_Directory::findFile
      //
      File *Dir_Directory::findFile(char const *filename)
      {
         if(auto file = Core::BSearchStr(&files[0], &files[fileC], filename))
            return file->readFile() ? file : nullptr;

         return nullptr;
      }

      //
      // File_Directory::readFile
      //
      bool File_Directory::readFile()
      {
         if(file)
            return true;

         try
         {
            file = GDCC::Core::FileOpenBlock(fileName.get());
         }
         catch(std::exception const &)
         {
            return false;
         }

         format = DetectFormat(data = file->data(), size = file->size());

         return true;
      }

      //
      // CreateDir_Directory
      //
      std::unique_ptr<Dir> CreateDir_Directory(char const *name)
      {
         std::unique_ptr<Dir_Directory> dir{new Dir_Directory};

         try
         {
            dir->buildTables(name);
         }
         catch(std::exception const &)
         {
            return nullptr;
         }

         return std::unique_ptr<Dir>(dir.release());
      }
   }
}

// EOF

