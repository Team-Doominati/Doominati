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

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Game
   {
      //
      // InputButtons
      //
      
      class InputButtons
      {
      public:
         bool attack    : 1;
         bool altattack : 1;
         bool use       : 1;
      };
      
      //
      // InputFrame
      //
      
      class InputFrame
      {
      public:
         std::int16_t movefwd,    moveside;
         std::int16_t deltapitch, deltayaw;
         InputButtons buttons;
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
