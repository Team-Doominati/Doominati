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

#include "GL/Window.hpp"

#include "GL/Defs.hpp"
#include "GL/OpenGL2.1.h"

#include "Code/Callback.hpp"
#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include "Defs/Gamedefs.hpp"

#include "Game/Event.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::GL
{
   static Window *CurrentWindow;
   static Code::CallbackSet<void(Code::Word, Code::Word)> CallbackResize{"Resize"};
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
      Game::SetResolutionReal(w, h);

      int x = SDL_WINDOWPOS_UNDEFINED;
      int y = SDL_WINDOWPOS_UNDEFINED;

      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

      // Set up window.
      if(!(window = SDL_CreateWindow("Doominati Confirmed", x, y, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
      {
         std::cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
         throw EXIT_FAILURE;
      }

      // Set up OpenGL context.
      if(!(gl = SDL_GL_CreateContext(window)))
      {
         SDL_DestroyWindow(window);
         std::cerr << "SDL_GL_CreateContext: " << SDL_GetError() << '\n';
         throw EXIT_FAILURE;
      }

      SDL_GL_SetSwapInterval(0);
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
         Game::SetResolutionReal(w, h);
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

   //
   // GetWindowDefs
   //
   Defs::GamedefsGroup &GetWindowDefs()
   {
      static Defs::GamedefsGroup defs{&GetDefs(), "Window"};
      return defs;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // int DGE_Window_GetWidth(void)
   //
   DGE_Code_NativeDefn(DGE_Window_GetWidth)
   {
      task->dataStk.push(Window::GetCurrent()->w);
      return false;
   }

   //
   // int DGE_Window_GetHeight(void)
   //
   DGE_Code_NativeDefn(DGE_Window_GetHeight)
   {
      task->dataStk.push(Window::GetCurrent()->h);
      return false;
   }

   //
   // void DGE_Window_SetTitle(char const *str)
   //
   DGE_Code_NativeDefn(DGE_Window_SetTitle)
   {
      Window::GetCurrent()->setTitle(Code::MemStrDup({&task->prog->memory, argv[0]}).get());
      return false;
   }
}

// EOF

