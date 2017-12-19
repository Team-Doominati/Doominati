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

#include "Sh/Shell.hpp"

#include "Sh/Command.hpp"
#include "Sh/StdIn.hpp"

#include "Code/Callback.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"
#include "Code/Thread.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/String.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Sh
{
   Core::IDAllocator<Shell, Code::Word> &Shell::ShellVec = Shell::GetShellVec();
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Sh
{
   //
   // ShellWrite
   //
   static Code::Word ShellWrite(std::ostream &out,
      Code::MemPtr<Code::Byte const> buf, Code::Word len)
   {
      Code::Word res = 0;
      for(auto end = buf + len; buf != end;)
      {
         if(out.put(*buf++))
            ++res;
         else
            break;
      }

      return res;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Sh
{
   //
   // Shell constructor
   //
   Shell::Shell() :
      id    {GetShellVec().alloc(this)},
      cmdRes{0},

      state{&Shell::execCmdDone},
      task {nullptr},

      cmdWS{true}
   {
   }

   //
   // Shell destructor
   //
   Shell::~Shell()
   {
   }

   //
   // Shell::cmdEndWS
   //
   void Shell::cmdEndWS()
   {
      if(cmdWS)
      {
         cmdIdx.push_back(cmdBuf.size());
         cmdWS = false;
      }
   }

   //
   // Shell::cmdPutWS
   //
   void Shell::cmdPutWS()
   {
      if(!cmdWS)
      {
         cmdBuf.push_back('\0');
         cmdWS = true;
      }
   }

   //
   // Shell::exec
   //
   void Shell::exec()
   {
      while((this->*state)()) {}
   }

   //
   // Shell::execCmdDone
   //
   bool Shell::execCmdDone()
   {
      cmdArg.clear();
      cmdBuf.clear();
      cmdIdx.clear();

      putPrompt();

      state = &Shell::execCmdRead;
      return true;
   }

   //
   // Shell::execCmdExec
   //
   bool Shell::execCmdExec()
   {
      char const *name = &cmdBuf[cmdIdx[0]];

      if(auto cmd = Commands.findVal(name))
      {
         cmdArg.reserve(cmdIdx.size());
         for(auto idx : cmdIdx)
            cmdArg.push_back(&cmdBuf[idx]);

         cmdRes = cmd(this, cmdArg.data(), cmdArg.size());
      }
      else if(auto cmd = VMCommands.find(name))
      {
         task = Code::Process::GetMain()->mainThread()->startTask(cmd, &id, 1);
         ++task->join;
         task->exec();
      }
      else
      {
         std::cerr << "Command not found: '" << name << "'\n";
      }

      state = &Shell::execCmdWait;
      return true;
   }

   //
   // Shell::execCmdRead
   //
   bool Shell::execCmdRead()
   {
      for(char c; StdIn_Read(&c, 1);) switch(c)
      {
      case ' ':
      case '\t':
         cmdPutWS();
         break;

      case '\n':
         // Ignore lines with only whitespace.
         if(cmdIdx.empty())
         {
            putPrompt();
            break;
         }

         cmdPutWS();
         state = &Shell::execCmdExec;
         return true;

      case '\\':
         state = &Shell::execCmdReadES;
         return true;

      case '\"':
         cmdEndWS();
         // TODO: Double-quoted strings.
         break;

      case '\'':
         cmdEndWS();
         state = &Shell::execCmdReadQS;
         return true;

      default:
         cmdEndWS();
         cmdBuf.push_back(c);
         break;
      }

      return false;
   }

   //
   // Shell::execCmdReadES
   //
   bool Shell::execCmdReadES()
   {
      for(char c; StdIn_Read(&c, 1);) switch(c)
      {
      default:
         cmdEndWS();
         cmdBuf.push_back(c);
      case '\n':
         state = &Shell::execCmdRead;
         return true;
      }

      return false;
   }

   //
   // Shell::execCmdReadQS
   //
   bool Shell::execCmdReadQS()
   {
      for(char c; StdIn_Read(&c, 1);) switch(c)
      {
      case '\'':
         state = &Shell::execCmdRead;
         return true;

      default:
         cmdBuf.push_back(c);
         break;
      }

      return false;
   }

   //
   // Shell::execCmdWait
   //
   bool Shell::execCmdWait()
   {
      if(task)
      {
         if(task->state != Code::TaskState::Stop)
            return false;

         cmdRes = task->dataStk[0];
         --task->join;
         task = nullptr;
      }

      state = &Shell::execCmdDone;
      return true;
   }

   //
   // Shell::putPrompt
   //
   void Shell::putPrompt()
   {
      std::cout << "DGE$ " << std::flush;
   }

   //
   // Shell::GetShellVec
   //
   Core::IDAllocator<Shell, Code::Word> &Shell::GetShellVec()
   {
      static Core::IDAllocator<Shell, Code::Word> vec{1};
      return vec;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Sh
{
   //
   // void DGE_Shell_GetArgBuf(unsigned sh, char *buf)
   //
   DGE_Code_NativeDefn(DGE_Shell_GetArgBuf)
   {
      Shell                   *sh  = Shell::Get(argv[0]);
      Code::MemPtr<Code::Byte> buf = {&task->prog->memory, argv[1]};

      for(auto c : sh->cmdBuf)
         *buf++ = c;

      return false;
   }

   //
   // unsigned DGE_Shell_GetArgBufLen(unsigned sh)
   //
   DGE_Code_NativeDefn(DGE_Shell_GetArgBufLen)
   {
      task->dataStk.push(Shell::Get(argv[0])->cmdBuf.size());
      return false;
   }

   //
   // unsigned DGE_Shell_GetArgC(unsigned sh)
   //
   DGE_Code_NativeDefn(DGE_Shell_GetArgC)
   {
      task->dataStk.push(Shell::Get(argv[0])->cmdIdx.size());
      return false;
   }

   //
   // void DGE_Shell_GetArgV(unsigned sh, char **argv, char *buf)
   //
   DGE_Code_NativeDefn(DGE_Shell_GetArgV)
   {
      Shell                                 *sh  = Shell::Get(argv[0]);
      Code::MemPtr<Code::MemPtr<Code::Byte>> av  = {&task->prog->memory, argv[1]};
      Code::MemPtr<Code::Byte>               buf = {&task->prog->memory, argv[2]};

      for(auto idx : sh->cmdIdx)
         *av++ = buf + idx;

      return false;
   }

   //
   // unsigned DGE_Shell_ReadStd(unsigned sh, char *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Shell_ReadStd)
   {
      Code::MemPtr<Code::Byte> buf{&task->prog->memory, argv[1]};
      Code::Word               len = argv[2];
      Code::Word               res = 0;

      while(len--)
      {
         char c;
         if(!StdIn_Read(&c, 1))
            break;

         *buf++ = c;
         ++res;

         // Only read one line at a time.
         if(c == '\n')
            break;
      }

      task->dataStk.push(res);
      return false;
   }

   //
   // unsigned DGE_Shell_WriteErr(unsigned sh, char const *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Shell_WriteErr)
   {
      task->dataStk.push(ShellWrite(std::cerr, {&task->prog->memory, argv[1]}, argv[2]));
      return false;
   }

   //
   // unsigned DGE_Shell_WriteStd(unsigned sh, char const *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_Shell_WriteStd)
   {
      task->dataStk.push(ShellWrite(std::cout, {&task->prog->memory, argv[1]}, argv[2]));
      return false;
   }
}

// EOF

