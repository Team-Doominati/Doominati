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
#include <climits>

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

      InputSource_Local::InputSource_Local() :
         frameLast{},
         frameNext{}
      {
      }

      //
      // InputSource_Local destructor
      //

      InputSource_Local::~InputSource_Local()
      {
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

         frameNext = InputFrame{};

         // Poll events.
         SDL_Event event;
         while(SDL_PollEvent(&event))
         {
            if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
               bool down = (event.type == SDL_KEYDOWN);
               std::int16_t delta = down ? INT16_MAX : 0;

               switch(event.key.keysym.sym)
               {
               case SDLK_w: frameNext.movefwd  =  delta; break;
               case SDLK_s: frameNext.movefwd  = -delta; break;
               case SDLK_d: frameNext.moveside =  delta; break;
               case SDLK_a: frameNext.moveside = -delta; break;
               case SDLK_e: frameNext.buttons.use = down; break;
               case SDLK_ESCAPE: throw EXIT_SUCCESS;
               }
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN ||
                    event.type == SDL_MOUSEBUTTONUP)
            {
               bool down = (event.type == SDL_MOUSEBUTTONDOWN);

               switch(event.button.button)
               {
               case SDL_BUTTON_LEFT:
                  frameNext.buttons.attack = down;
                  break;
               case SDL_BUTTON_RIGHT:
                  frameNext.buttons.altattack = down;
                  break;
               }
            }
            else if(event.type == SDL_MOUSEMOTION)
            {
               // TODO: this should create a real fixed point angle
               frameNext.deltayaw   = event.motion.xrel;
               frameNext.deltapitch = event.motion.yrel;
            }
            else if(event.type == SDL_CONTROLLERAXISMOTION)
            {
               if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
                  frameNext.moveside = event.caxis.value;
               else if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
                  frameNext.movefwd = -event.caxis.value;
               // TODO: this should create a real fixed point angle
               else if(event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
                  frameNext.deltayaw = event.caxis.value;
               else if(event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
                  frameNext.deltapitch = event.caxis.value;
            }
            else if(event.type == SDL_QUIT)
               throw EXIT_SUCCESS;
         }
      }
   }
}

// EOF

