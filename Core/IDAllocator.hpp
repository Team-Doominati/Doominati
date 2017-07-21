//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// ID allocation.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__IDAllocator_H__
#define DGE__Core__IDAllocator_H__

#include <algorithm>
#include <climits>
#include <cstddef>
#include <limits>
#include <new>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // IDAllocator
   //
   template<typename T, typename ID = std::size_t>
   class IDAllocator
   {
   public:
      IDAllocator(ID start = 0) :
         freeE{nullptr}, freeP{nullptr}, freeV{nullptr}, itemV{nullptr}
      {
         while(start--) alloc(nullptr);
      }

      ~IDAllocator() {::operator delete(itemV);}

      T *operator [] (ID id) {return itemV[id];}

      //
      // alloc
      //
      ID alloc(T *item)
      {
         if(freeP == freeV) grow();

         itemV[*--freeP] = item;

         return *freeP;
      }

      //
      // free
      //
      void free(ID id)
      {
         itemV[id] = nullptr;
         *freeP++  = id;
      }

   private:
      static constexpr std::size_t Size = sizeof(T *) + sizeof(ID);
      static constexpr std::size_t MaxSize =
         std::numeric_limits<ID>::max() / Size;


      //
      // grow
      //
      void grow()
      {
         std::size_t oldSize = freeE - freeV;

         if(oldSize == MaxSize)
            throw std::bad_alloc();

         std::size_t newSize = oldSize + std::min(MaxSize - oldSize, oldSize + 128);

         auto newItemV = static_cast<T **>(::operator new(newSize * Size));
         auto newFreeV = reinterpret_cast<ID *>(newItemV + newSize);

         std::copy(itemV, itemV + oldSize, newItemV);

         itemV = newItemV;
         freeE = (freeP = freeV = newFreeV) + newSize;

         for(ID id = newSize; id-- != oldSize;)
            itemV[*freeP++ = id] = nullptr;
      }

      ID *freeE;
      ID *freeP;
      ID *freeV;
      T **itemV;
   };
}

#endif//DGE__Core__IDAllocator_H__

