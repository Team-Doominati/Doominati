//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dirs backed by Quake PAK archives.
//
//-----------------------------------------------------------------------------

#include "FS/Dir.hpp"

#include "FS/Path.hpp"

#include "Core/BinaryIO.hpp"
#include "Core/Search.hpp"

#include <GDCC/Core/Array.hpp>
#include <GDCC/Core/Dir.hpp>
#include <GDCC/Core/File.hpp>
#include <GDCC/Core/String.hpp>

#include <map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class Dir_PakSub;
   class DirLoader_Pak;
   class File_Pak;

   using PakMap = DirMap<File_Pak>;

   //
   // Dir_PakBase
   //
   class Dir_PakBase : public Dir
   {
   public:
      virtual DirPtr findDir(Core::HashedStr dirname);

      virtual FilePtr findFile(Core::HashedStr filename);

      virtual IterData iterEnd();
      virtual IterRes  iterGet(IterData iter);

   protected:
      Dir_PakBase() : files{nullptr}, fileC{0}, dirs{nullptr}, dirC{0} {}

      File_Pak   *files;
      std::size_t fileC;

      Dir_PakSub *dirs;
      std::size_t dirC;
   };

   //
   // Dir_Pak
   //
   class Dir_Pak : public Dir_PakBase
   {
   public:
      Dir_Pak(DirData &&dd_) : dd{std::move(dd_)} {}
      virtual ~Dir_Pak();


      friend class DirLoader_Pak;

      friend std::unique_ptr<Dir> CreateDir_Pak(DirData &&dd);

   protected:
      DirData dd;
   };

   //
   // Dir_PakSub
   //
   class Dir_PakSub : public Dir_PakBase
   {
   public:
      Dir_PakSub() : nameData{'\0'} {name = nameData;}

      void setName(char const *str, char const *end);


      friend class DirLoader_Pak;

   protected:
      char nameData[56];
   };

   //
   // DirLoader_Pak
   //
   class DirLoader_Pak
   {
   public:
      DirLoader_Pak(char const *data_, std::size_t size_) : data{data_}, size{size_} {}

      void buildTables(Dir_Pak *dir);
      void buildTables(Dir_Pak *root, PakMap const &map, Dir_PakSub *dirs);

      void loadDirectory(Dir_Pak *dir);

      void loadFile(File_Pak *file, char const fileHead[64]);

      void loadHeaderPACK();

      char const *data;
      std::size_t size;

      char const *dirData;
      std::size_t dirSize;
   };

   //
   // File_Pak
   //
   class File_Pak : public File
   {
   };

   //
   // PakError
   //
   class PakError : public std::exception
   {
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // Dir_PakBase::findDir
   //
   Dir::DirPtr Dir_PakBase::findDir(Core::HashedStr dirname)
   {
      if(auto dir = Core::BSearchKey(dirs, dirs + dirC, dirname))
         return {dir, false};

      return Dir::findDir(dirname);
   }

   //
   // Dir_PakBase::findFile
   //
   Dir::FilePtr Dir_PakBase::findFile(Core::HashedStr filename)
   {
      return Core::BSearchKey(files, files + fileC, filename);
   }

   //
   // Dir_PakBase::iterEnd
   //
   Dir::IterData Dir_PakBase::iterEnd()
   {
      return dirC + fileC;
   }

   //
   // Dir_PakBase::iterGet
   //
   Dir::IterRes Dir_PakBase::iterGet(IterData iter)
   {
      if(iter < dirC)
         return {{&dirs[iter], false}};

      if((iter -= dirC) < fileC)
         return {{files[iter].findDir(), false}, &files[iter]};

      return {};
   }

   //
   // Dir_Pak destructor
   //
   Dir_Pak::~Dir_Pak()
   {
      delete[] files;
      delete[] dirs;
   }

   //
   // DirLoader_Pak::buildTables
   //
   void DirLoader_Pak::buildTables(Dir_Pak *dir)
   {
      // Count sub-dirs.
      PakMap map{dir->files, dir->files + dirSize};
      dir->dirC  = map.map.size();
      dir->fileC = map.fileC;

      // Create sub-dirs.
      dir->dirs = new Dir_PakSub[map.count()];
      buildTables(dir, map, dir->dirs);

      // Finish lump names.
      for(auto i = dir->files, e = i + dirSize; i != e; ++i)
         if(auto s = std::strrchr(i->name.str, '/')) i->name = s + 1;
   }

   //
   // DirLoader_Pak::buildTables
   //
   void DirLoader_Pak::buildTables(Dir_Pak *root, PakMap const &map, Dir_PakSub *dirs)
   {
      auto dirItr = dirs;
      auto dirSrc = dirs + map.map.size();
      for(auto const &i : map.map)
      {
         auto const &m = i.second;

         dirItr->setName(i.first.name, i.first.end);

         dirItr->dirs = dirSrc;
         dirItr->dirC = m.map.size();

         dirItr->files = m.files;
         dirItr->fileC = m.fileC;

         ++dirItr;
         dirSrc += m.count();
      }

      dirItr = dirs;
      for(auto const &m : map.map)
         buildTables(root, m.second, dirItr++->dirs);
   }

   //
   // DirLoader_Pak::loadDirectory
   //
   void DirLoader_Pak::loadDirectory(Dir_Pak *dir)
   {
      // Allocate files.
      dir->files = new File_Pak[dirSize];

      // Process the lump headers.
      for(std::size_t i = 0; i != dirSize; ++i)
         loadFile(dir->files + i, dirData + i * 64);

      // Sort the files by name. This allows sub-dirs to point into this array.
      std::sort(dir->files, dir->files + dirSize,
         [](File_Pak const &l, File_Pak const &r)
            {return PathCompare(l.name.str, r.name.str) < 0;});
   }

   //
   // DirLoader_Pak::loadFile
   //
   void DirLoader_Pak::loadFile(File_Pak *file, char const fileHead[64])
   {
      // The file name must be null-terminated.
      if(fileHead[55])
         throw PakError();

      // Read file information.
      std::size_t fileOffset = Core::ReadLE4(fileHead + 56);
      std::size_t fileLength = Core::ReadLE4(fileHead + 60);

      // Check that the file fits in the provided data.
      if(fileOffset > size || fileLength > size - fileOffset)
         throw PakError();

      file->name = fileHead;
      file->data = data + fileOffset;
      file->size = fileLength;

      file->format = DetectFormat(file->data, file->size);
   }

   //
   // DirLoader_Pak::loadHeaderPACK
   //
   void DirLoader_Pak::loadHeaderPACK()
   {
      // Check signature.
      if(size < 12 || data[0] != 'P' || data[1] != 'A' || data[2] != 'C' || data[3] != 'K')
         throw PakError();

      // Read directory information.
      std::size_t dirOffset = Core::ReadLE4(data + 4);
      std::size_t dirLength = Core::ReadLE4(data + 8);

      // Length must be a multiple of 64.
      if(dirLength % 64)
         throw PakError();

      // Check that the directory fits in the provided data.
      if(dirOffset > size || dirLength > size - dirOffset)
         throw PakError();

      dirData = data + dirOffset;
      dirSize = dirLength / 64;
   }

   //
   // Dir_PakSub::setName
   //
   void Dir_PakSub::setName(char const *str, char const *end)
   {
      char *itr = nameData;
      while(str != end)
         *itr++ = *str++;

      *itr = '\0';
   }

   //
   // CreateDir_Pak
   //
   std::unique_ptr<Dir> CreateDir_Pak(DirData &&dd)
   {
      std::unique_ptr<Dir_Pak> dir{new Dir_Pak(std::move(dd))};

      try
      {
         DirLoader_Pak pak{dir->dd.data(), dir->dd.size()};

         pak.loadHeaderPACK();
         pak.loadDirectory(dir.get());
         pak.buildTables(dir.get());
      }
      catch(std::exception const &)
      {
         return nullptr;
      }

      return std::unique_ptr<Dir>{dir.release()};
   }
}


// EOF

