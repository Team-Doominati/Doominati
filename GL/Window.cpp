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

#include "GL/Window.hpp"
#include "GL/Shader.hpp"

#include <iostream>

#include "GL/gl_2_1.h"


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static char const baseFragShader[] = R"(
   #version 120
   
   uniform vec4 color;
   
   void main(void)
   {
      gl_FragColor = color;
   }
)";

// TODO: Reimplement vertex shader with custom pipeline.
static char const baseVertShader[] = R"(
   #version 120
   
   void main(void)
   {
      gl_Position = ftransform();
   }
)";


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace Doom
{
   namespace GL
   {
      int Window::W = 0;
      int Window::H = 0;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // Window constructor
      //

      Window::Window(int w_, int h_) :
         w{w_}, h{h_}
      {
         int x = SDL_WINDOWPOS_UNDEFINED;
         int y = SDL_WINDOWPOS_UNDEFINED;

         if(!(window = SDL_CreateWindow("Doominati", x, y, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
         {
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            std::cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
            throw EXIT_FAILURE;
         }

         if(!(gl = SDL_GL_CreateContext(window)))
         {
            SDL_DestroyWindow(window);
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            std::cerr << "SDL_GL_CreateContext: " << SDL_GetError() << '\n';
            throw EXIT_FAILURE;
         }

         glClearColor(0, 0, 0, 1);

         glEnable(GL_TEXTURE_2D);
         glEnable(GL_BLEND);

         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         glEnableClientState(GL_COLOR_ARRAY);
         glEnableClientState(GL_TEXTURE_COORD_ARRAY);
         glEnableClientState(GL_VERTEX_ARRAY);

         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

         resize(w, h);
         
         baseShader = new Shader{baseFragShader, baseVertShader};
         shaderDrop();
         setDrawColor(1.0, 1.0, 1.0);
      }

      //
      // Window destructor
      //

      Window::~Window()
      {
         SDL_GL_DeleteContext(gl);
         SDL_DestroyWindow(window);
      }

      //
      // Window::renderBegin
      //

      void Window::renderBegin()
      {
         // Check if window has been resized.
         {
            int newW, newH;
            SDL_GetWindowSize(window, &newW, &newH);

            if(w != newW || h != newH)
               resize(newW, newH);
         }

         W = w;
         H = h;

         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }

      //
      // Window::renderEnd
      //

      void Window::renderEnd()
      {
         SDL_GL_SwapWindow(window);
      }
      
      //
      // Window::setDrawColor
      //
      
      void Window::setDrawColor(float r, float g, float b, float a)
      {
         GLint location = glGetUniformLocation(shaderCurrent->program, "color");
         glUniform4f(location, r, g, b, a);
      }

      //
      // Window::shaderSwap
      //

      void Window::shaderSwap(Shader *sp)
      {
         shaderCurrent = sp;
         glUseProgram(shaderCurrent->program);
      }

      //
      // Window::shaderDrop
      //

      void Window::shaderDrop()
      {
         shaderSwap(baseShader);
      }

      //
      // Window::resize
      //

      void Window::resize(int w_, int h_)
      {
         w = w_;
         h = h_;

         xl = -(w / 2.0f);
         yl = -(h / 2.0f);
         xh = +(w / 2.0f);
         yh = +(h / 2.0f);

         if(w & 1) --xh;
         if(h & 1) --yh;

         glViewport(0,0, w,h);

         // Set up GL_PROJECTION matrix for 2D.
         glMatrixMode(GL_PROJECTION);
         glLoadIdentity();
         glOrtho(xl, xh, yl, yh, 0, 1);
         glMatrixMode(GL_MODELVIEW);
      }
   }
}

// EOF

