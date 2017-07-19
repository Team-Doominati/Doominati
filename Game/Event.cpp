//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
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

#define PushEvent() \
   EventStack.push_back(pevent); \
   GameEventStack.push_back(pevent)


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Game
{
   static std::vector<Event> EventStack;
   static std::vector<Event> GameEventStack;

   static unsigned ResRH, ResRW, ResVH, ResVW;
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
   // PumpEvents
   //
   void PumpEvents()
   {
      EventStack.clear();

      // Poll events.
      for(SDL_Event event; SDL_PollEvent(&event);)
      {
         switch(event.type)
         {
         case SDL_KEYDOWN: case SDL_KEYUP:
         if(!event.key.repeat)
         {
            bool  down = event.type == SDL_KEYDOWN;
            Event pevent{down ? Event::KeyDown : Event::KeyUp};

            pevent.data.key = char32_t(event.key.keysym.sym);

            PushEvent();
         }
            break;
         case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
         {
            bool  down = (event.type == SDL_MOUSEBUTTONDOWN);
            Event pevent{down ? Event::MouseDown : Event::MouseUp};

            pevent.data.mb = MouseButton(event.button.button);

            PushEvent();
         }
            break;
         case SDL_MOUSEWHEEL:
         {
            Event pevent{Event::MouseWheel};
            pevent.data.axis.x = event.wheel.x;
            pevent.data.axis.y = event.wheel.y;

            if(event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            {
               pevent.data.axis.x = -pevent.data.axis.x;
               pevent.data.axis.y = -pevent.data.axis.y;
            }

            PushEvent();
         }
            break;
         case SDL_MOUSEMOTION:
         {
            Event pevent{Event::MouseMove};

            pevent.data.axis.x = event.motion.x * ResVW / ResRW;
            pevent.data.axis.y = event.motion.y * ResVH / ResRH;

            PushEvent();
         }
            break;
         case SDL_QUIT:
            throw EXIT_SUCCESS;
         }
      }
   }

   //
   // ProcessEvents
   //
   void ProcessEvents(EventSink &sink)
   {
      for(auto const &evt : EventStack)
         sink.sink(evt);
   }

   //
   // ProcessGameEvents
   //
   void ProcessGameEvents(EventSink &sink)
   {
      for(auto const &evt : GameEventStack)
         sink.sink(evt);
   }

   //
   // SetResolutionReal
   //
   void SetResolutionReal(unsigned w, unsigned h)
   {
      ResRH = h;
      ResRW = w;
   }

   //
   // SetResolutionVirt
   //
   void SetResolutionVirt(unsigned w, unsigned h)
   {
      ResVH = h;
      ResVW = w;
   }
}

// EOF

