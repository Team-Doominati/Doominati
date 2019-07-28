//-----------------------------------------------------------------------------
//
// Copyright (C) 2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef DGE__AL__Types_H__
#define DGE__AL__Types_H__

#include "../AL/OpenAL.hpp"

#include "../Code/Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   class Renderer;
   class SoundData;
   class SoundSource;

   using Sound = Core::Resource<SoundData>;
   using SoundSample = ALshort;
}

#endif//DGE__Code__Types_H__

