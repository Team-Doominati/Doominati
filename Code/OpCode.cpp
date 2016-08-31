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

namespace Doom
{
   namespace Code
   {
      //
      // operator std::ostream << OpCode::Op
      //
      std::ostream &operator << (std::ostream &out, OpCode::Op in)
      {
         switch(in)
         {
            #define Doom_Code_OpList(op) \
               case OpCode::op: out << #op; break;
            #include "Code/OpList.hpp"

         default:
            out << "Op(" << static_cast<std::uint16_t>(in) << ')';
            break;
         }

         return out;
      }
   }
}

// EOF

