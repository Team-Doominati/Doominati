//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Builtin shell commands.
//
//-----------------------------------------------------------------------------

#include "Sh/Command.hpp"

#include "Code/Callback.hpp"
#include "Code/Native.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/String.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Sh
{
   //
   // CommandVec
   //
   static auto &CommandVec()
   {
      static auto *vec = new std::vector<std::pair<Core::HashedStr, Command>>;
      return vec;
   }
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Sh
{
   Core::HashMapFixed<Core::HashedStr, Command> Commands;

   Code::CallbackMap<Code::SWord(Shell *)> VMCommands{"ShellCommand"};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Sh
{
   DGE_Code_NativeLoaderDecl(Command_File);
   DGE_Code_NativeLoaderDecl(Command_Misc);

   //
   // CommandAdder constructor
   //
   CommandAdder::CommandAdder(Core::HashedStr name, Command cmd)
   {
      Add(name, cmd);
   }

   //
   // CommandAdder::Add
   //
   void CommandAdder::Add(Core::HashedStr name, Command cmd)
   {
      CommandVec()->push_back({name, cmd});
   }

   //
   // CommandAdder::Finish
   //
   void CommandAdder::Finish()
   {
      DGE_Code_NativeLoaderCall(Command_File);
      DGE_Code_NativeLoaderCall(Command_Misc);

      // Commands.
      {
         auto &vec = CommandVec();

         auto vecItr = vec->begin();
         Commands.reset(vec->size(),
            [&](auto elem){elem->key = vecItr->first; elem->val = vecItr->second; ++vecItr;});

         delete vec;
         vec = nullptr;
      }
   }
}

// EOF

