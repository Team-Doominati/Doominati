//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Bit streams.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__BitStream_H__
#define DGE__Core__BitStream_H__

#include <streambuf>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // BitStreamBase
   //
   class BitStreamBase
   {
   public:
      BitStreamBase(BitStreamBase const &) = delete;
      BitStreamBase(std::streambuf &buf_) : buf{&buf_}, getPos{0} {}

      explicit operator bool () const;

      BitStreamBase &operator = (BitStreamBase const &) = delete;

      unsigned getB();

   protected:
      bool getFill();

      std::streambuf *buf;

      unsigned getBuf;
      unsigned getPos;
   };

   //
   // BitStreamBE
   //
   class BitStreamBE : public BitStreamBase
   {
   public:
      using BitStreamBase::BitStreamBase;

      unsigned get();
      unsigned get(unsigned n);
   };

   //
   // BitStreamLE
   //
   class BitStreamLE : public BitStreamBase
   {
   public:
      using BitStreamBase::BitStreamBase;

      unsigned get();
      unsigned get(unsigned n);
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // BitStreamBE::get
   //
   inline unsigned BitStreamBE::get()
   {
      if(!getPos && getFill())
         return 0;

      return (getBuf >> --getPos) & 1;
   }

   //
   // BitStreamLE::get
   //
   inline unsigned BitStreamLE::get()
   {
      if(!getPos && getFill())
         return 0;

      unsigned res = getBuf & 1;
      getBuf >>= 1;
      getPos  -= 1;
      return res;
   }
}

#endif//DGE__Core__BitStream_H__

