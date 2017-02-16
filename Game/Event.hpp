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

#include "Core/Stack.hpp"

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // These are special characters outside any usable Unicode range which
   // represent special keypresses (like F1-F12).
   //
   enum : char32_t
   {
      KC_Invalid = '\0',

      // ASCII keys
      KC_Backspace = '\b',
      KC_Tab       = '\t',
      KC_Return    = '\r',
      KC_Escape    = '\033',
      KC_Delete    = '\177',

      // Special keys, using a Unicode reserved plane
      KC_Special  = 0x40000000,
      KC_Capslock = KC_Special + 0x39,

      // Function keys
      KC_F1, KC_F2,  KC_F3,  KC_F4,
      KC_F5, KC_F6,  KC_F7,  KC_F8,
      KC_F9, KC_F10, KC_F11, KC_F12,
      KC_SysRq,
      KC_ScrollLock,
      KC_Pause,

      // Movement keys
      KC_Insert, KC_Home, KC_PageUp, KC_End, KC_PageDown,

      // Arrow keys
      KC_Right, KC_Left, KC_Down, KC_Up,

      // Keypad
      KC_NumLock,
      KC_KP_Div, KC_KP_Mul, KC_KP_Sub, KC_KP_Add,
      KC_KP_Enter,
      KC_KP_1, KC_KP_2, KC_KP_3,
      KC_KP_4, KC_KP_5, KC_KP_6,
      KC_KP_7, KC_KP_8, KC_KP_9,
      KC_KP_0, KC_KP_Dot
   };

   //
   // MouseButton
   //
   enum MouseButton
   {
      MB_Invalid,
      MB_Left,
      MB_Middle,
      MB_Right,
      MB_Extra1,
      MB_Extra2
   };

   //
   // Event
   //
   struct Event
   {
      enum Type
      {
         Null,
         KeyDown,   KeyUp,
         MouseDown, MouseUp,
         MouseMove,
         MouseWheel,
         // TODO: gamepad support
         Max
      };

      Event(Type type_) : type(type_) {}
      Event(Event &&) = default;
      Event(Event const &) = default;

      Type type;

      union
      {
         char32_t    key;
         MouseButton mb;
         struct {float x, y;} axis;
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
   void PumpEvents();
   void ProcessEvents(EventSink &sink);
   void ProcessGameEvents(EventSink &sink);
}

#endif//DGE__Game__Event_H__

