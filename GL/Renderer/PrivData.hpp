//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Renderer private data class.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Renderer__PrivData_H__
#define DGE__GL__Renderer__PrivData_H__

#include "GL/Renderer.hpp"

#include "GL/DynamicBuffer.hpp"
#include "GL/Texture.hpp"

#include "Core/HashMap.hpp"
#include "Core/List.hpp"

#include <GDCC/Core/String.hpp>

#include "SDL.h"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // Renderer::Texture
   //
   class Renderer::Texture
   {
   public:
      Texture() = delete;
      Texture(Texture &&tex) : data{std::move(tex.data)},
         link{this, std::move(tex.link)}, name{tex.name}, idx{tex.idx} {}
      Texture(TextureData &&data_, GDCC::Core::String name_, std::size_t idx_) :
         data{std::move(data_)}, link{this}, name{name_}, idx{idx_} {}

      TextureData             data;
      Core::ListLink<Texture> link;
      GDCC::Core::String      name;
      std::size_t             idx;
   };

   //
   // Renderer::PrivData
   //
   class Renderer::PrivData
   {
   public:
      using TextureMap = Core::HashMapKeyMem<GDCC::Core::String, Texture,
         &Texture::name, &Texture::link>;
      using TextureVec = std::vector<Texture>;

      PrivData() : texBound{nullptr} {}
      PrivData(PrivData const &other) = delete;

      Texture *texAdd(GLsizei texw, GLsizei texh, TexturePixel const *data,
         GDCC::Core::String name);

      TextureData const *texBound;
      TextureMap         texMap;
      TextureVec         texVec;
      Texture           *texNone;

      DynamicBuffer const *circleBuff, *circleLineBuff;

      std::unordered_map<int, DynamicBuffer> circleBuffers, circleLineBuffers;
   };
}

#endif//DGE__GL__Renderer__PrivData_H__

