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

#include "SDL.h"


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
      PrivData() = delete;
      PrivData(PrivData const &other) = delete;
      PrivData(int w_, int h_);

      ~PrivData();

      SDL_Window   *window;
      SDL_GLContext gl;
   };
}

#endif//DGE__GL__Window__PrivData_H__

