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

#ifndef Doom__Game__Input_H__
#define Doom__Game__Input_H__

#include "../Core/Coord.hpp"
#include "Keycode.hpp"

#include <string>
#include <utility>
#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Game
   {
      //
      // InputKind_Button
      //
      
      class InputKind_Button
      {
      public:
         KeyCode bind;
         bool    button;
      };
      
      //
      // InputKind_Axis
      //
      
      class InputKind_Axis
      {
      public:
         enum AxisType
         {
            None,
            Mouse,
            MouseWheel,
            Joystick1,
            Joystick2,
            JoystickT1,
            JoystickT2,
            Buttons
         };
         
         AxisType type;
         KeyCode  bind, bind2;
         double   deltax, deltay;
      };
      
      //
      // InputFrame
      //
      class InputFrame
      {
      public:
         std::unordered_map<std::string, InputKind_Button> input_btn;
         std::unordered_map<std::string, InputKind_Axis>   input_axs;
         
         InputFrame &operator=(InputFrame const &other)
         {
            input_btn = other.input_btn;
            input_axs = other.input_axs;
            return *this;
         }
      };

      //
      // InputSink
      //
      class InputSink
      {
      public:
         virtual ~InputSink() {}

         virtual void sink(InputFrame const &frame) = 0;
      };

      //
      // InputSink_Null
      //
      class InputSink_Null : public InputSink
      {
      public:
         virtual void sink(InputFrame const &frame);
      };

      //
      // InputSource
      //
      class InputSource
      {
      public:
         virtual ~InputSource() {}

         virtual bool canPoll() const = 0;

         virtual InputFrame const &getLast() const = 0;
         virtual InputFrame const &getNext() const = 0;
         
         virtual void addListener_Button(char const *name, KeyCode bind) = 0;
         virtual void addListener_Axis(char const *name, InputKind_Axis::AxisType type) = 0;
         virtual void addListener_ButtonAxis(char const *name, KeyCode bind, KeyCode bind2) = 0;

         virtual void poll() = 0;
      };

      //
      // InputSource_Local
      //
      class InputSource_Local : public InputSource
      {
      public:
         InputSource_Local();
         virtual ~InputSource_Local();

         virtual bool canPoll() const;

         virtual InputFrame const &getLast() const;
         virtual InputFrame const &getNext() const;
         
         virtual void addListener_Button(char const *name, KeyCode bind);
         virtual void addListener_Axis(char const *name, InputKind_Axis::AxisType type);
         virtual void addListener_ButtonAxis(char const *name, KeyCode bind, KeyCode bind2);

         virtual void poll();

      private:
         InputFrame frameLast;
         InputFrame frameNext;
      };
   }
}

#endif//Doom__Game__Input_H__

//
// EOF
//
