//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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
//
// A value of 1 denotes GCC style.
// A value of 2 denotes Microsoft Windows assembly using Visual C++ syntax.
//
#if !defined(DGE_Code_DynamicGoto) && defined(NDEBUG)
# if defined(__GNUC__) && DGE_Code_GNUC_DynamicGoto
#  define DGE_Code_DynamicGoto 1
// Due to complete bullshit, MSVC only allows you to use assembly on i686.
// This can be worked around with asm files, but we're not going to do that.
# elif defined(_MSC_VER) && defined(_WIN32) && defined(_M_IX86)
#  define DGE_Code_DynamicGoto 2
# else
#  define DGE_Code_DynamicGoto 0
# endif
#endif

//
// DeclCase
//
#if DGE_Code_DynamicGoto == 1 || DGE_Code_DynamicGoto == 2
# define DeclCase(name) case_OpCode##name
#else
# define DeclCase(name) case OpCode::name
#endif

//
// IntrCase
//
#define IntrCase() do {++codePtr; goto exec_intr;} while(0)

//
// NextCase
//
#if DGE_Code_DynamicGoto == 1
# define NextCase() goto *cases[(++codePtr)->op]
#elif DGE_Code_DynamicGoto == 2
# define NextCase() {auto _addr = cases[(++codePtr)->op]; __asm jmp _addr}
#else
# define NextCase() do {++codePtr; goto next_case;} while(0)
#endif

//
// ThisCase
//
#if DGE_Code_DynamicGoto == 1
# define ThisCase() goto *cases[codePtr->op]
#elif DGE_Code_DynamicGoto == 2
# define ThisCase() {auto _addr = cases[codePtr->op]; __asm jmp _addr}
#else
# define ThisCase() goto next_case
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
#define BAnd(l, r) ((l) &= (r))
#define BOrI(l, r) ((l) |= (r))
#define BOrX(l, r) ((l) ^= (r))
#define MulU(l, r) ((l) *= (r))
#define ShLU(l, r) ((l) <<= (r))
#define ShRU(l, r) ((l) >>= (r))
#define SubU(l, r) ((l) -= (r))


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Code
{
   //
   // Bclz
   //
   static inline void Bclz(Word &w)
   {
      #define Count0(base) (base)
      #define Count1(base) (w & (0x80000000 >> (base)) ? Count0(base) : Count0(base + 1))
      #define Count2(base) (w & (0xC0000000 >> (base)) ? Count1(base) : Count1(base + 2))
      #define Count4(base) (w & (0xF0000000 >> (base)) ? Count2(base) : Count2(base + 4))
      #define Count8(base) (w & (0xFF000000 >> (base)) ? Count4(base) : Count4(base + 8))

      w = w ? w & 0xFFFF0000 ? Count8(0) : Count8(16) : 32;

      #undef Count8
      #undef Count4
      #undef Count2
      #undef Count1
      #undef Count0
   }
   static inline void Bclo(Word &w) {Bclz(w = ~w);}

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
   // DiXI
   //
   static inline void DiXI(Word o[2], Word l, Word r)
   {
      if(r)
      {
         o[0] = static_cast<SWord>(l) / static_cast<SWord>(r);
         o[1] = static_cast<SWord>(l) % static_cast<SWord>(r);
      }
   }

   //
   // DiXU
   //
   static inline void DiXU(Word o[2], Word l, Word r)
   {
      if(r)
      {
         o[0] = l / r;
         o[1] = l % r;
      }
   }

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

   //
   // WriteDWord
   //
   static inline void WriteDWord(Word *l, DWord r)
   {
      l[0] = r;
      l[1] = r >> 32;
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
      if(delay && --delay)
         return;

      #if DGE_Code_DynamicGoto == 1
      static decltype(&&case_OpCodeNop) const cases[] =
      {
         #define DGE_Code_OpList(name, ...) &&case_OpCode##name,
         #include "Code/OpList.hpp"
      };
      #elif DGE_Code_DynamicGoto == 2
      static void *cases[OpCode::NumOpCodes];
      static bool caseInit = false;

      // There's probably a better way of doing this, but the mov+test is so
      // cheap that it isn't even worth looking into.
      if(!caseInit)
      {
         enum
         {
            // We have to define the opcode enum with C names here, so that we
            // can use it in assembly.
            #define DGE_Code_OpList(name, ...) _OpCode_##name,
            #include "Code/OpList.hpp"
         };

         // Here we initialize the cases array using assembly to get the
         // address of each case.
         __asm lea edx, cases
         #define DGE_Code_OpList(name, ...) \
            __asm lea eax, case_OpCode##name \
            __asm mov [edx][_OpCode_##name * TYPE cases], eax
         #include "Code/OpList.hpp"

         caseInit = true;
      }
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
         DefnCase_OpStk(BAnd);
         DefnCase_OpStk(BOrI);
         DefnCase_OpStk(BOrX);
         DefnCase_OpStk(DivI);
         DefnCase_OpStk(DivU);
         DefnCase_OpStk(ModI);
         DefnCase_OpStk(ModU);
         DefnCase_OpStk(MulU);
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

      DeclCase(Ad3U):
         WriteDWord(&dataStk[3], DWord(dataStk[2]) + dataStk[1] + dataStk[3]);
         dataStk.drop();
         NextCase();

      DeclCase(AdXU):
         WriteDWord(&dataStk[2], DWord(dataStk[2]) + dataStk[1]);
         NextCase();

      DeclCase(BNot):
         dataStk[1] = ~dataStk[1];
         NextCase();

      DeclCase(Bclo):
         Bclo(dataStk[1]);
         NextCase();

      DeclCase(Bclz):
         Bclz(dataStk[1]);
         NextCase();

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

      DeclCase(DiXI):
         DiXI(&dataStk[2], dataStk[2], dataStk[1]);
         NextCase();

      DeclCase(DiXU):
         DiXU(&dataStk[2], dataStk[2], dataStk[1]);
         NextCase();

      DeclCase(Drop_Nul):
         dataStk.drop();
         NextCase();

      DeclCase(Drop_Ptr):
         prog->memory.setW(dataStk[1] >> 2, dataStk[2]);
         dataStk.drop(2);
         NextCase();

      DeclCase(Drop_PtrB):
         prog->memory.setB(dataStk[1], dataStk[2]);
         dataStk.drop(2);
         NextCase();

      DeclCase(Drop_PtrH):
         prog->memory.setH(dataStk[1] >> 1, dataStk[2]);
         dataStk.drop(2);
         NextCase();

      DeclCase(Drop_Reg):
         locReg[codePtr->w.w] = dataStk[1];
         dataStk.drop();
         NextCase();

      DeclCase(Drop_RegB):
         SetWordByte(locReg[codePtr->w.w], codePtr->h.h, dataStk[1]);
         dataStk.drop();
         NextCase();

      DeclCase(Drop_RegH):
         SetWordHWord(locReg[codePtr->w.w], codePtr->h.h, dataStk[1]);
         dataStk.drop();
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

      DeclCase(Jcnd_Tab):
         if(Word *jump = prog->jumps[codePtr->w.w].find(dataStk[1]))
         {
            dataStk.drop();
            codePtr = &prog->codes[*jump];
         }
         else
            ++codePtr;
         ThisCase();

      DeclCase(Jcnd_Tru):
         if(dataStk[1])
            codePtr = &prog->codes[codePtr->w.w];
         else
            ++codePtr;
         dataStk.drop();
         ThisCase();

      DeclCase(Jfar_Pro):
         if(jumpbuf)
         {
            dataStk.drop(codePtr->h.h);

         Jfar_propogate:
            // If in the same call frame as the original Jfar_Set...
            if(prog->memory.getW(jumpbuf) == callStk.size())
            {
               // Push result and branch.
               for(Word i = 0, e = prog->memory.getW(jumpbuf + 1); i != e; ++i)
                  dataStk.push(prog->memory.getW(jumpbuf + 3 + i));
               codePtr = &prog->codes[prog->memory.getW(jumpbuf + 2)];

               // Clear jump state.
               jumpbuf = 0;
            }
            // Otherwise, branch to propogation target.
            else
               codePtr = &prog->codes[codePtr->w.w];
            ThisCase();
         }
         else
            NextCase();

      DeclCase(Jfar_Set):
         // Initialize jump buffer contents.
         prog->memory.setW((dataStk[1] >> 2) + 0, callStk.size());
         prog->memory.setW((dataStk[1] >> 2) + 1, codePtr->h.h);
         prog->memory.setW((dataStk[1] >> 2) + 2, codePtr->w.w);
         dataStk.drop();
         NextCase();

      DeclCase(Jfar_Sta):
         // Set jump state.
         jumpbuf = dataStk[1] >> 2; dataStk.drop();

         // Set result value.
         for(Word i = 0, e = codePtr->h.h; i != e; ++i)
            prog->memory.setW(jumpbuf + 3 + i, dataStk[e - i]);
         dataStk.drop(codePtr->h.h);

         // Go to Jfar propogation.
         goto Jfar_propogate;

      DeclCase(Jump):
         codePtr = &prog->codes[dataStk[1]];
         dataStk.drop();
         ThisCase();

      DeclCase(Jump_Lit):
         codePtr = &prog->codes[codePtr->w.w];
         ThisCase();

      DeclCase(LAnd):
         dataStk[2] = dataStk[2] && dataStk[1];
         dataStk.drop();
         NextCase();

      DeclCase(LNot):
         dataStk[1] = !dataStk[1];
         NextCase();

      DeclCase(LOrI):
         dataStk[2] = dataStk[2] || dataStk[1];
         dataStk.drop();
         NextCase();

      DeclCase(MuXU):
         WriteDWord(&dataStk[2], DWord(dataStk[2]) * dataStk[1]);
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

      DeclCase(Push_PtrB):
         dataStk[1] = prog->memory.getB(dataStk[1]);
         NextCase();

      DeclCase(Push_PtrH):
         dataStk[1] = prog->memory.getH(dataStk[1] >> 1);
         NextCase();

      DeclCase(Push_Reg):
         dataStk.push(locReg[codePtr->w.w]);
         NextCase();

      DeclCase(Push_RegB):
         dataStk.push(GetWordByte(locReg[codePtr->w.w], codePtr->h.h));
         NextCase();

      DeclCase(Push_RegH):
         dataStk.push(GetWordHWord(locReg[codePtr->w.w], codePtr->h.h));
         NextCase();

      DeclCase(Push_StrB):
         dataStk[2] = GDCC::Core::String{dataStk[2]}[dataStk[1]];
         dataStk.drop();
         NextCase();

      DeclCase(Retn):
         // If no call frames left, terminate the task.
         if(callStk.empty())
            goto task_stop;

         // Apply call frame.
         codePtr   = callStk[1].codePtr;
         locReg.free(           vaaRegC);
         locReg.free(callStk[1].locRegC);
         vaaRegC   = callStk[1].vaaRegC;

         // Drop call frame.
         callStk.drop();

         NextCase();

      DeclCase(Su3U):
         WriteDWord(&dataStk[3], DWord(dataStk[2]) - dataStk[1] - (~dataStk[3] + 1));
         dataStk.drop();
         NextCase();

      DeclCase(SuXU):
         WriteDWord(&dataStk[2], DWord(dataStk[2]) - dataStk[1]);
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

