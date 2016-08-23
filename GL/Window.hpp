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

#ifndef Doom__GL__Window_H__
#define Doom__GL__Window_H__

#include "SDL.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace GL
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

         int w, h;
         float xl, yl, xh, yh;


         static int W, H;

      private:
         void resize(int w, int h);

         SDL_GLContext gl;
         SDL_Window   *window;
      };
   }
}

#endif//Doom__GL__Window_H__

