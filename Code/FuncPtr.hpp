//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM function pointers.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__FuncPtr_H__
#define DGE__Code__FuncPtr_H__

#include "Code/TuplePtr.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // FuncPtr
   //
   template<typename Ret, typename... Args>
   class FuncPtr<Ret(Args...)>
   {
   public:
      FuncPtr(Word idx_ = 0) : idx{idx_} {}


      //
      // operator ()
      //
      Ret operator () (Args const &...args) const
      {
         std::array<Word, ArgC> argV;
         std::array<Word, RetC> retV;

         // Build argument array.
         MemInfo<std::tuple<Args...>>::Set(argV.data(), args...);

         auto proc = Process::GetMain();
         proc->call(&proc->prog->funcs[idx],
            argV.data(), argV.size(), retV.data(), retV.size());

         // Convert and return result.
         return MemInfo<Ret>::Get(retV.data());
      }

      explicit operator bool () const {return idx != 0;}
      operator Function * () const {return &Process::GetMain()->prog->funcs[idx];}
      explicit operator Word () const {return idx;}

      bool operator == (FuncPtr const &r) const {return idx == r.idx;}
      bool operator != (FuncPtr const &r) const {return idx != r.idx;}

      bool operator <  (FuncPtr const &r) const {return idx <  r.idx;}
      bool operator <= (FuncPtr const &r) const {return idx <= r.idx;}
      bool operator >  (FuncPtr const &r) const {return idx >  r.idx;}
      bool operator >= (FuncPtr const &r) const {return idx >= r.idx;}

   private:
      Word idx;


      static constexpr Word ArgC = MemInfo<std::tuple<Args...>>::Step;
      static constexpr Word RetC = MemInfo<Ret>::Step;
   };

   //
   // MemInfo<FuncPtr>
   //
   template<typename T>
   struct MemInfo<FuncPtr<T> const>
   {
      static Word Get(Memory *mem, Word idx) {return FuncPtr<T>{mem->getW(idx)};}

      static Word Get(Word const *mem) {return FuncPtr<T>{*mem};}

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step;
   };

   template<typename T>
   struct MemInfo<FuncPtr<T>> : MemInfo<FuncPtr<T> const>
   {
      static void Set(Memory *mem, Word idx, Word val)
         {mem->setW(idx, static_cast<Word>(val));}

      static void Set(Word *mem, Word val) {*mem = static_cast<Word>(val);}
   };
}

namespace std
{
   //
   // hash<::DGE::Code::FuncPtr>
   //
   template<typename T>
   struct hash<::DGE::Code::FuncPtr<T>>
   {
      std::size_t operator () (::DGE::Code::FuncPtr<T> f) const
         {return static_cast<::DGE::Code::Word>(f);}
   };
}

#endif//DGE__Code__FuncPtr_H__

