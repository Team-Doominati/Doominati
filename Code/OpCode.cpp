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

#include "Code/OpCode.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // operator std::ostream << OpCode::Op
   //
   std::ostream &operator << (std::ostream &out, OpCode::Op in)
   {
      switch(in)
      {
         #define DGE_Code_OpList(op) \
            case OpCode::op: out << #op; break;
         #include "Code/OpList.hpp"

      default:
         out << "Op(" << static_cast<std::uint16_t>(in) << ')';
         break;
      }

      return out;
   }
}

// EOF

