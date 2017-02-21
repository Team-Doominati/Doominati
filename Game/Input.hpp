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
      std::int16_t  ax1x, ax1y;
      std::int16_t  ax2x, ax2y;
      std::uint16_t buttons;
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


      static InputSource *GetCurrent();
      static void SetCurrent(InputSource *input);
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
      enum {A_AxF, A_AxB, A_AxR, A_AxL};
      enum
      {
         A_Ax1S,    A_Ax1E    = A_Ax1S    + 4,
         A_Ax2S,    A_Ax2E    = A_Ax2S    + 4,
         A_ButtonS, A_ButtonE = A_ButtonS + 8,
         A_Max
      };

      InputFrame frameLast{};
      InputFrame frameNext{};

      bool actions[A_Max]{};
   };
}

#endif//DGE__Game__Input_H__

