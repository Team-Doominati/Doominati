//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Handling for TEXTDEFS.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Textdefs_H__
#define DGE__Code__Textdefs_H__

#include "../Code/Types.hpp"

#include "../Defs/Textdefs.hpp"


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   extern Defs::Textdefs                        Textdefs;
   extern GDCC::Core::Array<GDCC::Core::String> TextdefsFallbacks;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   Defs::GamedefsGroup &GetTextdefsDefs();
   void TextdefsFinish();
}

#endif//DGE__Code__Textdefs_H__

