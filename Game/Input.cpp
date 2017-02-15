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

#include "Code/Program.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include <GDCC/Core/Option.hpp>

#include <GDCC/Option/Bool.hpp>

#include "SDL.h"

#include <iostream>
#include <climits>
#include <tuple>


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
// Static Objects                                                             |
//

namespace DGE::Game
{
   static InputSource *CurrentInput;
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
      frameNext = InputFrame{};

      // Process events into actions.
      ProcessGameEvents(*this);

      // After that, we can process those into an input frame.
      auto getAxis = [this](int actionstart)
      {
         std::int16_t axf = 0, axr = 0;
         if(actions[actionstart + A_AxF]) axf += INT16_MAX;
         if(actions[actionstart + A_AxB]) axf -= INT16_MAX;
         if(actions[actionstart + A_AxR]) axr += INT16_MAX;
         if(actions[actionstart + A_AxL]) axr -= INT16_MAX;
         return std::pair<std::int16_t, std::int16_t>{axf, axr};
      };

      std::tie(frameNext.ax1y, frameNext.ax1x) = getAxis(A_Ax1);
      std::tie(frameNext.ax2y, frameNext.ax2x) = getAxis(A_Ax2);
      std::tie(frameNext.ax3y, frameNext.ax3x) = getAxis(A_Ax3);

      for(int i = 0; i < 4; i++)
         frameNext.buttons |= actions[A_Buttons + i] << i;
   }

   //
   // InputSource_Local::sink
   //
   void InputSource_Local::sink(Event const &event)
   {
      switch(event.type)
      {
      case Event::KeyDown:
         switch(event.data.key)
         {
         case 'w': actions[A_Ax1F] = true; break;
         case 'a': actions[A_Ax1L] = true; break;
         case 's': actions[A_Ax1B] = true; break;
         case 'd': actions[A_Ax1R] = true; break;
         }
         break;
      case Event::KeyUp:
         switch(event.data.key)
         {
         case 'w': actions[A_Ax1F] = false; break;
         case 'a': actions[A_Ax1L] = false; break;
         case 's': actions[A_Ax1B] = false; break;
         case 'd': actions[A_Ax1R] = false; break;
         case KC_Escape: throw EXIT_SUCCESS;
         }
         break;
      case Event::MouseDown:
         switch(event.data.mb)
         {
         case MB_Left:  actions[A_Button1] = true; break;
         case MB_Right: actions[A_Button2] = true; break;
         }
         break;
      case Event::MouseUp:
         switch(event.data.mb)
         {
         case MB_Left:  actions[A_Button1] = false; break;
         case MB_Right: actions[A_Button2] = false; break;
         }
         break;
      }

      if(DebugInput)
      {
         auto printMouseButton = [](Event const &ev)
         {
            switch(ev.data.mb)
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

         auto printAxis = [](Event const &ev)
         {
            std::cout << "x: " << ev.data.axis.x <<
                       "\ty: " << ev.data.axis.y << std::endl;
         };

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

   //
   // InputSource::GetCurrent
   //
   InputSource *InputSource::GetCurrent()
   {
      return CurrentInput;
   }

   //
   // InputSource::SetCurrent
   //
   void InputSource::SetCurrent(InputSource *input)
   {
      CurrentInput = input;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // _Fract DGE_GetInputAxis(unsigned num, unsigned axis)
   //
   DGE_Code_NativeDefn(DGE_GetInputAxis)
   {
      auto const &frame = InputSource::GetCurrent()->getNext();
      int ret;

      // TODO: change to switch-init statement when we switch to VS2017
      bool y = argv[1];
      switch(argv[0])
      {
      default:
      case 1: ret = y ? frame.ax1y : frame.ax1x; break;
      case 2: ret = y ? frame.ax2y : frame.ax2x; break;
      case 3: ret = y ? frame.ax3y : frame.ax3x; break;
      }

      task->dataStk.push(ret);
      return false;
   }

   //
   // unsigned DGE_GetInputButtons(void)
   //
   DGE_Code_NativeDefn(DGE_GetInputButtons)
   {
      task->dataStk.push(InputSource::GetCurrent()->getNext().buttons);
      return false;
   }
}

// EOF

