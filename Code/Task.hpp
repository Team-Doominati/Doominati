//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM tasks.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Code__Task_H__
#define Doom__Code__Task_H__

#include "Code/Types.hpp"

#include "Core/List.hpp"
#include "Core/Stack.hpp"
#include "Core/Store.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // CallFrame
      //
      class CallFrame
      {
      public:
         OpCode const *codePtr;
         Word          locRegC;
         Word          vaaRegC;
      };

      //
      // Task
      //
      class Task
      {
      public:
         Task();
         Task(Task const &) = delete;
         Task(Task &&) = delete;

         void exec();

         void stop();

         Core::ListLink<Task> link;

         Core::Stack<CallFrame> callStk;
         Core::Stack<Word>      dataStk;
         Core::Store<Word>      locReg;

         Process *proc;
         Program *prog;
         Thread  *thrd;

         OpCode const *codePtr;
         Word          delay;
         Word          vaaRegC;


         static Task *Create(Thread *thrd);

         static constexpr std::size_t CallStkSize =   8;
         static constexpr std::size_t DataStkSize = 256;
      };
   }
}

#endif//Doom__Code__Task_H__

