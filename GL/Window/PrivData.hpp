//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Window private data class.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Window__PrivData_H__
#define DGE__GL__Window__PrivData_H__

#include "GL/Window.hpp"

#include "GL/DynamicBuffer.hpp"
#include "GL/Texture.hpp"

#include "Core/HashMap.hpp"

#include <GDCC/Core/String.hpp>

#include "SDL.h"

#include <unordered_map>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // Window::Texture
   //
   class Window::Texture
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
   // Window::PrivData
   //
   class Window::PrivData
   {
   public:

      using TextureMap = Core::HashMapKeyMem<GDCC::Core::String, Texture,
         &Texture::name, &Texture::link>;
      using TextureVec = std::vector<Texture>;

      PrivData() = delete;
      PrivData(PrivData const &other) = delete;
      PrivData(int w, int h);

      ~PrivData();

      Texture *texAdd(GLsizei w, GLsizei h, TexturePixel const *data,
         GDCC::Core::String name);

      SDL_Window   *window;
      SDL_GLContext gl;
      GLuint        texBound;
      TextureMap    texMap;
      TextureVec    texVec;
      DynamicBuffer circleBuff, circleLineBuff;
      Texture      *texNone;
   };
}

#endif//DGE__GL__Window__PrivData_H__

