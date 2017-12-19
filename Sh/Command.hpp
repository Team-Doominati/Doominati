//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Builtin shell commands.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Sh__Command_H__
#define DGE__Sh__Command_H__

#include "../Sh/Types.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Sh_CommandArgs
//
#define DGE_Sh_CommandArgs() \
   [[maybe_unused]] Shell      *sh, \
   [[maybe_unused]] char      **argv, \
   [[maybe_unused]] std::size_t argc

//
// DGE_Sh_CommandDefn
//
#define DGE_Sh_CommandDefn(name) \
   static int DGE_Sh_Command_##name(DGE_Sh_CommandArgs()); \
   static ::DGE::Sh::CommandAdder DGE_Sh_CommandAdder_##name{ \
      #name, DGE_Sh_Command_##name}; \
   static int DGE_Sh_Command_##name(DGE_Sh_CommandArgs())


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Sh
{
   //
   // CommandAdder
   //
   class CommandAdder
   {
   public:
      CommandAdder(Core::HashedStr name, Command cmd);


      static void Add(Core::HashedStr name, Command cmd);

      static void Finish();
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Sh
{
   extern Core::HashMapFixed<Core::HashedStr, Command> Commands;

   extern Code::CallbackMap<Code::SWord(Shell *)> VMCommands;
}

#endif//DGE__Sh__Command_H__

