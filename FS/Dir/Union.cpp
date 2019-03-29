//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Dirs backed by a union of Dirs.
//
//-----------------------------------------------------------------------------

#include "FS/Dir.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   //
   // Dir_Union
   //
   class Dir_Union : public Dir
   {
   public:
      Dir_Union(GDCC::Core::Array<Dir *> &&dirs_) :
         dirs{std::move(dirs_)},
         tmp{dirs.begin(), dirs.end(), [](Dir *){return DirPtr{nullptr, false};}}
      {
      }

      Dir_Union(Core::HashedStr name_, GDCC::Core::Array<DirPtr> &&dirs_) :
         Dir{name_},
         dirs{dirs_.begin(), dirs_.end(), [](DirPtr &d){return d.get();}},
         ptrs{std::move(dirs_)},
         tmp{dirs.begin(), dirs.end(), [](Dir *){return DirPtr{nullptr, false};}}
      {
      }

      virtual bool createDir(Core::HashedStr name);
      virtual bool createDirPath(Core::HashedStr path);

      virtual bool createFile(Core::HashedStr name,
         std::unique_ptr<char[]> &&data, std::size_t size);
      virtual bool createFilePath(Core::HashedStr path,
         std::unique_ptr<char[]> &&data, std::size_t size);

      virtual DirPtr findDir(Core::HashedStr name);
      virtual DirPtr findDirPath(Core::HashedStr path);

      virtual FilePtr findFile(Core::HashedStr name);
      virtual FilePtr findFilePath(Core::HashedStr path);

      virtual void flush();

      virtual bool hasName(Core::HashedStr name);

      virtual IterData        iterEnd();
      virtual IterRes         iterGet(IterData iter);
      virtual Core::HashedStr iterGetName(IterData iter);
      virtual IterData        iterNext(IterData iter);

      virtual void prune();

   protected:
      GDCC::Core::Array<Dir *>  dirs;
      GDCC::Core::Array<DirPtr> ptrs;

      GDCC::Core::Array<DirPtr> tmp;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // Dir_Union::createDir
   //
   bool Dir_Union::createDir(Core::HashedStr dirname)
   {
      for(auto &dir : dirs)
         if(dir->createDir(dirname)) return true;

      return false;
   }

   //
   // Dir_Union::createDirPath
   //
   bool Dir_Union::createDirPath(Core::HashedStr path)
   {
      for(auto &dir : dirs)
         if(dir->createDirPath(path)) return true;

      return false;
   }

   //
   // Dir_Union::createFile
   //
   bool Dir_Union::createFile(Core::HashedStr filename,
      std::unique_ptr<char[]> &&data, std::size_t size)
   {
      for(auto &dir : dirs)
         if(dir->createFile(filename, std::move(data), size)) return true;

      return false;
   }

   //
   // Dir_Union::createFilePath
   //
   bool Dir_Union::createFilePath(Core::HashedStr path,
      std::unique_ptr<char[]> &&data, std::size_t size)
   {
      for(auto &dir : dirs)
         if(dir->createFilePath(path, std::move(data), size)) return true;

      return false;
   }

   //
   // Dir_Union::findDir
   //
   Dir::DirPtr Dir_Union::findDir(Core::HashedStr dirname)
   {
      auto tmpPtr = tmp.begin();

      for(auto &dir : dirs)
         if(auto d = dir->findDir(dirname)) *tmpPtr++ = std::move(d);

      if(tmpPtr == &tmp[0]) return {nullptr, false};
      if(tmpPtr == &tmp[1]) return std::move(tmp[0]);
      return {new Dir_Union{tmp[0]->name,
         {GDCC::Core::Move, tmp.begin(), tmpPtr}}, true};
   }

   //
   // Dir_Union::findDirPath
   //
   Dir::DirPtr Dir_Union::findDirPath(Core::HashedStr path)
   {
      auto tmpPtr = tmp.begin();

      for(auto &dir : dirs)
         if(auto d = dir->findDirPath(path)) *tmpPtr++ = std::move(d);

      if(tmpPtr == &tmp[0]) return {nullptr, false};
      if(tmpPtr == &tmp[1]) return std::move(tmp[0]);
      return {new Dir_Union{tmp[0]->name,
         {GDCC::Core::Move, tmp.begin(), tmpPtr}}, true};
   }

   //
   // Dir_Union::findFile
   //
   Dir::FilePtr Dir_Union::findFile(Core::HashedStr filename)
   {
      for(auto &dir : dirs)
         if(auto *file = dir->findFile(filename)) return file;

      return nullptr;
   }

   //
   // Dir_Union::findFilePath
   //
   Dir::FilePtr Dir_Union::findFilePath(Core::HashedStr path)
   {
      for(auto &dir : dirs)
         if(auto *file = dir->findFilePath(path)) return file;

      return nullptr;
   }

   //
   // Dir_Union::flush
   //
   void Dir_Union::flush()
   {
      for(auto &dir : dirs)
         dir->flush();
   }

   //
   // Dir_Union::hasName
   //
   bool Dir_Union::hasName(Core::HashedStr filename)
   {
      for(auto &dir : dirs)
         if(dir->hasName(filename)) return true;

      return false;
   }

   //
   // Dir_Union::iterEnd
   //
   Dir::IterData Dir_Union::iterEnd()
   {
      IterData iter = 0;

      for(auto &dir : dirs)
         iter += dir->iterEnd();

      return iter;
   }

   //
   // Dir_Union::iterGet
   //
   Dir::IterRes Dir_Union::iterGet(IterData iter)
   {
      for(auto &dir : dirs)
      {
         IterData dirEnd = dir->iterEnd();

         if(iter < dirEnd)
         {
            if(auto d = findDir(dir->iterGetName(iter))) return d;
            return dir->iterGet(iter);
         }

         iter -= dirEnd;
      }

      return {};
   }

   //
   // Dir_Union::iterGetName
   //
   Core::HashedStr Dir_Union::iterGetName(IterData iter)
   {
      for(auto &dir : dirs)
      {
         IterData dirEnd = dir->iterEnd();

         if(iter < dirEnd)
            return dir->iterGetName(iter);

         iter -= dirEnd;
      }

      return {};
   }

   //
   // Dir_Union::iterNext
   //
   Dir::IterData Dir_Union::iterNext(IterData iter)
   {
      IterData base = 0;
      IterData dirEnd;

      auto dir = dirs.begin();

      // Optimization for first dir, since its names are always priority.
      if(dir != dirs.end())
      {
         dirEnd = (*dir)->iterEnd();

         if(iter < dirEnd)
         {
            if((iter = (*dir)->iterNext(iter)) != dirEnd)
               return iter;

            goto check_skip;
         }

         goto check_dir;
      }

      // Perform initial increment before checking for skips.
      for(; dir != dirs.end(); ++dir)
      {
         dirEnd = (*dir)->iterEnd();

         if(iter < dirEnd) {iter = (*dir)->iterNext(iter); goto check_skip;}

      check_dir:
         iter -= dirEnd;
         base += dirEnd;
      }

      for(; dir != dirs.end(); ++dir)
      {
         dirEnd = (*dir)->iterEnd();

      check_skip:
         while(iter != dirEnd)
         {
            Core::HashedStr itername = (*dir)->iterGetName(iter);

            // If name is not present in any previous dir, return iterator.
            if(!std::any_of(dirs.begin(), dir, [&](Dir *d){return d->hasName(itername);}))
               return iter + base;

            // Otherwise, skip iterator and try again.
            iter = (*dir)->iterNext(iter);
         }

         iter -= dirEnd;
         base += dirEnd;
      }

      return base;
   }

   //
   // Dir_Union::prune
   //
   void Dir_Union::prune()
   {
      for(auto &dir : dirs)
         dir->prune();
   }

   //
   // CreateDir_Union
   //
   std::unique_ptr<Dir> CreateDir_Union(GDCC::Core::Array<Dir *> &&dirs)
   {
      return std::unique_ptr<Dir>{new Dir_Union{std::move(dirs)}};
   }
}

// EOF

