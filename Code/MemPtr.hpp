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

#ifndef Doom__Code__MemPtr_H__
#define Doom__Code__MemPtr_H__

#include "Code/Memory.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // MemFuncs
      //
      template<typename T>
      class MemFuncs;

      template<>
      class MemFuncs<Byte>
      {
      public:
         constexpr static Byte (Memory::*Get)(Word)       = &Memory::getB;
         constexpr static void (Memory::*Set)(Word, Byte) = &Memory::setB;
      };

      template<>
      class MemFuncs<HWord>
      {
      public:
         constexpr static HWord (Memory::*Get)(Word)        = &Memory::getH;
         constexpr static void  (Memory::*Set)(Word, HWord) = &Memory::setH;
      };

      template<>
      class MemFuncs<Word>
      {
      public:
         constexpr static Word (Memory::*Get)(Word)       = &Memory::getW;
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
      // MemRef<T const>
      //
      template<typename T>
      class MemRef<T const>
      {
      public:
         MemRef(Memory *mem_, Word idx_) : mem{mem_}, idx{idx_} {}

         operator T () const {return (mem->*MemFuncs<T>::Get)(idx);}

         MemRef &operator = (MemRef const &) = delete;
         MemRef const &operator = (T val) const = delete;

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
         MemPtr(Memory *mem_, Word idx_ = 0) : mem{mem_}, idx{idx_} {}

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

         MemPtr &operator = (MemPtr const &) = default;
         MemPtr &operator = (Word idx_) {idx = idx_; return *this;}

         MemPtr &operator += (Word off) {idx += off; return *this;}
         MemPtr *operator -= (Word off) {idx -= off; return *this;}

      private:
         Memory *mem;
         Word    idx;
      };
   }
}

#endif//Doom__Code__MemPtr_H__

