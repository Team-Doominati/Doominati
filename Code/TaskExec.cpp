//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM task execution.
//
//-----------------------------------------------------------------------------

#include "Code/Task.hpp"

#include "Code/OpCode.hpp"
#include "Code/Program.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// Doom_Code_DynamicGoto
//
// If nonzero, enables use of dynamic goto labels in core interpreter loop.
// Currently, only gcc syntax is supported.
//
#ifndef Doom_Code_DynamicGoto
// TODO: Use CMake to determine if GNUC dynamic goto syntax is available.
#if defined(__GNUC__) && defined(NDEBUG)
#define Doom_Code_DynamicGoto 1
#else
#define Doom_Code_DynamicGoto 0
#endif
#endif

//
// DeclCase
//
#if Doom_Code_DynamicGoto
#define DeclCase(name) case_OpCode##name
#else
#define DeclCase(name) case OpCode::name
#endif

//
// NextCase
//
#if Doom_Code_DynamicGoto
#define NextCase() goto *cases[(++codePtr)->op]
#else
#define NextCase() ++codePtr; goto next_case
#endif

//
// ThisCase
//
#if Doom_Code_DynamicGoto
#define ThisCase() goto *cases[codePtr->op]
#else
#define ThisCase() goto next_case
#endif


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
      //
      // Task::exec
      //
      void Task::exec()
      {
         if(delay)
         {
            --delay;
            return;
         }

         #if Doom_Code_DynamicGoto
         static void const *const cases[] =
         {
            #define Doom_Code_OpList(name, ...) &&case_OpCode##name,
            #include "Code/OpList.hpp"
         };
         #endif

         #if Doom_Code_DynamicGoto
         ThisCase();
         #else
         next_case: switch(codePtr->op)
         #endif
         {
         DeclCase(Kill):
            std::cerr << "Kill: " << codePtr->h.h << '-' << codePtr->w.w << '\n';
            goto task_stop;

         DeclCase(Call):
         {
            Function *func = &prog->funcs[dataStk[1]];
            dataStk.drop();

            // Reserve stack space.
            callStk.reserve(CallStkSize);
            dataStk.reserve(DataStkSize);

            // Push call frame.
            callStk.push({codePtr, (Word)locReg.size(), vaaRegC});

            // Process arguments.
            Word argC = codePtr->w.w;
            auto argV = &dataStk[argC];
            Word vaaC = argC > func->param ? argC - func->param : 0;
            auto vaaV = argV + argC - vaaC;

            // Copy variadic arguments.
            locReg.alloc(vaaC);
            std::copy(vaaV, vaaV + vaaC, &locReg[0]);

            // Copy normal arguments.
            locReg.alloc(std::max(func->local, func->param));
            std::copy(argV, vaaV, &locReg[0]);

            // Fill missing arguments.
            if(argC < func->param)
               std::fill(&locReg[argC], &locReg[func->param], 0);

            dataStk.drop(argC);

            // Apply function data.
            codePtr = func->entry;
            vaaRegC = vaaC;
         }
            ThisCase();

         DeclCase(Drop_Nul):
            dataStk.drop();
            NextCase();

         DeclCase(Drop_Ptr):
            // TODO
            dataStk.drop(2);
            NextCase();

         DeclCase(Drop_Reg):
            locReg[codePtr->w.w] = dataStk[1];
            dataStk.drop();
            NextCase();

         DeclCase(Push_Lit):
            dataStk.push(codePtr->w.w);
            NextCase();

         DeclCase(Push_Ptr):
            // TODO
            NextCase();

         DeclCase(Push_Reg):
            dataStk.push(locReg[codePtr->w.w]);
            NextCase();

         DeclCase(Retn):
            // If no call frames left, terminate the task.
            if(callStk.empty())
               goto task_stop;

            // Apply call frame.
            codePtr   = callStk[1].codePtr;
            vaaRegC   = callStk[1].vaaRegC;
            locReg.free(callStk[1].locRegC);
            locReg.free(callStk[1].vaaRegC);


            // Drop call frame.
            callStk.drop();

            NextCase();
         }

      task_stop:
         stop();
         goto exec_stop;

      exec_intr:
         // Execution interrupted, check execution state.
         if(!delay)
            NextCase();

      exec_stop:;
      }
   }
}

// EOF

