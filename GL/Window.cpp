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
   
   varying vec4 color;
   
   void main(void)
   {
      gl_FragColor = color;
   }
)";

static char const baseVertShader[] = R"(
   #version 120
   
   varying vec4 color;
   
   void main(void)
   {
      gl_Position = ftransform();
      color = gl_Color;
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
         
         shaderBase = new Shader{baseFragShader, baseVertShader};
         shaderDrop();
         drawColorSet(1.0, 1.0, 1.0);
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
      // Window::drawColorSet
      //
      
      void Window::drawColorSet(float r, float g, float b, float a)
      {
         glColor4f(r, g, b, a);
         cr = r;
         cg = g;
         cb = b;
         ca = a;
      }
      
      //
      // Window::drawColorSet
      //
      
      void Window::drawColorSet(Color const &col)
      {
         drawColorSet(float(col.r), float(col.g), float(col.b), float(col.a));
      }
      
      //
      // Window::drawColorGet
      //
      
      Color Window::drawColorGet()
      {
         return { cr, cg, cb, ca };
      }

      //
      // Window::drawLine
      //

      void Window::drawLine(int x1, int y1, int x2, int y2)
      {
         glBegin(GL_LINES);

         glVertex2f(x1, y1);
         glVertex2f(x2, y2);

         glEnd();
      }

      //
      // Window::drawRectangle
      //

      void Window::drawRectangle(int x1, int y1, int x2, int y2, bool line)
      {
         if(line)
         {
            glBegin(GL_LINE_LOOP);

            // A--B
            // |  |
            // D--C
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);

            glEnd();
         }
         else
         {
            glBegin(GL_TRIANGLES);

            // B--A
            // | /
            // C
            glVertex2f(x2, y1);
            glVertex2f(x1, y1);
            glVertex2f(x1, y2);

            //    A
            //  / |
            // B--C
            glVertex2f(x2, y1);
            glVertex2f(x1, y2);
            glVertex2f(x2, y2);

            glEnd();
         }
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
         shaderSwap(shaderBase);
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

