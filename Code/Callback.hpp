//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM registerable callback handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Callback_H__
#define DGE__Code__Callback_H__

#include "../Code/Types.hpp"

#include "../Code/FuncPtr.hpp"

#include "../Core/String.hpp"

#include <set>
#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Callback
   //
   class Callback
   {
   public:
      Callback(Core::HashedStr name);

      virtual void erase(Program *prog, Word func) = 0;

      virtual void insert(Program *prog, Word func) = 0;
   };

   //
   // CallbackMap
   //
   template<typename Ret, typename... Args>
   class CallbackMap<Ret(Args...)> : public Callback
   {
   public:
      CallbackMap(Core::HashedStr name) : Callback{name} {}

      //
      // erase
      //
      virtual void erase(Program *prog, Word func)
      {
         funcs.erase(prog->funcs.data(func).key);
      }

      //
      // find
      //
      FuncPtr<Ret(Args...)> find(Core::HashedStr name)
      {
         if(auto func = funcs.find(name); func != funcs.end())
            return func->second;

         return 0;
      }

      //
      // insert
      //
      virtual void insert(Program *prog, Word func)
      {
         funcs.insert({prog->funcs.data(func).key, func});
      }

   private:
      std::unordered_map<Core::HashedStr, FuncPtr<Ret(Args...)>> funcs;
   };

   //
   // CallbackSet
   //
   template<typename Ret, typename... Args>
   class CallbackSet<Ret(Args...)> : public Callback
   {
   public:
      CallbackSet(Core::HashedStr name) : Callback{name} {}

      //
      // operator ()
      //
      void operator () (Args const &...args)
      {
         for(auto const &func : funcs)
            func(args...);
      }

      //
      // erase
      //
      virtual void erase(Program *, Word func)
      {
         funcs.erase(func);
      }

      //
      // insert
      //
      virtual void insert(Program *, Word func)
      {
         funcs.insert(func);
      }

   private:
      std::set<FuncPtr<Ret(Args...)>> funcs;
   };
}

#endif//DGE__Code__Callback_H__

