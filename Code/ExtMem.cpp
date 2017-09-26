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

#include "Code/ExtMem.hpp"

#include "Code/Glyph.hpp"
#include "Code/Native.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // ExtensionMembers constructor
   //
   ExtensionMembers::ExtensionMembers(Core::HashedStr name, ExtensionMembers *parent_) :
      memVec{new std::vector<std::pair<Core::HashedStr, Word>>},
      parent{parent_},
      memMax{0}
   {
      NativeAdder::Add(name, this);
   }

   //
   // ExtensionMembers::add
   //
   void ExtensionMembers::add(Core::HashedStr name, Word size)
   {
      memVec->emplace_back(name, memMax);
      memMax += size;
   }

   //
   // ExtensionMembers::find
   //
   Word *ExtensionMembers::find(Core::HashedStr name) const
   {
      if(auto m = memMap.find(name))
         return m;

      return parent ? parent->find(name) : nullptr;
   }

   //
   // ExtensionMembers::finish
   //
   void ExtensionMembers::finish()
   {
      if(!memVec) return;
      if(parent) parent->finish();

      // Build member name storage.
      std::size_t memNameC = 0;
      for(auto &m : *memVec)
         memNameC += m.first.size();

      memNames.reset(new char[memNameC]);

      memNameC = 0;
      for(auto &m : *memVec)
      {
         std::memcpy(&memNames[memNameC], m.first.data(), m.first.size());
         memNameC += m.first.size();
      }

      // Build map.
      auto vecItr  = memVec->begin();
      auto nameItr = memNames.get();
      memMap.reset(memVec->size(), [&](auto elem)
      {
         elem->key = {nameItr, vecItr->first.size()};
         elem->val = vecItr->second + (parent ? parent->memMax : 0);
         nameItr += vecItr->first.size();
         ++vecItr;
      });

      if(parent)
         memMax += parent->memMax;

      delete memVec;
      memVec = nullptr;
   }
}

// EOF

