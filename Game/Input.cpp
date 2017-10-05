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

      frameNext.buttons = 0;
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
         case Key::Escape: throw EXIT_SUCCESS;
         }
         break;

      case Event::MouseDown:
         switch(event.data.mb)
         {
         case MouseButton::Left:  actions[A_ButtonS + 0] = true; break;
         case MouseButton::Right: actions[A_ButtonS + 1] = true; break;
         }
         break;

      case Event::MouseUp:
         switch(event.data.mb)
         {
         case MouseButton::Left:  actions[A_ButtonS + 0] = false; break;
         case MouseButton::Right: actions[A_ButtonS + 1] = false; break;
         }
         break;

      case Event::MouseMove:
         frameNext.curx = event.data.mouse.x;
         frameNext.cury = event.data.mouse.y;
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
   // DGE_Point3R DGE_Input_GetAxis(unsigned num)
   //
   DGE_Code_NativeDefn(DGE_Input_GetAxis)
   {
      auto const &frame = InputSource::GetCurrent()->getNext();

      switch(argv[0])
      {
      default:
      case 1:
         task->dataStk.push(Code::SFractToSLFract(frame.ax1x));
         task->dataStk.push(Code::SFractToSLFract(frame.ax1y));
         task->dataStk.push(0);
         break;
      case 2:
         task->dataStk.push(Code::SFractToSLFract(frame.ax2x));
         task->dataStk.push(Code::SFractToSLFract(frame.ax2y));
         task->dataStk.push(0);
         break;
      }

      return false;
   }

   //
   // unsigned DGE_Input_GetButtons(void)
   //
   DGE_Code_NativeDefn(DGE_Input_GetButtons)
   {
      task->dataStk.push(InputSource::GetCurrent()->getNext().buttons);
      return false;
   }

   //
   // DGE_Point2I DGE_Input_GetCursor(void)
   //
   DGE_Code_NativeDefn(DGE_Input_GetCursor)
   {
      auto const &frame = InputSource::GetCurrent()->getNext();
      task->dataStk.push(frame.curx);
      task->dataStk.push(frame.cury);
      return false;
   }
}

// EOF

