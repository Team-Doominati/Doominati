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

#include "SDL.h"

#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // Window::PrivData
   //
   class Window::PrivData
   {
   public:
      using TextureHashmap = std::unordered_map<std::string, TextureData>;

      PrivData() = delete;
      PrivData(PrivData const &other) = delete;
      PrivData(int w, int h);

      ~PrivData();

      SDL_Window     *window;
      SDL_GLContext   gl;
      GLuint          textureCurrent;
      TextureHashmap  textures;
      DynamicBuffer   circleBuff, circleLineBuff;
   };
}

#endif//DGE__GL__Window__PrivData_H__

