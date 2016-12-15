//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM programs.
//
//-----------------------------------------------------------------------------

#include "Code/Program.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // Program::findOriginFunc
   //
   GDCC::Core::String Program::findOriginFunc(std::size_t code)
   {
      GDCC::Core::String name = GDCC::Core::STR_;
      std::size_t        idx  = 0;

      for(auto const &func : funcs)
      {
         std::size_t i = func.val.entry - codes.data();
         if(idx < i && i <= code)
            i = idx, name = func.key;
      }

      return name;
   }

   //
   // Program::getOrigin
   //
   Origin Program::getOrigin(std::size_t code)
   {
      if(code >= codes.size())
         return {{}, GDCC::Core::STR_};

      if(code >= origs.size())
         return {{}, findOriginFunc(code)};

      auto orig = origs[code];

      if(!orig.func)
         orig.func = findOriginFunc(code);

      return orig;
   }
}

// EOF

