//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Graphics Library environment.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Window_H__
#define DGE__GL__Window_H__

#include "GL/Types.hpp"

#include "SDL.h"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // Window
   //
   class Window
   {
   public:
      Window(int w, int h);
      ~Window();

      void renderBegin();
      void renderEnd();
      void setTitle(char const *title);

      int w, h;


      static Window *GetCurrent();
      static void SetCurrent(Window *window);

   private:
      void resize(int w, int h);

      SDL_Window   *window;
      SDL_GLContext gl;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   Defs::GamedefsGroup &GetWindowDefs();
}

#endif//DGE__GL__Window_H__

