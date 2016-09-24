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

#ifndef DGE__Code__OpCode_H__
#define DGE__Code__OpCode_H__

#include "Code/Types.hpp"

#include <ostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
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
         #define DGE_Code_OpList(op) op,
         #include "Code/OpList.hpp"
      };


      Op                            op;
      union H {HWord h; Byte b[2];} h;
      union W {Word  w; H    h[2];} w;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   std::ostream &operator << (std::ostream &out, OpCode::Op in);
}

#endif//DGE__Code__OpCode_H__

