//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Builtin console shell.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Sh__Shell_H__
#define DGE__Sh__Shell_H__

#include "../Sh/Types.hpp"

#include "../Code/MemPtr.hpp"

#include "../Core/IDAllocator.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Sh
{
   //
   // Shell
   //
   class Shell
   {
   public:
      Shell();
      ~Shell();

      void exec();

      std::vector<char>        cmdBuf;
      std::vector<std::size_t> cmdIdx;

      Code::Word const id;
      Code::SWord      cmdRes;


      static Shell *Get(Code::Word id) {return ShellVec[id];}

   private:
      void cmdEndWS();
      void cmdPutWS();

      bool execCmdDone();
      bool execCmdExec();
      bool execCmdRead();
      bool execCmdReadES();
      bool execCmdReadQS();
      bool execCmdWait();

      void putPrompt();

      std::vector<char *> cmdArg;

      bool (Shell::*state)();

      Code::Task *task;

      bool cmdWS;


      static Core::IDAllocator<Shell, Code::Word> &GetShellVec();

      static Core::IDAllocator<Shell, Code::Word> &ShellVec;
   };
}

namespace DGE::Code
{
   //
   // MemInfo<Sh::Shell *>
   //
   template<>
   struct MemInfo<Sh::Shell *const>
   {
      static Sh::Shell *Get(Memory *mem, Word idx) {return Sh::Shell::Get(mem->getW(idx));}

      static Sh::Shell *Get(Word const *mem) {return Sh::Shell::Get(*mem);}

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step;
   };

   template<>
   struct MemInfo<Sh::Shell *> : MemInfo<Sh::Shell *const>
   {
      static void Set(Memory *mem, Word idx, Sh::Shell *val) {mem->setW(idx, val->id);}

      static void Set(Word *mem, Sh::Shell *val) {*mem = val->id;}
   };
}

#endif//DGE__Sh__Shell_H__

