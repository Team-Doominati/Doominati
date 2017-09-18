//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// String functions.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   DGE_Code_NativeLoaderDefn(Native_String);

   //
   // str DGE_String_Create(char const *str, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_String_Create)
   {
      task->dataStk.push(static_cast<std::size_t>(GDCC::Core::String::Get(
         MemStrDup({&task->prog->memory, argv[0]}, argv[1]).get())));
      return false;
   }
}

// EOF

