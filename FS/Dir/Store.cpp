//-----------------------------------------------------------------------------
//
// Copyright (C) 2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dirs backed by writable in-memory files.
//
//-----------------------------------------------------------------------------

#include "FS/Dir.hpp"

#include "Core/BinaryIO.hpp"
#include "Core/Search.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class File_Store;

   //
   // Dir_Store
   //
   class Dir_Store : public Dir
   {
   public:
      Dir_Store(Core::HashedStr dirname) : Dir{dirname}
      {
         name.str = GDCC::Core::StrDup(name.str, name.len).release();
      }

      virtual ~Dir_Store()
      {
         delete[] name.str;
      }

      void archive(std::istream &in);
      void archive(std::ostream &out);

      virtual bool createDir(Core::HashedStr name);

      virtual FilePtr createFile(Core::HashedStr name);

      virtual DirPtr findDir(Core::HashedStr name);

      virtual FilePtr findFile(Core::HashedStr name);

      virtual IterData        iterEnd();
      virtual IterRes         iterGet(IterData iter);
      virtual Core::HashedStr iterGetName(IterData iter);

      virtual bool removeDir(Core::HashedStr name);

      virtual bool removeFile(Core::HashedStr name);

   protected:
      std::vector<std::unique_ptr<Dir_Store>>  dirs;
      std::vector<std::unique_ptr<File_Store>> files;
   };

   //
   // Dir_Extra
   //
   class Dir_Extra : public Dir
   {
   public:
      Dir_Extra();
      virtual ~Dir_Extra();

      virtual DirPtr findDir(Core::HashedStr name);

      virtual FilePtr findFile(Core::HashedStr name);

      virtual void flush();

      virtual IterData iterEnd();
      virtual IterRes  iterGet(IterData iter);

   protected:
      Dir_Store tmp;
      Dir_Store var;
   };

   //
   // File_Store
   //
   class File_Store : public File
   {
   public:
      File_Store(Core::HashedStr filename) : File{filename}, alloc{0}
      {
         name.str = GDCC::Core::StrDup(name.str, name.len).release();
      }

      ~File_Store()
      {
         std::free(const_cast<char *>(data));
         delete[] name.str;
      }

      void archive(std::istream &in);
      void archive(std::ostream &out);

      virtual bool trunc(std::size_t len);

      virtual std::size_t write(std::size_t pos, char const *buf, std::size_t len);

      std::size_t alloc;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // Dir_Store::archive
   //
   void Dir_Store::archive(std::istream &in)
   {
      std::vector<char> nameBuf;
      std::size_t       n;

      for(dirs.reserve(n = Core::ReadVLN<std::size_t>(in)); n--;)
      {
         nameBuf.resize(Core::ReadVLN<std::size_t>(in));
         in.read(nameBuf.data(), nameBuf.size());
         dirs.emplace_back(new Dir_Store{{nameBuf.data(), nameBuf.size()}});

         dirs.back()->archive(in);
      }

      for(files.reserve(n = Core::ReadVLN<std::size_t>(in)); n--;)
      {
         nameBuf.resize(Core::ReadVLN<std::size_t>(in));
         in.read(nameBuf.data(), nameBuf.size());
         files.emplace_back(new File_Store{{nameBuf.data(), nameBuf.size()}});

         files.back()->archive(in);
      }
   }

   //
   // Dir_Store::archive
   //
   void Dir_Store::archive(std::ostream &out)
   {
      Core::WriteVLN(out, dirs.size());
      for(auto &dir : dirs)
      {
         Core::WriteVLN(out, dir->name.len);
         out.write(dir->name.str, dir->name.len);
         dir->archive(out);
      }

      Core::WriteVLN(out, files.size());
      for(auto &file : files)
      {
         Core::WriteVLN(out, file->name.len);
         out.write(file->name.str, file->name.len);
         file->archive(out);
      }
   }

   //
   // Dir_Store::createDir
   //
   bool Dir_Store::createDir(Core::HashedStr dirname)
   {
      if(findDir(dirname))
         return true;

      dirs.emplace_back(new Dir_Store{dirname});
      std::sort(dirs.begin(), dirs.end(),
         [](std::unique_ptr<Dir_Store> const &l, std::unique_ptr<Dir_Store> const &r)
            {return l->name < r->name;});

      return true;
   }

   //
   // Dir_Store::createFile
   //
   Dir::FilePtr Dir_Store::createFile(Core::HashedStr filename)
   {
      auto file = static_cast<File_Store *>(findFile(filename));

      if(!file)
      {
         file = files.emplace_back(new File_Store(filename)).get();
         std::sort(files.begin(), files.end(),
            [](std::unique_ptr<File_Store> const &l, std::unique_ptr<File_Store> const &r)
               {return l->name < r->name;});
      }

      return file;
   }

   //
   // Dir_Store::findDir
   //
   Dir::DirPtr Dir_Store::findDir(Core::HashedStr dirname)
   {
      if(auto dir = Core::BSearchMemPtr(dirs.begin(), dirs.end(), dirname,
         [](std::unique_ptr<Dir_Store> const &d){return d->name;}))
         return {dir->get(), false};

      return Dir::findDir(dirname);
   }

   //
   // Dir_Store::findFile
   //
   Dir::FilePtr Dir_Store::findFile(Core::HashedStr filename)
   {
      if(auto file = Core::BSearchMemPtr(files.begin(), files.end(), filename,
         [](std::unique_ptr<File_Store> const &f){return f->name;}))
         return file->get();

      return nullptr;
   }

   //
   // Dir_Store::iterEnd
   //
   Dir::IterData Dir_Store::iterEnd()
   {
      return dirs.size() + files.size();
   }

   //
   // Dir_Store::iterGet
   //
   Dir::IterRes Dir_Store::iterGet(IterData iter)
   {
      if(iter < dirs.size())
         return {{dirs[iter].get(), false}};

      if((iter -= dirs.size()) < files.size())
         return files[iter].get();

      return {};
   }

   //
   // Dir_Store::iterGetName
   //
   Core::HashedStr Dir_Store::iterGetName(IterData iter)
   {
      if(iter < dirs.size())
         return dirs[iter]->name;

      if((iter -= dirs.size()) < files.size())
         return files[iter]->name;

      return nullptr;
   }

   //
   // Dir_Store::removeDir
   //
   bool Dir_Store::removeDir(Core::HashedStr dirname)
   {
      auto itr = Core::BSearchMem(dirs.begin(), dirs.end(), dirname,
         [](std::unique_ptr<Dir_Store> const &i){return i->name;});

      // Don't remove non-empty directories.
      if(itr == dirs.end() || !(*itr)->dirs.empty() || !(*itr)->files.empty())
         return false;

      dirs.erase(itr);
      return true;
   }

   //
   // Dir_Store::removeFile
   //
   bool Dir_Store::removeFile(Core::HashedStr filename)
   {
      auto itr = Core::BSearchMem(files.begin(), files.end(), filename,
         [](std::unique_ptr<File_Store> const &i){return i->name;});

      // Don't remove files with refs.
      if(itr == files.end() || (*itr)->refs)
         return false;

      files.erase(itr);
      return true;
   }

   //
   // Dir_Extra constructor
   //
   Dir_Extra::Dir_Extra() :
      tmp{"tmp"}, var{"var"}
   {
      if(VarArchive.data())
      {
         std::ifstream in{VarArchive.data(), std::ios_base::in | std::ios_base::binary};
         if(in && !std::memcmp(Core::ReadData<8>(in).data(), "DGE_FS\0\0", 8))
            var.archive(in);
      }
   }

   //
   // Dir_Extra destructor
   //
   Dir_Extra::~Dir_Extra()
   {
      try {flush();} catch(...) {}
   }

   //
   // Dir_Extra::findDir
   //
   Dir::DirPtr Dir_Extra::findDir(Core::HashedStr dirname)
   {
      if(dirname == tmp.name) return {&tmp, false};
      if(dirname == var.name) return {&var, false};
      return {nullptr, false};
   }

   //
   // Dir_Extra::findFile
   //
   Dir::FilePtr Dir_Extra::findFile(Core::HashedStr)
   {
      return nullptr;
   }

   //
   // Dir_Extra::flush
   //
   void Dir_Extra::flush()
   {
      if(VarArchive.data())
      {
         std::ofstream out{VarArchive.data(), std::ios_base::out | std::ios_base::binary};
         if(out)
         {
            out.write("DGE_FS\0\0", 8);
            var.archive(out);
         }
         else
            std::cerr << "Failed to open '" << VarArchive.data() << "' for writing.\n";
      }
   }

   //
   // Dir_Extra::iterEnd
   //
   Dir::IterData Dir_Extra::iterEnd()
   {
      return 2;
   }

   //
   // Dir_Extra::iterGet
   //
   Dir::IterRes Dir_Extra::iterGet(IterData iter)
   {
      return {{iter ? &var : &tmp, false}};
   }

   //
   // File_Store::archive
   //
   void File_Store::archive(std::istream &in)
   {
      size = Core::ReadVLN<std::size_t>(in);
      data = new char[size];
      in.read(const_cast<char *>(data), size);
   }

   //
   // File_Store::archive
   //
   void File_Store::archive(std::ostream &out)
   {
      Core::WriteVLN(out, size);
      out.write(data, size);
   }

   //
   // File_Store::trunc
   //
   bool File_Store::trunc(std::size_t len)
   {
      if(alloc < len)
      {
         void *dataNew = std::realloc(const_cast<char *>(data), len);
         if(!dataNew) return false;
         data  = static_cast<char *>(dataNew);
         alloc = len;
      }

      if(size < len)
         std::memset(const_cast<char *>(data) + size, 0, len - size);

      size = len;
      return true;
   }

   //
   // File_Store::write
   //
   std::size_t File_Store::write(std::size_t pos, char const *buf, std::size_t len)
   {
      if(len > SIZE_MAX - pos)
         len = SIZE_MAX - pos;

      std::size_t sizeNew = pos + len;

      if(alloc < sizeNew)
      {
         void *dataNew = std::realloc(const_cast<char *>(data), sizeNew);
         if(!dataNew) return 0;
         data  = static_cast<char *>(dataNew);
         alloc = sizeNew;
      }

      size = sizeNew;
      std::memcpy(const_cast<char *>(data) + pos, buf, len);
      format = DetectFormat(data, size);

      return len;
   }

   //
   // CreateDir_Extra
   //
   std::unique_ptr<Dir> CreateDir_Extra()
   {
      return std::unique_ptr<Dir>{new Dir_Extra};
   }
}

// EOF

