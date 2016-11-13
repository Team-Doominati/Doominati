//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM memory pointers.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__MemPtr_H__
#define DGE__Code__MemPtr_H__

#include "Code/Memory.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // MemFuncs
   //
   template<typename T>
   struct MemFuncs;

   //
   // MemFuncs<Byte>
   //
   template<>
   struct MemFuncs<Byte const>
   {
      constexpr static Byte (Memory::*Get)(Word) = &Memory::getB;

      constexpr static unsigned int Shift = 0;
   };

   template<>
   struct MemFuncs<Byte> : MemFuncs<Byte const>
   {
      constexpr static void (Memory::*Set)(Word, Byte) = &Memory::setB;
   };

   //
   // MemFuncs<HWord>
   //
   template<>
   struct MemFuncs<HWord const>
   {
      constexpr static HWord (Memory::*Get)(Word) = &Memory::getH;

      constexpr static unsigned int Shift = 1;
   };

   template<>
   struct MemFuncs<HWord> : MemFuncs<HWord const>
   {
      constexpr static void (Memory::*Set)(Word, HWord) = &Memory::setH;
   };

   //
   // MemFuncs<Word>
   //
   template<>
   struct MemFuncs<Word const>
   {
      constexpr static Word (Memory::*Get)(Word) = &Memory::getW;

      constexpr static unsigned int Shift = 2;
   };

   template<>
   struct MemFuncs<Word> : MemFuncs<Word const>
   {
      constexpr static void (Memory::*Set)(Word, Word) = &Memory::setW;
   };

   //
   // MemRef
   //
   template<typename T>
   class MemRef
   {
   public:
      MemRef(Memory *mem_, Word idx_) : mem{mem_}, idx{idx_} {}

      operator T () const {return (mem->*MemFuncs<T>::Get)(idx);}

      MemRef &operator = (MemRef const &) = delete;
      MemRef const &operator = (T val) const
         {(mem->*MemFuncs<T>::Set)(idx, val); return *this;}

   private:
      Memory *const mem;
      Word    const idx;
   };

   //
   // MemPtr
   //
   template<typename T>
   class MemPtr
   {
   public:
      MemPtr(Memory *mem_, Word idx_ = 0) :
         mem{mem_}, idx{idx_ >> MemFuncs<T>::Shift} {}

      explicit operator bool () const {return idx != 0;}

      MemPtr &operator ++ () {++idx; return *this;}
      MemPtr  operator ++ (int) {return {mem, idx++};}
      MemPtr &operator -- () {--idx; return *this;}
      MemPtr  operator -- (int) {return {mem, idx--};}

      MemRef<T> operator * () const {return {mem, idx};}

      MemPtr operator + (Word off) {return {mem, idx + off};}
      Word operator - (MemPtr const &r) {return idx - r.idx;}
      MemPtr operator - (Word off) {return {mem, idx - off};}

      bool operator == (MemPtr const &r) {return idx == r.idx;}
      bool operator != (MemPtr const &r) {return idx != r.idx;}

      MemPtr &operator = (MemPtr const &) = default;
      MemPtr &operator = (Word idx_)
         {idx = idx_ >> MemFuncs<T>::Shift; return *this;}

      MemPtr &operator += (Word off) {idx += off; return *this;}
      MemPtr *operator -= (Word off) {idx -= off; return *this;}

   private:
      Memory *mem;
      Word    idx;
   };
}

#endif//DGE__Code__MemPtr_H__

