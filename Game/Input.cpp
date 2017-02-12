//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Player input abstraction.
//
//-----------------------------------------------------------------------------

#include "Game/Input.hpp"

#include <iostream>
#include <climits>

#include <GDCC/Core/Option.hpp>

#include <GDCC/Option/Bool.hpp>

#include "SDL.h"


//----------------------------------------------------------------------------|
// Options                                                                    |
//

namespace DGE::Game
{
   //
   // --input-debug
   //
   static bool DebugInput = false;
   static GDCC::Option::Bool DebugInputOpt{
      &GDCC::Core::GetOptionList(),
      GDCC::Option::Base::Info()
         .setName("input-debug")
         .setDescS("Enables input debugging output."),
      &DebugInput
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // InputSource_Local::poll
   //
   void InputSource_Local::poll()
   {
      frameLast = frameNext;
      ProcessGameEvents(*this);
   }

   //
   // InputSource_Local
   //
   void InputSource_Local::sink(Event const &event)
   {
      if(DebugInput)
      {
         auto printMouseButton = [](Event const &event)
         {
            switch(event.data.mb)
            {
            default:        std::cout << "<unknown button>"; break;
            case MB_Left:   std::cout << "Left Button";      break;
            case MB_Right:  std::cout << "Right Button";     break;
            case MB_Middle: std::cout << "Middle Button";    break;
            case MB_Extra1: std::cout << "Extra Button 1";   break;
            case MB_Extra2: std::cout << "Extra Button 2";   break;
            }

            std::cout << std::endl;
         };

         auto printAxis = [](Event const &event)
            {std::cout << "x: " << event.data.axis.x << "\ty: " << event.data.axis.y << std::endl;};

         switch(event.type)
         {
         case Event::KeyDown:    std::cout << "KeyDown:    " << char32_t(event.data.key) << std::endl; break;
         case Event::KeyUp:      std::cout << "KeyUp:      " << char32_t(event.data.key) << std::endl; break;
         case Event::MouseDown:  std::cout << "MouseDown:  "; printMouseButton(event); break;
         case Event::MouseUp:    std::cout << "MouseUp:    "; printMouseButton(event); break;
         case Event::MouseMove:  std::cout << "MouseMove:  "; printAxis(event); break;
         case Event::MouseWheel: std::cout << "MouseWheel: "; printAxis(event); break;
         }
      }
   }
}

// EOF

