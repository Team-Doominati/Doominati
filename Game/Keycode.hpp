//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Key-code names.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Game__Keycode_H__
#define Doom__Game__Keycode_H__


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Game
   {
      //
      // Keycode
      //
      
      enum KeyCode
      {
         KC_NULL   = 0,
         
         KC_TAB    = 0x09,
         KC_ENTER  = 0x0d,
         KC_ESCAPE = 0x1b,

         KC_SPACEBAR = 0x20,

         KC_COMMA = 0x2c,
         KC_MINUS,
         KC_PERIOD,

         KC_EQUALS = 0x3d,

         KC_ACCGRAVE = 0x60,

         KC_BACKSPACE = 0x7f,

         KC_CTRL = 0x9d,

         KC_LEFTARROW = 0xac,
         KC_UPARROW,
         KC_RIGHTARROW,
         KC_DOWNARROW,

         KC_SHIFT = 0xb6,
         KC_ALT   = 0xb8,

         KC_CAPSLOCK = 0xba,

         KC_F1,
         KC_F2,
         KC_F3,
         KC_F4,
         KC_F5,
         KC_F6,
         KC_F7,
         KC_F8,
         KC_F9,
         KC_F10,
         KC_NUMLOCK,
         KC_SCROLLLOCK,
         KC_HOME,
         
         KC_PAGEUP   = 0xc9,
         KC_END      = 0xcf,
         KC_PAGEDOWN = 0xd1,
         KC_INSERT,

         KC_F11 = 0xd7,
         KC_F12,

         KC_MOUSE1 = 0xe0,
         KC_MOUSE2,
         KC_MOUSE3,
         KC_MOUSE4,
         KC_MOUSE5,

         KC_KP0 = 0xed,
         KC_KP1,
         KC_KP2,
         KC_KP3,
         KC_KP4,
         KC_KP5,
         KC_KP6,
         KC_KP7,
         KC_KP8,
         KC_KP9,
         KC_KPPERIOD,
         KC_KPDIVIDE,
         KC_KPMULTIPLY,
         KC_KPMINUS,
         KC_KPPLUS,
         KC_KPENTER,
         KC_KPEQUALS,
         KC_DEL,
         KC_PAUSE, // 0xff
         KC_SYSRQ,

         KC_JOY01,
         KC_JOY02,
         KC_JOY03,
         KC_JOY04,
         KC_JOY05,
         KC_JOY06,
         KC_JOY07,
         KC_JOY08,
         KC_JOY09,
         KC_JOY10,
         KC_JOY11,
         KC_JOY12,
         KC_JOY13,
         KC_JOY14,
         KC_JOY15,
         KC_JOY16,
         
         KC_AXISON01,
         KC_AXISON02,
         KC_AXISON03,
         KC_AXISON04,
         KC_AXISON05,
         KC_AXISON06,
         KC_AXISON07,
         KC_AXISON08,
         
         KC_NUM
      };
   }
}

#endif//Doom__Game__Keycode_H__

//
// EOF
//
