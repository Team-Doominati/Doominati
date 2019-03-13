//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// System events.
//
//-----------------------------------------------------------------------------

#include "Game/Event.hpp"

#include "GL/Window.hpp"

#include "SDL.h"

#include <vector>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Game
{
   static std::vector<Event> EventStack;
   static std::vector<Event> GameEventStack;

   static int ResRH, ResRW, ResVH, ResVW;
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Game
{
   //
   // PushEvent
   //
   static inline void PushEvent(Event const &evt)
   {
      EventStack.emplace_back(evt);
      GameEventStack.emplace_back(evt);
   }

   //
   // EventAxis
   //
   static inline void EventAxis(SDL_Event const &sevt)
   {
      Event evt{Event::GamepadMove};

      evt.data.axis.num = GamepadAxis(sevt.caxis.axis);
      evt.data.axis.val = sevt.caxis.value;

      PushEvent(evt);
   }

   //
   // EventButton
   //
   static inline void EventButton(SDL_Event const &sevt)
   {
      Event evt{sevt.cbutton.state == SDL_PRESSED ?
         Event::GamepadDown : Event::GamepadUp};

      evt.data.gb = GamepadButton(sevt.cbutton.button);

      PushEvent(evt);
   }

   //
   // EventKey
   //
   static inline void EventKey(SDL_Event const &sevt)
   {
      if(!sevt.key.repeat)
      {
         Event evt{sevt.key.state == SDL_PRESSED ?
            Event::KeyDown : Event::KeyUp};

         evt.data.key = char32_t(sevt.key.keysym.sym);

         PushEvent(evt);
      }
   }

   //
   // EventMotion
   //
   static inline void EventMotion(SDL_Event const &sevt)
   {
      Event evt{Event::MouseMove};

      evt.data.mouse = Core::Vector2I{sevt.motion.x * ResVW / ResRW,
                                      sevt.motion.y * ResVH / ResRH};

      PushEvent(evt);
   }

   //
   // EventMouse
   //
   static inline void EventMouse(SDL_Event const &sevt)
   {
      Event evt{sevt.button.state == SDL_PRESSED ?
         Event::MouseDown : Event::MouseUp};

      switch(sevt.button.button)
      {
      case SDL_BUTTON_LEFT:   evt.data.mb = MouseButton::Left;   break;
      case SDL_BUTTON_MIDDLE: evt.data.mb = MouseButton::Middle; break;
      case SDL_BUTTON_RIGHT:  evt.data.mb = MouseButton::Right;  break;
      case SDL_BUTTON_X1:     evt.data.mb = MouseButton::Extra1; break;
      case SDL_BUTTON_X2:     evt.data.mb = MouseButton::Extra2; break;
      default: return;
      }

      PushEvent(evt);
   }

   //
   // EventWheel
   //
   static inline void EventWheel(SDL_Event const &sevt)
   {
      Event evt{Event::MouseWheel};

      evt.data.mouse = Core::Vector2I{sevt.wheel.x, sevt.wheel.y};

      if(sevt.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
         evt.data.mouse = -evt.data.mouse;

      PushEvent(evt);
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // EventFrame
   //
   void EventFrame()
   {
      GameEventStack.clear();
   }

   //
   // ProcessEvents
   //
   void ProcessEvents(EventSink &sink)
   {
      for(auto const &evt : EventStack) sink.sink(evt);
   }

   //
   // ProcessGameEvents
   //
   void ProcessGameEvents(EventSink &sink)
   {
      for(auto const &evt : GameEventStack) sink.sink(evt);
   }

   //
   // PumpEvents
   //
   void PumpEvents()
   {
      EventStack.clear();

      // Poll events.
      for(SDL_Event sevt; SDL_PollEvent(&sevt);)
      {
         switch(sevt.type)
         {
         case SDL_CONTROLLERAXISMOTION:
            EventAxis(sevt);
            break;

         case SDL_CONTROLLERBUTTONDOWN:
         case SDL_CONTROLLERBUTTONUP:
            EventButton(sevt);
            break;

         case SDL_KEYDOWN:
         case SDL_KEYUP:
            EventKey(sevt);
            break;

         case SDL_MOUSEBUTTONDOWN:
         case SDL_MOUSEBUTTONUP:
            EventMouse(sevt);
            break;

         case SDL_MOUSEMOTION:
            EventMotion(sevt);
            break;

         case SDL_MOUSEWHEEL:
            EventWheel(sevt);
            break;

         case SDL_QUIT:
            throw EXIT_SUCCESS;
         }
      }
   }

   //
   // SetResolutionReal
   //
   void SetResolutionReal(int w, int h)
   {
      ResRH = h;
      ResRW = w;
   }

   //
   // SetResolutionVirt
   //
   void SetResolutionVirt(int w, int h)
   {
      ResVH = h;
      ResVW = w;
   }
}

// EOF

