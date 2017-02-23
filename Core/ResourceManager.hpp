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
      using Res    = Resource<ResourceData>;
      using ResMan = Core::HashMapKeyMem<GDCC::Core::String, Res,
         &Res::name, &Res::link>;
      using ResVec = std::vector<Res>;


      ResourceManager() : resMap{}, resVec{}, resNone{nullptr} {}
      ResourceManager(ResourceManager const &) = delete;

      //
      // add
      //
      Res *add(ResourceData &&data, GDCC::Core::String name)
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
      Res *get(std::size_t idx)
      {
         if(idx < resVec.size())
            return &resVec[idx];

         return resNone;
      }

      ResMan resMap;
      ResVec resVec;
      Res   *resNone;
   };

   //
   // ResourceSaver
   //
   template<typename ResourceData>
   class ResourceSaver
   {
   public:
      using Res    = Resource<ResourceData>;
      using ResMan = ResourceManager<ResourceData>;


      ResourceSaver(ResMan &man_, Res *&res_) :
         man{man_}, res{res_}, idx{res ? res->idx : SIZE_MAX} {}
      ResourceSaver(ResMan &man_, Res const *&res_) :
         man{man_}, res{const_cast<Res *&>(res_)}, idx{res ? res->idx : SIZE_MAX} {}
      ~ResourceSaver() {if(idx != SIZE_MAX) res = man.get(idx);}

   private:
      ResMan     &man;
      Res       *&res;
      std::size_t idx;
   };
}

#endif//DGE__Core__ResourceManager_H__

