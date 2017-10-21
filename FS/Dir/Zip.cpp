//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dirs backed by ZIP archives.
//
//-----------------------------------------------------------------------------

#include "FS/Dir.hpp"

#include "FS/Path.hpp"

#include "Core/BinaryIO.hpp"
#include "Core/Search.hpp"
#include "Core/Zip.hpp"

#include <GDCC/Core/Range.hpp>
#include <GDCC/Core/String.hpp>

#include <algorithm>
#include <iostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class Dir_ZipSub;
   class DirLoader_Zip;
   class File_Zip;
   class File_ZipNF;
   class File_ZipNL;

   using ZipMap = DirMap<File_Zip, File_ZipNF, File_ZipNL>;

   //
   // Dir_ZipBase
   //
   class Dir_ZipBase : public Dir
   {
   public:
      virtual DirPtr findDir(Core::HashedStr dirname);

      virtual FilePtr findFile(Core::HashedStr filename);

      virtual IterData        iterEnd();
      virtual IterRes         iterGet(IterData iter);
      virtual Core::HashedStr iterGetName(IterData iter);

   protected:
      Dir_ZipBase() : files{nullptr}, fileC{0}, dirs{nullptr}, dirC{0} {}

      File_Zip   *files;
      std::size_t fileC;

      Dir_ZipSub *dirs;
      std::size_t dirC;
   };

   //
   // Dir_Zip
   //
   class Dir_Zip : public Dir_ZipBase
   {
   public:
      Dir_Zip(DirData &&dd_) : dd{std::move(dd_)} {}
      virtual ~Dir_Zip();


      friend class DirLoader_Zip;

      friend std::unique_ptr<Dir> CreateDir_Zip(DirData &&dd);

   protected:
      DirData dd;
   };

   //
   // Dir_ZipSub
   //
   class Dir_ZipSub : public Dir_ZipBase
   {
   public:
      Dir_ZipSub() {name = "";}
      virtual ~Dir_ZipSub() {}

      void setName(char const *str, char const *end);


      friend class DirLoader_Zip;

   protected:
      std::unique_ptr<char[]> nameData;
   };

   //
   // DirLoader_Zip
   //
   class DirLoader_Zip
   {
   public:
      DirLoader_Zip(char const *data_, std::size_t size_) :
         data{data_}, dataEnd{data + size_}, size{size_} {}

      void buildTables(Dir_Zip *dir);
      void buildTables(ZipMap const &map, Dir_ZipSub *dirs);

      void loadDirectory(Dir_Zip *dir);

      char const *loadFile(File_Zip *file, char const *head);

      void loadHeaderEOCD();

      char const *data;
      char const *dataEnd;
      std::size_t size;

      char const *cdData;
      std::size_t cdElem;
      std::size_t cdSize;

   private:
      char const *findEOCD();
   };

   //
   // File_Zip
   //
   class File_Zip : public File
   {
   public:
      File_Zip *findFile();

      void finishName();

      char const *dataRaw;
      std::size_t sizeRaw;

      char const *nameRaw;
      std::size_t nameLen;

      std::uint_least32_t crc;

      std::uint_least16_t comp;
      std::uint_least16_t flag;

   private:
      void findFile_Zip();

      std::unique_ptr<char[]> nameData;
      std::unique_ptr<char[]> dataData;
   };

   //
   // File_ZipNF
   //
   class File_ZipNF
   {
   public:
      char const *operator () (File_Zip const *file) {return file->nameRaw;}
   };

   //
   // File_ZipNL
   //
   class File_ZipNL
   {
   public:
      std::size_t operator () (File_Zip const *file) {return file->nameLen;}
   };

   //
   // ZipError
   //
   class ZipError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // Dir_Zip destructor
   //
   Dir_Zip::~Dir_Zip()
   {
      delete[] dirs;
      delete[] files;
   }

   //
   // Dir_ZipBase::findDir
   //
   Dir::DirPtr Dir_ZipBase::findDir(Core::HashedStr dirname)
   {
      if(auto dir = Core::BSearchKey(dirs, dirs + dirC, dirname))
         return {dir, false};

      return Dir::findDir(dirname);
   }

   //
   // Dir_ZipBase::findFile
   //
   Dir::FilePtr Dir_ZipBase::findFile(Core::HashedStr filename)
   {
      if(auto file = Core::BSearchKey(files, files + fileC, filename))
         return file->findFile();

      return nullptr;
   }

   //
   // Dir_ZipBase::idxEnd
   //
   Dir::IterData Dir_ZipBase::iterEnd()
   {
      return dirC + fileC;
   }

   //
   // Dir_ZipBase::iterGet
   //
   Dir::IterRes Dir_ZipBase::iterGet(IterData iter)
   {
      if(iter < dirC)
         return {{&dirs[iter], false}};

      if((iter -= dirC) < fileC)
      {
         auto &file = files[iter];

         if(!file.findFile()) return {};

         return {{file.findDir(), false}, &file};
      }

      return {};
   }

   //
   // Dir_ZipBase::iterGetName
   //
   Core::HashedStr Dir_ZipBase::iterGetName(IterData iter)
   {
      if(iter < dirC)
         return dirs[iter].name;

      if((iter -= dirC) < fileC)
         return files[iter].name;

      return nullptr;
   }

   //
   // Dir_ZipSub::setName
   //
   void Dir_ZipSub::setName(char const *str, char const *end)
   {
      nameData = GDCC::Core::StrDup(str, end - str);
      name     = nameData.get();
   }

   //
   // DirLoader_Zip::buildTables
   //
   void DirLoader_Zip::buildTables(Dir_Zip *dir)
   {
      // Count sub-dirs.
      ZipMap map{dir->files, dir->files + cdElem};
      dir->dirC  = map.map.size();
      dir->fileC = map.fileC;

      // Create sub-dirs.
      dir->dirs = new Dir_ZipSub[map.count()];
      buildTables(map, dir->dirs);

      // Finish file names.
      for(auto &file : GDCC::Core::MakeRange(dir->files, dir->files + cdElem))
         file.finishName();
   }

   //
   // DirLoader_Zip::buildTables
   //
   void DirLoader_Zip::buildTables(ZipMap const &map, Dir_ZipSub *dirs)
   {
      auto dirItr = dirs;
      auto dirSrc = dirs + map.map.size();
      for(auto const &i : map.map)
      {
         auto const &m = i.second;

         dirItr->setName(i.first.name, i.first.end);

         dirItr->dirs = dirSrc;
         dirItr->dirC = m.map.size();

         // First file is the directory itself.
         dirItr->files = m.files + 1;
         dirItr->fileC = m.fileC - 1;

         ++dirItr;
         dirSrc += m.count();
      }

      dirItr = dirs;
      for(auto const &m : map.map)
         buildTables(m.second, dirItr++->dirs);
   }

   //
   // DirLoader_Zip::findEOCD
   //
   char const *DirLoader_Zip::findEOCD()
   {
      if(size < 22)
         throw ZipError("too small for EOCD");

      char const *eocd = dataEnd - 22;
      for(std::size_t clen = 0;; --eocd, ++clen)
      {
         // Scan for EOCD signature.
         // As an extra check, ensure that the comment length fits the size.
         if(Core::ReadLE4(eocd) == 0x06054B50 && Core::ReadLE2(eocd + 20) == clen)
            return eocd;

         if(eocd == data || clen == 0xFFFF)
            throw ZipError("no EOCD found");
      }
   }

   //
   // DirLoader_Zip::loadDirectory
   //
   void DirLoader_Zip::loadDirectory(Dir_Zip *dir)
   {
      dir->files = new File_Zip[cdElem];

      // Read central directory entries.
      char const *itr = cdData;
      for(auto &file : GDCC::Core::MakeRange(dir->files, dir->files + cdElem))
         itr = loadFile(&file, itr);

      // Sort the files by name. This allows sub-dirs to point into this array.
      std::sort(dir->files, dir->files + cdElem,
         [](File_Zip const &l, File_Zip const &r)
            {return PathCompare(l.nameRaw, l.nameLen, r.nameRaw, r.nameLen) < 0;});
   }

   //
   // DirLoader_Zip::loadFile
   //
   char const *DirLoader_Zip::loadFile(File_Zip *file, char const *head)
   {
      std::size_t headSize = dataEnd - head;

      // Read base header fields.
      if(headSize < 46 || Core::ReadLE4(head) != 0x02014B50)
         throw ZipError("invalid file header");

    //verMade            = Core::ReadLE2(head +  4);
    //verNeed            = Core::ReadLE2(head +  6);
      file->flag         = Core::ReadLE2(head +  8);
      file->comp         = Core::ReadLE2(head + 10);
    //modTime            = Core::ReadLE2(head + 12);
    //modDate            = Core::ReadLE2(head + 14);
      file->crc          = Core::ReadLE4(head + 16);
      file->sizeRaw      = Core::ReadLE4(head + 20);
      file->size         = Core::ReadLE4(head + 24);
      file->nameLen      = Core::ReadLE2(head + 28);
      std::size_t extLen = Core::ReadLE2(head + 30);
      std::size_t cmtLen = Core::ReadLE2(head + 32);
    //disk               = Core::ReadLE2(head + 34);
    //attrInt            = Core::ReadLE2(head + 36);
    //attrExt            = Core::ReadLE4(head + 38);
      std::size_t offs   = Core::ReadLE4(head + 42);
      file->nameRaw      =               head + 46;

      // Check that file data fits in archive data.
      if(size < offs || size - offs < file->sizeRaw)
         throw ZipError("invalid file offset");

      // Check for enough space for name, extra, and comment fields.
      if(headSize - 46 < file->nameLen + extLen + cmtLen)
         throw ZipError("invalid file header size");

      char const *next = head + 46 + file->nameLen + extLen + cmtLen;

      // Parse just enough of the local header to find the data start.
      head     = data    + offs;
      headSize = dataEnd - head;

      if(headSize < 30 || Core::ReadLE4(head) != 0x04034B50)
         throw ZipError("invalid local header");

    //verNeed                = Core::ReadLE2(head +  4);
    //std::size_t locFlag    = Core::ReadLE2(head +  6);
    //std::size_t locComp    = Core::ReadLE2(head +  8);
    //modTime                = Core::ReadLE2(head + 10);
    //modDate                = Core::ReadLE2(head + 12);
    //std::size_t locCRC     = Core::ReadLE4(head + 14);
      std::size_t locSizeRaw = Core::ReadLE4(head + 18);
    //std::size_t locSize    = Core::ReadLE4(head + 22);
      std::size_t locNameLen = Core::ReadLE2(head + 26);
      std::size_t locExtLen  = Core::ReadLE2(head + 28);

      // Check for enough space for name, extra, and data fields.
      if(headSize - 30 < locNameLen + locExtLen + locSizeRaw)
         throw ZipError("invalid local header size");

      file->dataRaw = head + 30 + locNameLen + locExtLen;

      return next;
   }

   //
   // DirLoader_Zip::loadHeaderEOCD
   //
   void DirLoader_Zip::loadHeaderEOCD()
   {
      char const *eocd = findEOCD();
      std::size_t cdOffs;

      cdElem = Core::ReadLE2(eocd +  8);
      cdSize = Core::ReadLE4(eocd + 12);
      cdOffs = Core::ReadLE4(eocd + 16);

      // This disk must be the disk with the start of the central directory.
      if(Core::ReadLE2(eocd + 4) != Core::ReadLE2(eocd + 6))
         throw ZipError("does not contain CD");

      // This disk must contain all of the entries of the central directory.
      if(cdElem != Core::ReadLE2(eocd + 10))
         throw ZipError("does not contain full CD");

      // Central directory must be within data.
      if(cdOffs > size || size - cdOffs < cdSize)
         throw ZipError("invalid CD offset");

      cdData = data + cdOffs;
   }

   //
   // File_Zip::findFile
   //
   File_Zip *File_Zip::findFile()
   {
      if(data) return this;

      switch(comp)
      {
      case  0: data = dataRaw; break;
      case  8: findFile_Zip(); break;

      default: return nullptr;
      }

      if(!data) return nullptr;

      format = DetectFormat(data, size);

      return this;
   }

   //
   // File_Zip::findFile_Zip
   //
   void File_Zip::findFile_Zip()
   {
      Core::Zip zip{dataRaw, sizeRaw};

      try
      {
         dataData.reset(new char[size]);
         zip.loadStream(reinterpret_cast<Core::Byte *>(dataData.get()));
         data = dataData.get();
      }
      catch(Core::ZipError const &e)
      {
         std::cerr << "FS::File_Zip: ";
         std::cerr.write(nameRaw, nameLen);
         std::cerr << ": " << e.what() << std::endl;
      }
   }

   //
   // File_Zip::finishName
   //
   void File_Zip::finishName()
   {
      char const *nameEnd = nameRaw + nameLen;
      char c = '/';

      auto s = std::find_end(nameRaw, nameEnd, &c, &c + 1);
      if(s != nameEnd)
         nameData = GDCC::Core::StrDup(s + 1, nameEnd - s - 1);
      else
         nameData = GDCC::Core::StrDup(nameRaw, nameLen);

      name = nameData.get();
   }

   //
   // CreateDir_Zip
   //
   std::unique_ptr<Dir> CreateDir_Zip(DirData &&dd)
   {
      std::unique_ptr<Dir_Zip> dir{new Dir_Zip(std::move(dd))};

      try
      {
         DirLoader_Zip zip{dir->dd.data(), dir->dd.size()};

         zip.loadHeaderEOCD();
         zip.loadDirectory(dir.get());
         zip.buildTables(dir.get());
      }
      catch(std::exception const &e)
      {
         std::cerr << "FS::CreateDir_Zip: " << e.what() << std::endl;
         return nullptr;
      }

      return std::unique_ptr<Dir>{dir.release()};
   }
}

// EOF


