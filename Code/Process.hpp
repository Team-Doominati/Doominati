//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM processes.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Process_H__
#define DGE__Code__Process_H__

#include "Code/Types.hpp"

#include "Core/List.hpp"

#include <array>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Process
   //
   class Process
   {
   public:
      Process(Program *prog);
      ~Process();

      // call
      Word call(Function *func) {return call(func, nullptr, 0, nullptr, 0);}

      Word call(Function *func, Word const *argV, Word argC, Word *retV, Word retC);

      //
      // call
      //
      template<Word ArgC, Word RetC = 0>
      std::array<Word, RetC> call(Function *func, std::array<Word, ArgC> const &args)
      {
         std::array<Word, RetC> ret;
         call(func, args.data(), args.size(), ret.data(), ret.size());
         return ret;
      }

      void exec();

      Thread *mainThread();

      Core::ListLink<Thread> threads;

      Program *const prog;


      static Process *GetMain();
   };
}

#endif//DGE__Code__Process_H__

