//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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

#include <iterator>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // MemInfo
   //
   template<typename T>
   struct MemInfo;

   //
   // MemInfo<void>
   //
   template<>
   struct MemInfo<void const>
   {
      static void Get(Memory *, Word) {}

      static void Get(Word const *) {}

      constexpr static unsigned int Shift = 0;
      constexpr static unsigned int Step  = 0;
   };

   template<>
   struct MemInfo<void> : MemInfo<void const>
   {
   };

   //
   // MemInfo<Byte>
   //
   template<>
   struct MemInfo<Byte const>
   {
      static Byte Get(Memory *mem, Word idx) {return mem->getB(idx);}

      constexpr static unsigned int Shift = 0;
      constexpr static unsigned int Step  = 1;
   };

   template<>
   struct MemInfo<Byte> : MemInfo<Byte const>
   {
      static void Set(Memory *mem, Word idx, Byte val) {mem->setB(idx, val);}
   };

   //
   // MemInfo<HWord>
   //
   template<>
   struct MemInfo<HWord const>
   {
      static HWord Get(Memory *mem, Word idx) {return mem->getH(idx);}

      constexpr static unsigned int Shift = 1;
      constexpr static unsigned int Step  = 1;
   };

   template<>
   struct MemInfo<HWord> : MemInfo<HWord const>
   {
      static void Set(Memory *mem, Word idx, HWord val) {mem->setH(idx, val);}
   };

   //
   // MemInfo<Word>
   //
   template<>
   struct MemInfo<Word const>
   {
      static Word Get(Memory *mem, Word idx) {return mem->getW(idx);}

      static Word Get(Word const *mem) {return *mem;}

      constexpr static unsigned int Shift = 2;
      constexpr static unsigned int Step  = 1;
   };

   template<>
   struct MemInfo<Word> : MemInfo<Word const>
   {
      static void Set(Memory *mem, Word idx, Word val) {mem->setW(idx, val);}

      static void Set(Word *mem, Word val) {*mem = val;}
   };

   //
   // MemInfo<MemPtr>
   //
   template<typename T>
   struct MemInfo<MemPtr<T> const>
   {
      static Word Get(Memory *mem, Word idx) {return MemPtr<T>{mem, mem->getW(idx)};}

      static Word Get(Word const *mem) {return MemPtr<T>{mem, *mem};}

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step;
   };

   template<typename T>
   struct MemInfo<MemPtr<T>> : MemInfo<MemPtr<T> const>
   {
      static void Set(Memory *mem, Word idx, Word val) {mem->setW(idx, static_cast<Word>(val));}

      static void Set(Word *mem, Word val) {*mem = static_cast<Word>(val);}
   };

   //
   // MemRef
   //
   template<typename T>
   class MemRef
   {
   public:
      MemRef(Memory *mem_, Word idx_) : mem{mem_}, idx{idx_} {}

      operator T () const {return MemInfo<T>::Get(mem, idx);}

      MemRef &operator = (MemRef const &) = delete;
      MemRef const &operator = (T val) const
         {MemInfo<T>::Set(mem, idx, val); return *this;}

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
         mem{mem_}, idx{idx_ >> MemInfo<T>::Shift} {}
      MemPtr(Memory &mem_, Word idx_ = 0) :
         mem{&mem_}, idx{idx_ >> MemInfo<T>::Shift} {}

      explicit operator bool () const {return idx != 0;}
      explicit operator Word () const {return idx << MemInfo<T>::Shift;}

      MemPtr &operator ++ () {idx += MemInfo<T>::Step; return *this;}
      MemPtr  operator ++ (int) {auto p = *this; return ++*this, p;}
      MemPtr &operator -- () {idx += MemInfo<T>::Step; return *this;}
      MemPtr  operator -- (int) {auto p = *this; return --*this, p;}

      MemRef<T> operator * () const {return {mem, idx};}

      MemPtr operator + (Word off) const {return {mem, idx + off * MemInfo<T>::Step};}
      Word operator - (MemPtr const &r) const {return (idx - r.idx) / MemInfo<T>::Step;}
      MemPtr operator - (Word off) const {return {mem, idx - off * MemInfo<T>::Step};}

      bool operator == (MemPtr const &r) const {return idx == r.idx;}
      bool operator != (MemPtr const &r) const {return idx != r.idx;}

      MemPtr &operator = (MemPtr const &) = default;
      MemPtr &operator = (Word idx_)
         {idx = idx_ >> MemInfo<T>::Shift; return *this;}

      MemPtr &operator += (Word off) {idx += off * MemInfo<T>::Step; return *this;}
      MemPtr *operator -= (Word off) {idx -= off * MemInfo<T>::Step; return *this;}

   private:
      Memory *mem;
      Word    idx;
   };
}

namespace std
{
   //
   // iterator_traits<::DGE::Code::MemPtr>
   //
   template<typename T>
   struct iterator_traits<::DGE::Code::MemPtr<T>>
   {
      using difference_type   = typename iterator_traits<T *>::difference_type;
      using value_type        = T;
      using pointer           = T *;
      using reference         = T &;
      using iterator_category = typename iterator_traits<T *>::iterator_category;
   };
}

#endif//DGE__Code__MemPtr_H__

