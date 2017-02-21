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

#include "Code/Convert.hpp"
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

      std::tie(frameNext.ax1y, frameNext.ax1x) = getAxis(A_Ax1S);
      std::tie(frameNext.ax2y, frameNext.ax2x) = getAxis(A_Ax2S);

      for(int i = 0; i < 4; i++)
         frameNext.buttons |= actions[A_ButtonS + i] << i;
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
         case 'w': actions[A_Ax1S + A_AxF] = true; break;
         case 'a': actions[A_Ax1S + A_AxL] = true; break;
         case 's': actions[A_Ax1S + A_AxB] = true; break;
         case 'd': actions[A_Ax1S + A_AxR] = true; break;
         }
         break;
      case Event::KeyUp:
         switch(event.data.key)
         {
         case 'w': actions[A_Ax1S + A_AxF] = false; break;
         case 'a': actions[A_Ax1S + A_AxL] = false; break;
         case 's': actions[A_Ax1S + A_AxB] = false; break;
         case 'd': actions[A_Ax1S + A_AxR] = false; break;
         case KC_Escape: throw EXIT_SUCCESS;
         }
         break;
      case Event::MouseDown:
         switch(event.data.mb)
         {
         case MB_Left:  actions[A_ButtonS + 0] = true; break;
         case MB_Right: actions[A_ButtonS + 1] = true; break;
         }
         break;
      case Event::MouseUp:
         switch(event.data.mb)
         {
         case MB_Left:  actions[A_ButtonS + 0] = false; break;
         case MB_Right: actions[A_ButtonS + 1] = false; break;
         }
         break;
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
   // long _Fract DGE_GetInputAxis(unsigned num, unsigned axis)
   //
   DGE_Code_NativeDefn(DGE_GetInputAxis)
   {
      auto const &frame = InputSource::GetCurrent()->getNext();
      int ret;

      bool y = argv[1];
      switch(argv[0])
      {
      default:
      case 1: ret = y ? frame.ax1y : frame.ax1x; break;
      case 2: ret = y ? frame.ax2y : frame.ax2x; break;
      }

      task->dataStk.push(Code::SFractToSLFract(ret));
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

