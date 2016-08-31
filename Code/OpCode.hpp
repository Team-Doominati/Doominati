//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM instructions.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Code__OpCode_H__
#define Doom__Code__OpCode_H__

#include "Code/Types.hpp"

#include <ostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // OpCode
      //
      class OpCode
      {
      public:
         //
         // Op
         //
         enum Op : std::uint16_t
         {
            #define Doom_Code_OpList(op) op,
            #include "Code/OpList.hpp"
         };


         std::uint16_t                 op;
         union H {HWord h; Byte b[2];} h;
         union W {Word  w; H    h[2];} w;
      };
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
      std::ostream &operator << (std::ostream &out, OpCode::Op in);
   }
}

#endif//Doom__Code__OpCode_H__

