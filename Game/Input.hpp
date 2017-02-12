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

#ifndef DGE__Game__Input_H__
#define DGE__Game__Input_H__

#include "Game/Event.hpp"

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // InputFrame
   //
   class InputFrame
   {
   public:
      std::int16_t ax1x, ax1y;
      std::int16_t ax2x, ax2y;
      std::int16_t ax3x, ax3y;
      std::int16_t buttons;
   };

   //
   // InputSource
   //
   class InputSource
   {
   public:
      virtual ~InputSource() {}

      virtual InputFrame const &getLast() const = 0;
      virtual InputFrame const &getNext() const = 0;

      virtual void poll() = 0;
   };

   //
   // InputSource_Local
   //
   class InputSource_Local : public InputSource, EventSink
   {
   public:
      virtual InputFrame const &getLast() const {return frameLast;}
      virtual InputFrame const &getNext() const {return frameNext;}

      virtual void poll();
      virtual void sink(Event const &event);

   private:
      InputFrame frameLast;
      InputFrame frameNext;
   };
}

#endif//DGE__Game__Input_H__

