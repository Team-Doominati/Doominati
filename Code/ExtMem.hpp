//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Extension Members handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__ExtMem_H__
#define DGE__Code__ExtMem_H__

#include "Types.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/String.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC::Core
{
   class String;
}

namespace DGE::Code
{
   //
   // ExtensionMembers
   //
   class ExtensionMembers
   {
   public:
      ExtensionMembers(Core::HashedStr name, ExtensionMembers *parent);

      void add(Core::HashedStr name, Word size);

      Word *find(Core::HashedStr name) const;

      void finish();

      Word max() const {return memMax;}

   private:
      Core::HashMapFixed<Core::HashedStr, Word>      memMap;
      std::unique_ptr<char[]>                        memNames;
      std::vector<std::pair<Core::HashedStr, Word>> *memVec;
      ExtensionMembers                        *const parent;
      Word                                           memMax;
   };
}

#endif//DGE__Code__ExtMem_H__

