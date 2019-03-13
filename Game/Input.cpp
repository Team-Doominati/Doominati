//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
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
#include <unordered_map>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Game
{
   static std::unordered_map<unsigned, InputSource *> InputSources;
   static InputSource_Local *LocalInput;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // InputSource::Get
   //
   InputSource *InputSource::Get(unsigned player)
   {
      if(auto it = InputSources.find(player); it != InputSources.end())
         return it->second;
      else
         return nullptr;
   }

   //
   // InputSource::Set
   //
   void InputSource::Set(unsigned player, InputSource *input)
   {
      InputSources[player] = input;
   }

   //
   // InputSource_Local constructor
   //
   InputSource_Local::InputSource_Local() :
      frameLast{},
      frameNext{},
      bindsKey(NumBind)
   {
      for(auto &bind : bindsMouse) bind = ~0u;
   }

   //
   // InputSource_Local::poll
   //
   void InputSource_Local::poll()
   {
      frameLast = frameNext;
      ProcessGameEvents(*this);
   }

   //
   // InputSource_Local::sink
   //
   void InputSource_Local::sink(Event const &event)
   {
      switch(event.type)
      {
      case Event::KeyDown:
         if(auto it = bindsKey.find(event.data.key); it != bindsKey.end())
            frameNext.bind[it->second] = true;
         break;

      case Event::KeyUp:
         if(auto it = bindsKey.find(event.data.key); it != bindsKey.end())
            frameNext.bind[it->second] = false;
         break;

      case Event::MouseDown:
         if(auto btn = bindsMouse[static_cast<std::size_t>(event.data.mb)]; btn != ~0u)
            frameNext.bind[btn] = true;
         break;

      case Event::MouseUp:
         if(auto btn = bindsMouse[static_cast<std::size_t>(event.data.mb)]; btn != ~0u)
            frameNext.bind[btn] = false;
         break;

      case Event::MouseMove:
         frameNext.curs.x = event.data.mouse.x;
         frameNext.curs.y = event.data.mouse.y;
         break;

      case Event::MouseWheel:
         // TODO
         break;

      case Event::GamepadDown:
         // TODO
         break;

      case Event::GamepadUp:
         // TODO
         break;

      case Event::GamepadMove:
         // TODO
         break;
      }
   }

   //
   // InputSource_Local::bindKey
   //
   void InputSource_Local::bindKey(unsigned btn, char32_t ch)
   {
      bindsKey.insert_or_assign(ch, btn);
   }

   //
   // InputSource_Local::bindMouse
   //
   void InputSource_Local::bindMouse(unsigned btn, MouseButton mb)
   {
      if(mb < MouseButton::Max)
         bindsMouse[static_cast<std::size_t>(mb)] = btn;
   }

   //
   // InputSource_Local::GetCurrent
   //
   InputSource_Local *InputSource_Local::GetCurrent()
   {
      return LocalInput;
   }

   //
   // InputSource_Local::SetCurrent
   //
   void InputSource_Local::SetCurrent(InputSource_Local *input)
   {
      LocalInput = input;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // DGE_Point3R DGE_Input_GetAxis(unsigned player, unsigned num)
   //
   DGE_Code_NativeDefn(DGE_Input_GetAxis)
   {
      auto const &frame = InputSource::Get(argv[0])->getNext();

      if(argv[1] < InputSource::NumAxis)
      {
         auto &axis = frame.axis[argv[1]];
         task->dataStk.push(Code::SFractToSLFract(axis.x));
         task->dataStk.push(Code::SFractToSLFract(axis.y));
         task->dataStk.push(Code::SFractToSLFract(axis.z));
      }
      else
      {
         task->dataStk.push(0);
         task->dataStk.push(0);
         task->dataStk.push(0);
      }

      return false;
   }

   //
   // unsigned DGE_Input_GetButton(unsigned player, unsigned btn)
   //
   DGE_Code_NativeDefn(DGE_Input_GetButton)
   {
      auto const &frameNext = InputSource::Get(argv[0])->getNext();
      auto const &framePrev = InputSource::Get(argv[0])->getLast();

      if(argv[1] < InputSource::NumBind)
      {
         unsigned next = frameNext.bind[argv[1]];
         unsigned prev = framePrev.bind[argv[1]];
         task->dataStk.push((next << 0) | (prev << 1));
      }

      return false;
   }

   //
   // DGE_Point2I DGE_Input_GetCursor(unsigned player)
   //
   DGE_Code_NativeDefn(DGE_Input_GetCursor)
   {
      auto const &frame = InputSource::Get(argv[0])->getNext();
      task->dataStk.push(frame.curs.x);
      task->dataStk.push(frame.curs.y);
      return false;
   }

   //
   // void DGE_Input_SetBindKey(unsigned btn, int ch)
   //
   DGE_Code_NativeDefn(DGE_Input_SetBindKey)
   {
      InputSource_Local::GetCurrent()->bindKey(argv[0], argv[1]);
      return false;
   }

   //
   // void DGE_Input_SetBindMouse(unsigned btn, int mb)
   //
   DGE_Code_NativeDefn(DGE_Input_SetBindMouse)
   {
      InputSource_Local::GetCurrent()->bindMouse(
         argv[0], static_cast<MouseButton>(argv[1]));
      return false;
   }
}

// EOF

