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
      static KeyCode SDLKey(int sym);
      static KeyCode SDLButton(int button);
      
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
      // InputSource_Local::addListener_Button
      //
         
      void InputSource_Local::addListener_Button(char const *name, KeyCode bind)
      {
         frameNext.input_btn[name] = InputKind_Button{bind, false};
         frameLast.input_btn[name] = InputKind_Button{bind, false};
      }
      
      //
      // InputSource_Local::addListener_Axis
      //
      
      void InputSource_Local::addListener_Axis(char const *name, InputKind_Axis::AxisType type)
      {
         frameNext.input_axs[name] = InputKind_Axis{type, KC_NULL, KC_NULL, 0.0, 0.0};
         frameLast.input_axs[name] = InputKind_Axis{type, KC_NULL, KC_NULL, 0.0, 0.0};
      }
      
      //
      // InputSource_Local::addListener_ButtonAxis
      //
      
      void InputSource_Local::addListener_ButtonAxis(char const *name, KeyCode bind, KeyCode bind2)
      {
         frameNext.input_axs[name] = InputKind_Axis{InputKind_Axis::Buttons, bind, bind2, 0.0, 0.0};
         frameLast.input_axs[name] = InputKind_Axis{InputKind_Axis::Buttons, bind, bind2, 0.0, 0.0};
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
            InputKind_Axis::AxisType axistype = InputKind_Axis::None;
            double deltax, deltay;
            
            KeyCode key = KC_NULL;
            bool down;

            switch(event.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
               key  = SDLKey(event.key.keysym.sym);
               down = (event.type == SDL_KEYDOWN);
               break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
               key  = SDLButton(event.button.button);
               down = (event.type == SDL_MOUSEBUTTONDOWN);
               break;
            case SDL_MOUSEMOTION:
               axistype = InputKind_Axis::Mouse;
               deltax   = event.motion.xrel;
               deltay   = event.motion.yrel;
               break;
            case SDL_MOUSEWHEEL:
               axistype = InputKind_Axis::MouseWheel;
               deltax   = event.wheel.x;
               deltay   = event.wheel.y;
               break;
            case SDL_QUIT:
               throw EXIT_SUCCESS;
            }
            
            if(key)
            {
               for(auto &input : frameNext.input_btn)
                  if(input.second.bind == key)
                     input.second.button = down;
            }
            else if(axistype != InputKind_Axis::None)
            {
               for(auto &input : frameNext.input_axs)
                  if(input.second.type == axistype)
                  {
                     input.second.deltax = deltax;
                     input.second.deltay = deltay;
                  }
            }
         }
      }

      //
      // SDLKey
      //
      
      static
      KeyCode SDLKey(int sym)
      {
         KeyCode rc = KC_NULL;

         switch(sym)
         {
         case SDLK_LEFT:         rc = KC_LEFTARROW;  break;
         case SDLK_RIGHT:        rc = KC_RIGHTARROW; break;
         case SDLK_DOWN:         rc = KC_DOWNARROW;  break;
         case SDLK_UP:           rc = KC_UPARROW;    break;
         case SDLK_ESCAPE:       rc = KC_ESCAPE;     break;
         case SDLK_RETURN:       rc = KC_ENTER;      break;
         case SDLK_TAB:          rc = KC_TAB;        break;
         case SDLK_F1:           rc = KC_F1;         break;
         case SDLK_F2:           rc = KC_F2;         break;
         case SDLK_F3:           rc = KC_F3;         break;
         case SDLK_F4:           rc = KC_F4;         break;
         case SDLK_F5:           rc = KC_F5;         break;
         case SDLK_F6:           rc = KC_F6;         break;
         case SDLK_F7:           rc = KC_F7;         break;
         case SDLK_F8:           rc = KC_F8;         break;
         case SDLK_F9:           rc = KC_F9;         break;
         case SDLK_F10:          rc = KC_F10;        break;
         case SDLK_F11:          rc = KC_F11;        break;
         case SDLK_F12:          rc = KC_F12;        break;
         case SDLK_BACKSPACE:    rc = KC_BACKSPACE;  break;
         case SDLK_PAUSE:        rc = KC_PAUSE;      break;
         case SDLK_EQUALS:       rc = KC_EQUALS;     break;
         case SDLK_MINUS:        rc = KC_MINUS;      break;

         case SDLK_KP_0:         rc = KC_KP0;        break;
         case SDLK_KP_1:         rc = KC_KP1;        break;
         case SDLK_KP_2:         rc = KC_KP2;        break;
         case SDLK_KP_3:         rc = KC_KP3;        break;
         case SDLK_KP_4:         rc = KC_KP4;        break;
         case SDLK_KP_5:         rc = KC_KP5;        break;
         case SDLK_KP_6:         rc = KC_KP6;        break;
         case SDLK_KP_7:         rc = KC_KP7;        break;
         case SDLK_KP_8:         rc = KC_KP8;        break;
         case SDLK_KP_9:         rc = KC_KP9;        break;
         case SDLK_KP_PERIOD:    rc = KC_KPPERIOD;   break;
         case SDLK_KP_DIVIDE:    rc = KC_KPDIVIDE;   break;
         case SDLK_KP_MULTIPLY:  rc = KC_KPMULTIPLY; break;
         case SDLK_KP_MINUS:     rc = KC_KPMINUS;    break;
         case SDLK_KP_PLUS:      rc = KC_KPPLUS;     break;
         case SDLK_KP_ENTER:     rc = KC_KPENTER;    break;
         case SDLK_KP_EQUALS:    rc = KC_KPEQUALS;   break;

         case SDLK_NUMLOCKCLEAR: rc = KC_NUMLOCK;    break;
         case SDLK_SCROLLLOCK:   rc = KC_SCROLLLOCK; break;
         case SDLK_CAPSLOCK:     rc = KC_CAPSLOCK;   break;
         case SDLK_LSHIFT:
         case SDLK_RSHIFT:       rc = KC_SHIFT;      break;
         case SDLK_LCTRL:
         case SDLK_RCTRL:        rc = KC_CTRL;       break;

         case SDLK_LALT:
         case SDLK_RALT:
         case SDLK_LGUI:
         case SDLK_RGUI:         rc = KC_ALT;        break;
         case SDLK_PAGEUP:       rc = KC_PAGEUP;     break;
         case SDLK_PAGEDOWN:     rc = KC_PAGEDOWN;   break;
         case SDLK_HOME:         rc = KC_HOME;       break;
         case SDLK_END:          rc = KC_END;        break;
         case SDLK_INSERT:       rc = KC_INSERT;     break;
         case SDLK_DELETE:       rc = KC_DEL;        break;
         
         case SDLK_SYSREQ:
         case SDLK_PRINTSCREEN:  rc = KC_SYSRQ;      break;
         
         default:
            rc = static_cast<KeyCode>(sym);
            break;
         }
         
         return rc;
      }
      
      //
      // SDLButton
      //

      static
      KeyCode SDLButton(int button)
      {
         KeyCode rc = KC_NULL;
         
         switch(button)
         {
         case SDL_BUTTON_LEFT:   rc = KC_MOUSE1; break;
         case SDL_BUTTON_MIDDLE: rc = KC_MOUSE3; break;
         case SDL_BUTTON_RIGHT:  rc = KC_MOUSE2; break;
         case SDL_BUTTON_X1:     rc = KC_MOUSE4; break;
         case SDL_BUTTON_X2:     rc = KC_MOUSE5; break;
         }
         
         return rc;
      }
   }
}

// EOF

