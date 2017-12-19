//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM tasks.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Task_H__
#define DGE__Code__Task_H__

#include "../Code/Types.hpp"

#include "../Core/List.hpp"
#include "../Core/Stack.hpp"
#include "../Core/Store.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
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
   // TaskState
   //
   enum class TaskState
   {
      Exec,
      Stop,
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

      // Returns number of words returned. Rest of ret is zero-filled.
      Word stop(Word *retV, Word retC);

      Core::ListLink<Task> link;

      Core::Stack<CallFrame> callStk;
      Core::Stack<Word>      dataStk;
      Core::Store<Word>      locReg;

      Process *proc;
      Program *prog;
      Thread  *thrd;

      OpCode const *codePtr;
      TaskState     state;
      Word          delay;
      Word const    id;
      Word          join;
      Word          jumpbuf;
      Word          vaaRegC;


      static Task *Create(Thread *thrd);

      static constexpr std::size_t CallStkSize =   8;
      static constexpr std::size_t DataStkSize = 256;
   };
}

#endif//DGE__Code__Task_H__

