//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM registerable callback handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Callback_H__
#define DGE__Code__Callback_H__

#include "Code/Types.hpp"

#include "Core/String.hpp"

#include <array>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Callback
   //
   class Callback
   {
   public:
      Callback(Core::HashedStr name);

      // operator ()
      void operator () () {call(nullptr, 0);}
      template<typename ...Args>
      void operator () (Args &&...args)
      {
         Word argV[] = {static_cast<Word>(args)...};
         call(argV, sizeof(argV) / sizeof(*argV));
      }

      // call
      void call(Word const *argV, Word argC);
      template<Word ArgC>
      void call(std::array<Word, ArgC> const &args)
         {call(args.data(), args.size());}

      void erase(Function *func);

      void insert(Function *func);

   private:
      std::vector<Function *> funcs;
   };
}

#endif//DGE__Code__Callback_H__

