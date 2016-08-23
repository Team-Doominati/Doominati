//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Player input abstraction.
//
//-----------------------------------------------------------------------------

#include "Game/Input.hpp"

#include "GL/Window.hpp"

#include <iostream>

#include "SDL.h"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Game
   {
      //
      // InputSink_Null::sink
      //
      void InputSink_Null::sink(InputFrame const &)
      {
      }

      //
      // InputSource_Local constructor
      //
      InputSource_Local::InputSource_Local() : frameLast{}, frameNext{}
      {
         if(SDL_InitSubSystem(SDL_INIT_EVENTS))
         {
            std::cerr << "SDL_InitSubSystem: " << SDL_GetError() << '\n';
            throw EXIT_FAILURE;
         }
      }

      //
      // InputSource_Local destructor
      //
      InputSource_Local::~InputSource_Local()
      {
         SDL_QuitSubSystem(SDL_INIT_EVENTS);
      }

      //
      // InputSource_Local::canPoll
      //
      bool InputSource_Local::canPoll() const
      {
         return true;
      }

      //
      // InputSource_Local::getLast
      //
      InputFrame const &InputSource_Local::getLast() const
      {
         return frameLast;
      }

      //
      // InputSource_Local::getNext
      //
      InputFrame const &InputSource_Local::getNext() const
      {
         return frameNext;
      }

      //
      // InputSource_Local::poll
      //
      void InputSource_Local::poll()
      {
         frameLast = frameNext;

         // Poll events.
         SDL_Event event;
         while(SDL_PollEvent(&event))
         {
            switch(event.type)
            {
            case SDL_KEYDOWN:
               switch(event.key.keysym.sym)
               {
               case SDLK_ESCAPE: frameNext.menuClose = true; break;

               default: break;
               }
               break;

            case SDL_KEYUP:
               switch(event.key.keysym.sym)
               {
               case SDLK_ESCAPE: frameNext.menuClose = false; break;

               default: break;
               }
               break;

            case SDL_MOUSEBUTTONDOWN:
               switch(event.button.button)
               {
               case SDL_BUTTON_LEFT:   frameNext.use1 = true; break;
               case SDL_BUTTON_MIDDLE: frameNext.use3 = true; break;
               case SDL_BUTTON_RIGHT:  frameNext.use2 = true; break;

               default: break;
               }
               break;

            case SDL_MOUSEBUTTONUP:
               switch(event.button.button)
               {
               case SDL_BUTTON_LEFT:   frameNext.use1 = false; break;
               case SDL_BUTTON_MIDDLE: frameNext.use3 = false; break;
               case SDL_BUTTON_RIGHT:  frameNext.use2 = false; break;

               default: break;
               }
               break;

            case SDL_MOUSEMOTION:
               frameNext.pos.x = event.motion.x - (GL::Window::W / 2);
               frameNext.pos.y = GL::Window::H - event.motion.y - (GL::Window::H / 2);
               break;

            case SDL_QUIT:
               throw EXIT_SUCCESS;

            default: break;
            }
         }
      }
   }
}

// EOF

