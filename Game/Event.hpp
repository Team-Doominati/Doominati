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

#ifndef DGE__Game__Event_H__
#define DGE__Game__Event_H__

#include "Core/Vector2.hpp"

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   namespace Key
   {
      //
      // Key
      //
      enum Key : char32_t
      {
         Invalid = '\0',

         Backspace = '\b',
         Tab       = '\t',
         Return    = '\r',
         Escape    = '\033',
         Delete    = '\177',

         Special  = 0x40000000,
         Capslock = Special + 0x39,

         F1, F2,  F3,  F4,
         F5, F6,  F7,  F8,
         F9, F10, F11, F12,
         SysRq,
         ScrollLock,
         Pause,

         Insert, Home, PageUp,
                 End,  PageDown,

         Right, Left, Down, Up,

         NumLock,
         KP_Div, KP_Mul, KP_Sub, KP_Add,
         KP_Enter,
         KP_1, KP_2, KP_3,
         KP_4, KP_5, KP_6,
         KP_7, KP_8, KP_9,
         KP_0, KP_Dot
      };
   }

   //
   // MouseButton
   //
   enum class MouseButton
   {
      Invalid,
      Left,
      Middle,
      Right,
      Extra1,
      Extra2,
      Extra3,
      Extra4
   };

   //
   // GamepadButton
   //
   enum class GamepadButton
   {
      Invalid,
      A, B, X, Y,
      Back,
      Menu,
      Start,
      StickLeft,
      StickRight,
      ShoulderLeft,
      ShoulderRight,
      DPadUp,
      DPadDown,
      DPadLeft,
      DPadRight
   };

   //
   // GamepadAxis
   //
   enum class GamepadAxis
   {
      Invalid,
      LeftX,
      LeftY,
      RightX,
      RightY,
      TriggerLeft,
      TriggerRight
   };

   //
   // Event
   //
   struct Event
   {
      enum Type
      {
         KeyDown,
         KeyUp,
         MouseDown,
         MouseUp,
         MouseMove,
         MouseWheel,
         GamepadDown,
         GamepadUp,
         GamepadMove
      };

      Event(Type type_) : type{type_} {}
      Event(Event &&) = default;
      Event(Event const &) = default;

      Type type;

      union
      {
         char32_t       key;
         MouseButton    mb;
         GamepadButton  gb;
         Core::Vector2I mouse;
         struct
         {
            GamepadAxis  num;
            std::int16_t val;
         } axis;
      } data;
   };

   //
   // EventSink
   //
   class EventSink
   {
   public:
      virtual ~EventSink() {}

      virtual void sink(Event const &event) = 0;
   };

   //
   // EventSink_Null
   //
   class EventSink_Null : public EventSink
   {
   public:
      virtual void sink(Event const &) {}
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   void EventFrame();
   void ProcessEvents(EventSink &sink);
   void ProcessGameEvents(EventSink &sink);
   void PumpEvents();
   void SetResolutionReal(int w, int h);
   void SetResolutionVirt(int w, int h);
}

#endif//DGE__Game__Event_H__

