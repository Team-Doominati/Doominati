//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
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

#include "SDL.h"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   class Shader;
   class TextureData;

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

      static Window *Current;

   private:
      void resize(int w, int h);

      SDL_Window   *window;
      SDL_GLContext gl;
   };
}

#endif//DGE__GL__Window_H__

