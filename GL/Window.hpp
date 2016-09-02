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
#include "GL/Shader.hpp"


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
         
         void setDrawColor(float r, float g, float b, float a = 1.0f);

         void shaderSwap(Shader *sp);
         void shaderDrop();

         int w, h;
         float xl, yl, xh, yh;

         static int W, H;

      private:
         void resize(int w, int h);

         SDL_GLContext gl;
         SDL_Window   *window;
         
         Shader *baseShader;
         Shader *shaderCurrent;
      };
   }
}

#endif//Doom__GL__Window_H__

