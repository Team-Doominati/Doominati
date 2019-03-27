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

#ifndef DGE__Game__Input_H__
#define DGE__Game__Input_H__

#include "Core/HashMap.hpp"
#include "Core/Point.hpp"

#include "Game/Event.hpp"

#include <cstdint>
#include <vector> // TODO
#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   class InputFrame;

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


      static InputSource *Get(unsigned player);
      static void Set(unsigned player, InputSource *input);

      static std::size_t NumAxis;
      static std::size_t NumBind;
   };

   //
   // InputFrame
   //
   class InputFrame
   {
   public:
      using Axis = Core::Point<std:: int16_t, 3>;
      using Curs = Core::Point<std::uint16_t, 2>;
      using Bind = std::vector<bool>; // TODO: replace with non-crap bitset

      InputFrame() :
         axis{new Axis[InputSource::NumAxis]}, bind(InputSource::NumBind),
         curs{} {}

      ~InputFrame() {delete[] axis;}

      InputFrame &operator=(InputFrame const &o)
      {
         std::memmove(axis, o.axis, sizeof(Axis) * InputSource::NumAxis);
         bind = o.bind;
         curs = o.curs;
         return *this;
      }

      Axis *axis;
      Bind  bind;
      Curs  curs;
   };

   //
   // InputSource_Local
   //
   class InputSource_Local : public InputSource, EventSink
   {
   public:
      InputSource_Local();

      virtual ~InputSource_Local() {}

      virtual InputFrame const &getLast() const {return frameLast;}
      virtual InputFrame const &getNext() const {return frameNext;}

      virtual void poll();
      virtual void sink(Event const &event);

      void bindKey(unsigned btn, char32_t ch);
      void bindMouse(unsigned btn, MouseButton mb);


      static InputSource_Local *GetCurrent();
      static void SetCurrent(InputSource_Local *input);

   private:
      InputFrame frameLast;
      InputFrame frameNext;

      std::unordered_map<char32_t, unsigned> bindsKey;

      unsigned bindsMouse[static_cast<std::size_t>(MouseButton::Max)];
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   Defs::GamedefsGroup &GetInputDefs();
}

#endif//DGE__Game__Input_H__

