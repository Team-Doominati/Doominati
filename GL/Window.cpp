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

#include "GL/Window/PrivData.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::GL
{
   Window *Window::Current = nullptr;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Window::PrivData constructor
   //
   Window::PrivData::PrivData(int w_, int h_) :
      window{},
      gl{}
   {
      int x = SDL_WINDOWPOS_UNDEFINED;
      int y = SDL_WINDOWPOS_UNDEFINED;

      // Set up window.
      if(!(window = SDL_CreateWindow("Doominati", x, y, w_, h_, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
      {
         SDL_QuitSubSystem(SDL_INIT_VIDEO);
         std::cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
         throw EXIT_FAILURE;
      }

      // Set up OpenGL context.
      if(!(gl = SDL_GL_CreateContext(window)))
      {
         SDL_DestroyWindow(window);
         SDL_QuitSubSystem(SDL_INIT_VIDEO);
         std::cerr << "SDL_GL_CreateContext: " << SDL_GetError() << '\n';
         throw EXIT_FAILURE;
      }
   }

   //
   // Window::PrivData destructor
   //
   Window::PrivData::~PrivData()
   {
      SDL_GL_DeleteContext(gl);
      SDL_DestroyWindow(window);
   }

   //
   // Window constructor
   //
   Window::Window(int w_, int h_) :
      w{w_}, h{h_},
      privdata{new PrivData(w_, h_)}
   {
   }

   //
   // Window destructor
   //
   // Needs to exist here specifically, because of the way
   // unique_ptr works with incomplete types.
   //
   Window::~Window()
   {
   }

   //
   // Window::renderBegin
   //
   void Window::renderBegin()
   {
      // Check if window has been resized.
      {
         int newW, newH;
         SDL_GetWindowSize(privdata->window, &newW, &newH);

         if(w != newW || h != newH)
         {
            w = newW;
            h = newH;
         }
      }
   }

   //
   // Window::renderEnd
   //
   void Window::renderEnd()
   {
      SDL_GL_SwapWindow(privdata->window);
   }
}

// EOF

