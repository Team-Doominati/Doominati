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

#include "Core/BitStream.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // BitStreamBase::getB
   //
   unsigned BitStreamBase::getB()
   {
      getPos = 0;

      auto c = buf->sbumpc();
      if(c == std::streambuf::traits_type::eof())
         return 0;

      return static_cast<unsigned char>(c);
   }

   //
   // BitStreamBase::getFill
   //
   bool BitStreamBase::getFill()
   {
      auto c = buf->sbumpc();
      if(c == std::streambuf::traits_type::eof())
         return true;

      getBuf = static_cast<unsigned char>(c);
      getPos = 8;

      return false;
   }

   //
   // BitStreamBE::get
   //
   unsigned BitStreamBE::get(unsigned n)
   {
      if(!getPos && getFill())
         return 0;

      // Check if there are enough bits in the buffer already.
      if(n <= getPos)
         return (getBuf >> (getPos -= n)) & ((1u << n) - 1);

      // Otherwise, consume the entire buffer.
      unsigned res = getBuf & ((1u << getPos) - 1);
      n -= getPos;

      // Consume 8-bit chunks, if possible.
      // Do NOT execute this loop if n == 8, because that would result in an
      // unnecessary getFill after the loop.
      for(; n > 8; n -= 8)
      {
         if(getFill()) return 0;

         res = (res << 8) | getBuf;
      }

      // Refill buffer and consume the remaining needed bits.
      if(getFill()) return 0;

      return (res << n) | ((getBuf >> (getPos -= n)) & ((1u << n) - 1));
   }

   //
   // BitStreamLE::get
   //
   unsigned BitStreamLE::get(unsigned n)
   {
      if(!getPos && getFill())
         return 0;

      // Check if there are enough bits in the buffer already.
      if(n <= getPos)
      {
         unsigned res = getBuf & ((1u << n) - 1);
         getBuf >>= n;
         getPos  -= n;
         return res;
      }

      // Otherwise, consume the entire buffer.
      unsigned res = getBuf;
      unsigned bit = getPos;
      n -= getPos;

      // Consume 8-bit chunks, if possible.
      // Do NOT execute this loop if n == 8, because that would result in an
      // unnecessary getFill after the loop.
      while(n > 8)
      {
         if(getFill()) return 0;

         res  = res | (getBuf << bit);
         bit += 8;
         n   -= 8;
      }

      // Refill buffer and consume the remaining needed bits.
      if(getFill()) return 0;

      res      = res | ((getBuf & ((1u << n) - 1)) << bit);
      getBuf >>= n;
      getPos  -= n;
      return res;
   }
}

// EOF

