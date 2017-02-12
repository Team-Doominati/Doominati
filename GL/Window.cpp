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

#include "Code/Callback.hpp"
#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include "GL/OpenGL2.1.h"

#include <iostream>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::GL
{
   static Window *CurrentWindow;
   static Code::Callback CallbackResize{"Resize"};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Window constructor
   //
   Window::Window(int w_, int h_) :
      w{w_}, h{h_},
      window{},
      gl{}
   {
      int x = SDL_WINDOWPOS_UNDEFINED;
      int y = SDL_WINDOWPOS_UNDEFINED;

      // Set up window.
      if(!(window = SDL_CreateWindow("Doominati Confirmed", x, y, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
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
      int newW, newH;
      SDL_GetWindowSize(window, &newW, &newH);

      if(w != newW || h != newH)
      {
         glViewport(0, 0, w = newW, h = newH);
         CallbackResize(w, h);
      }
   }

   //
   // Window::renderEnd
   //
   void Window::renderEnd()
   {
      SDL_GL_SwapWindow(window);
   }

   //
   // Window::setTitle
   //
   void Window::setTitle(char const *title)
   {
      SDL_SetWindowTitle(window, title);
   }

   //
   // Window::GetCurrent
   //
   Window *Window::GetCurrent()
   {
      return CurrentWindow;
   }

   //
   // Window::SetCurrent
   //
   void Window::SetCurrent(Window *window)
   {
      CurrentWindow = window;
      SDL_GL_MakeCurrent(window->window, window->gl);
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // int DGE_GetScrW(void)
   //
   DGE_Code_NativeDefn(DGE_GetScrW)
   {
      task->dataStk.push(Window::GetCurrent()->w);
      return false;
   }

   //
   // int DGE_GetScrH(void)
   //
   DGE_Code_NativeDefn(DGE_GetScrH)
   {
      task->dataStk.push(Window::GetCurrent()->h);
      return false;
   }

   //
   // void DGE_SetWindowTitle(char const *str)
   //
   DGE_Code_NativeDefn(DGE_SetWindowTitle)
   {
      Window::GetCurrent()->setTitle(Code::MemStrDup({&task->prog->memory, argv[0]}).get());
      return false;
   }
}

// EOF

