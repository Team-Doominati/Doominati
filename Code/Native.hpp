//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM native functions.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Code__Native_H__
#define Doom__Code__Native_H__

#include "Code/Types.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/String.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// Doom_Code_NativeArgs
//
#if __GNUC__
# define Doom_Code_NativeArgs() \
   __attribute__((unused)) ::Doom::Code::Task *task, \
   __attribute__((unused)) ::Doom::Code::Word const *argv, \
   __attribute__((unused)) ::Doom::Code::Word argc
#else
# define Doom_Code_NativeArgs() ::Doom::Code::Task *task, \
   ::Doom::Code::Word const *argv, ::Doom::Code::Word argc
#endif

//
// Doom_Code_NativeDefn
//
#define Doom_Code_NativeDefn(name) \
   static bool Doom_Code_Native_##name(Doom_Code_NativeArgs()); \
   static ::Doom::Code::NativeAdder Doom_Code_NativeAdder_##name{ \
      "_" #name, Doom_Code_Native_##name}; \
   static bool Doom_Code_Native_##name(Doom_Code_NativeArgs())


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // NativeAdder
      //
      class NativeAdder
      {
      public:
         NativeAdder(Core::HashedStr name, Native native);


         static void Finish();
      };
   }
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace Doom
{
   namespace Code
   {
      extern Core::HashMapFixed<Core::HashedStr, Native> Natives;
   }
}

#endif//Doom__Code__Native_H__

