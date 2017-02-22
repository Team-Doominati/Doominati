//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Resource management.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__ResourceManager_H__
#define DGE__Core__ResourceManager_H__

#include "Core/HashMap.hpp"
#include "Core/List.hpp"

#include <GDCC/Core/String.hpp>

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // Resource
   //
   template<typename ResourceData>
   class Resource
   {
   public:
      Resource() = delete;
      Resource(Resource &&rsrc) : data{std::move(rsrc.data)},
         link{this, std::move(rsrc.link)}, name{rsrc.name}, idx{rsrc.idx} {}
      Resource(ResourceData &&data_, GDCC::Core::String name_, std::size_t idx_) :
         data{std::move(data_)}, link{this}, name{name_}, idx{idx_} {}

      ResourceData             data;
      Core::ListLink<Resource> link;
      GDCC::Core::String       name;
      std::size_t              idx;
   };

   //
   // ResourceManager
   //
   template<typename ResourceData>
   class ResourceManager
   {
   public:
      using ResourceT   = Resource<ResourceData>;
      using ResourceMap = Core::HashMapKeyMem<GDCC::Core::String, ResourceT,
         &ResourceT::name, &ResourceT::link>;
      using ResourceVec = std::vector<ResourceT>;

      ResourceManager() : resNone{nullptr} {}
      ResourceManager(ResourceManager const &) = delete;

      //
      // add
      //
      ResourceT *add(ResourceData &&data, GDCC::Core::String name)
      {
         std::size_t idx = resVec.size();
         resVec.emplace_back(std::move(data), name, idx);
         resMap.insert(&resVec.back());

         resNone = &resVec.front();

         return &resVec.back();
      }

      //
      // get
      //
      ResourceT *get(std::size_t idx)
      {
         if(idx < resVec.size())
            return &resVec[idx];

         return resNone;
      }

      ResourceMap resMap;
      ResourceVec resVec;
      ResourceT  *resNone;
   };
}

#endif//DGE__Core__ResourceManager_H__

