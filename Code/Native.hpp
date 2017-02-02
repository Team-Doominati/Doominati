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
#if __GNUC__
# define DGE_Code_NativeArgs() \
   __attribute__((unused)) ::DGE::Code::Task *task, \
   __attribute__((unused)) ::DGE::Code::Word const *argv, \
   __attribute__((unused)) ::DGE::Code::Word argc
#else
# define DGE_Code_NativeArgs() ::DGE::Code::Task *task, \
   ::DGE::Code::Word const *argv, ::DGE::Code::Word argc
#endif

//
// DGE_Code_NativeDefn
//
#define DGE_Code_NativeDefn(name) \
   static bool DGE_Code_Native_##name(DGE_Code_NativeArgs()); \
   static ::DGE::Code::NativeAdder DGE_Code_NativeAdder_##name{ \
      "_" #name, DGE_Code_Native_##name}; \
   static bool DGE_Code_Native_##name(DGE_Code_NativeArgs())

//
// DGE_Code_NativeLoader
//
#define DGE_Code_NativeLoader(name) \
   namespace DGE::Code \
   { \
      void NativeLoad_##name() {} \
   }


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


      static void Finish();
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   extern Core::HashMapFixed<Core::HashedStr, Native> Natives;
}

#endif//DGE__Code__Native_H__

