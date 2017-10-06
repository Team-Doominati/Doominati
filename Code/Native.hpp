//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM native functions.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Native_H__
#define DGE__Code__Native_H__

#include "Code/Types.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/String.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Code_NativeArgs
//
#define DGE_Code_NativeArgs() \
   [[maybe_unused]] ::DGE::Code::Task *task, \
   [[maybe_unused]] ::DGE::Code::Word const *argv, \
   [[maybe_unused]] ::DGE::Code::Word argc

//
// DGE_Code_NativeDefn
//
#define DGE_Code_NativeDefn(name) \
   static bool DGE_Code_Native_##name(DGE_Code_NativeArgs()); \
   static ::DGE::Code::NativeAdder DGE_Code_NativeAdder_##name{ \
      "_" #name, DGE_Code_Native_##name}; \
   static bool DGE_Code_Native_##name(DGE_Code_NativeArgs())

//
// DGE_Code_NativeLoaderCall
//
#define DGE_Code_NativeLoaderCall(name) \
   NativeLoader_##name()

//
// DGE_Code_NativeLoaderDecl
//
#define DGE_Code_NativeLoaderDecl(name) \
   void NativeLoader_##name()

//
// DGE_Code_NativeLoaderDefn
//
#define DGE_Code_NativeLoaderDefn(name) \
   void NativeLoader_##name() {}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // NativeAdder
   //
   class NativeAdder
   {
   public:
      NativeAdder(Core::HashedStr name, Native native);


      static void Add(Core::HashedStr name, Callback *cb);
      static void Add(Core::HashedStr name, ExtensionMembers *em);
      static void Add(Core::HashedStr name, Native native);

      static void Finish();
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   extern Core::HashMapFixed<Core::HashedStr, Callback *> Callbacks;
   extern Core::HashMapFixed<Core::HashedStr, ExtensionMembers *> ExtMems;
   extern Core::HashMapFixed<Core::HashedStr, Native> Natives;
}

#endif//DGE__Code__Native_H__

