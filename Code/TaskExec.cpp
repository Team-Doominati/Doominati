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

#include "Code/Native.hpp"
#include "Code/OpCode.hpp"
#include "Code/Program.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Code_DynamicGoto
//
// If nonzero, enables use of dynamic goto labels in core interpreter loop.
// Currently, only gcc syntax is supported.
//
#ifndef DGE_Code_DynamicGoto
// TODO: Use CMake to determine if GNUC dynamic goto syntax is available.
#if defined(__GNUC__) && defined(NDEBUG)
#define DGE_Code_DynamicGoto 1
#else
#define DGE_Code_DynamicGoto 0
#endif
#endif

//
// DeclCase
//
#if DGE_Code_DynamicGoto
#define DeclCase(name) case_OpCode##name
#else
#define DeclCase(name) case OpCode::name
#endif

//
// IntrCase
//
#define IntrCase() do {++codePtr; goto exec_intr;} while(0)

//
// NextCase
//
#if DGE_Code_DynamicGoto
#define NextCase() goto *cases[(++codePtr)->op]
#else
#define NextCase() do {++codePtr; goto next_case;} while(0)
#endif

//
// ThisCase
//
#if DGE_Code_DynamicGoto
#define ThisCase() goto *cases[codePtr->op]
#else
#define ThisCase() goto next_case
#endif

//
// DefnCase_OpStk
//
#define DefnCase_OpStk(op) \
   DeclCase(op): op(dataStk[2], dataStk[1]); dataStk.drop(); NextCase()

//
// Ops
//
#define AddU(l, r) ((l) += (r))
#define AndU(l, r) ((l) &= (r))
#define MulU(l, r) ((l) *= (r))
#define OrIU(l, r) ((l) |= (r))
#define OrXU(l, r) ((l) ^= (r))
#define ShLU(l, r) ((l) <<= (r))
#define ShRU(l, r) ((l) >>= (r))
#define SubU(l, r) ((l) -= (r))


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Code
{
   //
   // CmpI_*
   //
   static inline void CmpI_LE(Word &l, Word r)
      {l = static_cast<SWord>(l) <= static_cast<SWord>(r);}
   static inline void CmpI_LT(Word &l, Word r)
      {l = static_cast<SWord>(l) <  static_cast<SWord>(r);}
   static inline void CmpI_GE(Word &l, Word r)
      {l = static_cast<SWord>(l) >= static_cast<SWord>(r);}
   static inline void CmpI_GT(Word &l, Word r)
      {l = static_cast<SWord>(l) >  static_cast<SWord>(r);}

   //
   // CmpU_*
   //
   static inline void CmpU_EQ(Word &l, Word r) {l = l == r;}
   static inline void CmpU_LE(Word &l, Word r) {l = l <= r;}
   static inline void CmpU_LT(Word &l, Word r) {l = l <  r;}
   static inline void CmpU_GE(Word &l, Word r) {l = l >= r;}
   static inline void CmpU_GT(Word &l, Word r) {l = l >  r;}
   static inline void CmpU_NE(Word &l, Word r) {l = l != r;}

   //
   // DivI
   //
   static inline void DivI(Word &l, Word r)
   {
      if(r) l = static_cast<SWord>(l) / static_cast<SWord>(r);
   }

   //
   // DivU
   //
   static inline void DivU(Word &l, Word r)
   {
      if(r) l /= r;
   }

   //
   // ModI
   //
   static inline void ModI(Word &l, Word r)
   {
      if(r) l = static_cast<SWord>(l) % static_cast<SWord>(r);
   }

   //
   // ModU
   //
   static inline void ModU(Word &l, Word r)
   {
      if(r) l %= r;
   }

   //
   // ShRI
   //
   static inline void ShRI(Word &l, Word r)
   {
      l = static_cast<SWord>(l) >> r;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
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

      #if DGE_Code_DynamicGoto
      static decltype(&&case_OpCodeNop) const cases[] =
      {
         #define DGE_Code_OpList(name, ...) &&case_OpCode##name,
         #include "Code/OpList.hpp"
      };
      #endif

      #if DGE_Code_DynamicGoto
      ThisCase();
      #else
      next_case: switch(codePtr->op)
      #endif
      {
      DeclCase(Nop):
         NextCase();

      DeclCase(Kill):
         std::cerr << "Kill: " << codePtr->h.h << '-' << codePtr->w.w << '\n';
         goto task_stop;

         DefnCase_OpStk(AddU);
         DefnCase_OpStk(AndU);
         DefnCase_OpStk(DivI);
         DefnCase_OpStk(DivU);
         DefnCase_OpStk(ModI);
         DefnCase_OpStk(ModU);
         DefnCase_OpStk(MulU);
         DefnCase_OpStk(OrIU);
         DefnCase_OpStk(OrXU);
         DefnCase_OpStk(ShLU);
         DefnCase_OpStk(ShRI);
         DefnCase_OpStk(ShRU);
         DefnCase_OpStk(SubU);

         DefnCase_OpStk(CmpI_LE);
         DefnCase_OpStk(CmpI_LT);
         DefnCase_OpStk(CmpI_GE);
         DefnCase_OpStk(CmpI_GT);
         DefnCase_OpStk(CmpU_EQ);
         DefnCase_OpStk(CmpU_LE);
         DefnCase_OpStk(CmpU_LT);
         DefnCase_OpStk(CmpU_GE);
         DefnCase_OpStk(CmpU_GT);
         DefnCase_OpStk(CmpU_NE);

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

      DeclCase(Cnat):
         dataStk.drop(codePtr->h.h);
         if(Natives[codePtr->w.w](this, &dataStk[0], codePtr->h.h))
            IntrCase();
         NextCase();

      DeclCase(Copy):
         dataStk.push(dataStk[1]);
         NextCase();

      DeclCase(Drop_Nul):
         dataStk.drop();
         NextCase();

      DeclCase(Drop_Ptr):
         prog->memory.setW(dataStk[1] >> 2, dataStk[2]);
         dataStk.drop(2);
         NextCase();

      DeclCase(Drop_Reg):
         locReg[codePtr->w.w] = dataStk[1];
         dataStk.drop();
         NextCase();

      DeclCase(InvU):
         dataStk[1] = ~dataStk[1];
         NextCase();

      DeclCase(Jcnd_Lit):
         if(dataStk[1] == codePtr++->w.w)
         {
            dataStk.drop();
            codePtr = &prog->codes[codePtr->w.w];
         }
         else
            ++codePtr;
         ThisCase();

      DeclCase(Jcnd_Nil):
         if(dataStk[1])
            ++codePtr;
         else
            codePtr = &prog->codes[codePtr->w.w];
         dataStk.drop();
         ThisCase();

      DeclCase(Jcnd_Tru):
         if(dataStk[1])
            codePtr = &prog->codes[codePtr->w.w];
         else
            ++codePtr;
         dataStk.drop();
         ThisCase();

      DeclCase(Jump):
         codePtr = &prog->codes[dataStk[1]];
         dataStk.drop();
         ThisCase();

      DeclCase(Jump_Lit):
         codePtr = &prog->codes[codePtr->w.w];
         ThisCase();

      DeclCase(LNot):
         dataStk[1] = !dataStk[1];
         NextCase();

      DeclCase(NegI):
         dataStk[1] = ~dataStk[1] + 1;
         NextCase();

      DeclCase(Push_Lit):
         dataStk.push(codePtr->w.w);
         NextCase();

      DeclCase(Push_Ptr):
         dataStk[1] = prog->memory.getW(dataStk[1] >> 2);
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

      DeclCase(Swap):
         std::swap(dataStk[1], dataStk[2]);
         NextCase();
      }

   task_stop:
      state = TaskState::Stop;
      goto exec_stop;

   exec_intr:
      // Execution interrupted, check execution state.
      if(!delay)
         ThisCase();

   exec_stop:;
   }
}

// EOF

