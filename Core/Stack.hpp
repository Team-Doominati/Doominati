//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Stack class.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Stack_H__
#define DGE__Core__Stack_H__

#include "Core/CompilerIntrinsics.hpp"

#include <climits>
#include <iterator>
#include <new>
#include <utility>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // Stack
   //
   // Stack container.
   //
   template<typename T>
   class Stack
   {
   public:
      using const_iterator         = T const *;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
      using iterator               = T *;
      using reverse_iterator       = std::reverse_iterator<iterator>;

      Stack() : stack{nullptr}, stkEnd{nullptr}, stkPtr{nullptr} {}
      ~Stack() {clear(); ::operator delete(stack);}

      // operator []
      DGE_ForceInlineMSVC
      T &operator [] (std::size_t idx) {return *(stkPtr - idx);}

      // begin
      T       *begin()       {return stack;}
      T const *begin() const {return stack;}

      // clear
      void clear()
      {
         if(std::is_trivially_destructible<T>::value)
            stkPtr = stack;
         else while(stkPtr != stack)
            (--stkPtr)->~T();
      }

      // data
      T       *data()       {return stack;}
      T const *data() const {return stack;}

      // drop
      void drop() {(--stkPtr)->~T();}
      void drop(std::size_t n)
      {
         if(std::is_trivially_destructible<T>::value)
            stkPtr -= n;
         else while(n--)
            (--stkPtr)->~T();
      }

      // empty
      bool empty() const {return stkPtr == stack;}

      // end
      DGE_ForceInlineMSVC T       *end()       {return stkPtr;}
      DGE_ForceInlineMSVC T const *end() const {return stkPtr;}

      // push
      void push(T const &value)
      {
         // FUCKING MSVC.
         if(std::is_trivially_assignable<T &, T const &>::value)
            *stkPtr++ = value;
         else
            new(stkPtr++) T(value);
      }

      void push(T &&value)
      {
         if(std::is_trivially_assignable<T &, T &&>::value)
            *stkPtr++ = std::move(value);
         else
            new(stkPtr++) T(std::move(value));
      }

      // rbegin
      const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
            reverse_iterator rbegin()       {return       reverse_iterator(end());}

      // rend
      const_reverse_iterator rend() const {return const_reverse_iterator(begin());}
            reverse_iterator rend()       {return       reverse_iterator(begin());}

      //
      // reserve
      //
      void reserve(std::size_t count)
      {
         if(static_cast<std::size_t>(stkEnd - stkPtr) >= count)
            return;

         // Save pointers as indexes.
         std::size_t idxEnd = stkEnd - stack;
         std::size_t idxPtr = stkPtr - stack;

         // Calculate new array size.
         if(SIZE_MAX / sizeof(T) - idxEnd < count * 2)
            throw std::bad_alloc();

         idxEnd += count * 2;

         // Allocate and initialize new array.
         T *stackNew = static_cast<T *>(::operator new(idxEnd * sizeof(T)));
         for(T *itrNew = stackNew, *itr = stack, *end = stkPtr; itr != end;)
            new(itrNew++) T(std::move(*itr++));

         // Destruct old array.
         for(T *itr = stkPtr, *end = stack; itr != end;)
            (--itr)->~T();

         // Free old array.
         ::operator delete(stack);

         // Restore pointers.
         stack  = stackNew;
         stkPtr = stack + idxPtr;
         stkEnd = stack + idxEnd;
      }

      // size
      std::size_t size() const {return stkPtr - stack;}

   private:
      T *stack;
      T *stkEnd;
      T *stkPtr;
   };
}

#endif//DGE__Core__Stack_H__

