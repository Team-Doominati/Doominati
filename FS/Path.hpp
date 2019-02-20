//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Virtual filesystem and archive path handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__FS__Path_H__
#define DGE__FS__Path_H__

#include "Types.hpp"

#include <algorithm>
#include <cstring>
#include <map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   //
   // DirMapNF
   //
   template<typename FT>
   class DirMapNF
   {
   public:
      char const *operator () (FT const *file) {return file->name.str;}
   };

   //
   // DirMapNL
   //
   template<typename FT>
   class DirMapNL
   {
   public:
      std::size_t operator () (FT const *file) {return file->name.len;}
   };

   //
   // DirMap
   //
   // Acts as an intermediary loading format.
   //
   template<typename FT, typename NF = DirMapNF<FT>, typename NL = DirMapNL<FT>>
   class DirMap
   {
   public:
      //
      // Name
      //
      struct Name
      {
         Name(char const *name_, char const *end_) : name{name_}, end{end_} {}

         bool operator < (Name const &r) const
            {return std::lexicographical_compare(name, end, r.name, r.end);}

         char const *name, *end;
      };


      DirMap() : files{nullptr}, fileC{0} {}

      //
      // constructor
      //
      DirMap(FT *fileItr, FT *fileEnd) : files{nullptr}, fileC{0}
      {
         for(; fileItr != fileEnd; ++fileItr)
            add(fileItr, NF()(fileItr), NL()(fileItr));
      }

      //
      // add
      //
      void add(FT *file, char const *name, std::size_t nameLen)
      {
         if(auto s = static_cast<char const *>(std::memchr(name, '/', nameLen)))
            map[{name, s}].add(file, s + 1, name + nameLen - s - 1);
         else if(!files)
            files = file, fileC = 1;
         else
            ++fileC;
      }

      //
      // count
      //
      std::size_t count() const
      {
         std::size_t n = map.size();
         for(auto const &m : map)
            n += m.second.count();

         return n;
      }

      std::map<Name, DirMap> map;
      FT                    *files;
      std::size_t            fileC;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   int PathCompare(char const *l, char const *r);
   int PathCompare(char const *l, std::size_t lenL, char const *r, std::size_t lenR);
}

#endif//DGE__FS__Path_H__

