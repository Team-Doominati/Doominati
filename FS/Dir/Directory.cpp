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

#include <GDCC/Core/Array.hpp>
#include <GDCC/Core/Dir.hpp>
#include <GDCC/Core/File.hpp>
#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class File_Directory;

   //
   // Dir_Directory
   //
   class Dir_Directory : public Dir
   {
   public:
      virtual DirPtr findDir(Core::HashedStr name);

      virtual FilePtr findFile(Core::HashedStr name);

      virtual IterData        iterEnd();
      virtual IterRes         iterGet(IterData iter);
      virtual Core::HashedStr iterGetName(IterData iter);


      friend std::unique_ptr<Dir> CreateDir_Directory(char const *name);

   protected:
      void buildTables(char const *dirname);

      bool loadLump(File_Directory *file);

      std::unique_ptr<char[]> dirName;

      GDCC::Core::Array<Dir_Directory> dirs;

      GDCC::Core::Array<File_Directory> files;
   };

   //
   // File_Directory
   //
   class File_Directory : public File
   {
   public:
      bool findFile();

      std::unique_ptr<GDCC::Core::FileBlock> file;
      std::unique_ptr<char[]>                fileName;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // Dir_Directory::buildTables
   //
   void Dir_Directory::buildTables(char const *dirname)
   {
      // Count files and dirs.
      std::size_t dirC = 0, fileC = 0;
      for(auto dir = GDCC::Core::DirOpenStream(dirname); dir->next();)
      {
         switch(dir->getStat().type)
         {
         case GDCC::Core::Stat::Type::Dir:  ++dirC;  break;
         case GDCC::Core::Stat::Type::File: ++fileC; break;
         default: break;
         }
      }

      dirs  = GDCC::Core::Array<Dir_Directory>{dirC};
      files = GDCC::Core::Array<File_Directory>{fileC};

      Dir_Directory  *dirItr  = dirs.begin();
      File_Directory *fileItr = files.begin();

      // Set names. Load them as-needed.
      for(auto dir = GDCC::Core::DirOpenStream(dirname); dir->next();)
      {
         switch(dir->getStat().type)
         {
         case GDCC::Core::Stat::Type::Dir:
            dirItr->dirName = GDCC::Core::StrDup(dir->getStrFull(), dir->getLenFull());
            dirItr->name    = &dirItr->dirName[dir->getLenBase()];
            ++dirItr;
            break;

         case GDCC::Core::Stat::Type::File:
            fileItr->fileName = GDCC::Core::StrDup(dir->getStrFull(), dir->getLenFull());
            fileItr->name     = &fileItr->fileName[dir->getLenBase()];
            ++fileItr;
            break;

         default:
            break;
         }
      }

      // Sort the lists for binary search.
      std::sort(files.begin(), files.end(),
         [](File_Directory const &l, File_Directory const &r)
            {return l.name < r.name;});

      std::sort(dirs.begin(), dirs.end(),
         [](Dir_Directory const &l, Dir_Directory const &r)
            {return l.name < r.name;});
   }

   //
   // Dir_Directory::findDir
   //
   Dir::DirPtr Dir_Directory::findDir(Core::HashedStr dirname)
   {
      if(auto dir = Core::BSearchKey(dirs.begin(), dirs.end(), dirname))
      {
         if(dir->files.empty() && dir->dirs.empty())
            dir->buildTables(dir->dirName.get());

         return {dir, false};
      }

      return Dir::findDir(dirname);
   }

   //
   // Dir_Directory::findFile
   //
   Dir::FilePtr Dir_Directory::findFile(Core::HashedStr filename)
   {
      if(auto file = Core::BSearchKey(files.begin(), files.end(), filename))
         return file->findFile() ? file : nullptr;

      return nullptr;
   }

   //
   // Dir_Directory::iterEnd
   //
   Dir::IterData Dir_Directory::iterEnd()
   {
      return dirs.size() + files.size();
   }

   //
   // Dir_Directory::iterGet
   //
   Dir::IterRes Dir_Directory::iterGet(IterData iter)
   {
      if(iter < dirs.size())
      {
         auto &dir = dirs[iter];

         if(dir.files.empty() && dir.dirs.empty())
            dir.buildTables(dir.dirName.get());

         return {{&dir, false}};
      }

      if((iter -= dirs.size()) < files.size())
      {
         auto &file = files[iter];

         if(!file.findFile()) return {};

         return {{file.findDir(), false}, &file};
      }

      return {};
   }

   //
   // Dir_Directory::iterGetName
   //
   Core::HashedStr Dir_Directory::iterGetName(IterData iter)
   {
      if(iter < dirs.size())
         return dirs[iter].name;

      if((iter -= dirs.size()) < files.size())
         return files[iter].name;

      return nullptr;
   }

   //
   // File_Directory::findFile
   //
   bool File_Directory::findFile()
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

// EOF

