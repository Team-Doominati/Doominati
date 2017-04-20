//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Linked list vector handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__ListLinkVector_H__
#define DGE__Core__ListLinkVector_H__

#include "Core/List.hpp"

#include <new>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // ListLinkVector
   //
   template<typename T>
   class ListLinkVector
   {
   public:
      using Link = ListLink<T>;

      ListLinkVector(ListLinkVector const &) = delete;
      ListLinkVector(ListLinkVector &&) = delete; // TODO
      ListLinkVector(T *obj_) : obj{obj_},
         allocEnd{nullptr}, links{nullptr}, usedEnd{nullptr} {}
      ~ListLinkVector() {clear(); ::operator delete(links);}

      ListLinkVector &operator = (ListLinkVector const &) = delete;
      ListLinkVector &operator = (ListLinkVector &&) = delete; // TODO

      // begin
      Link *begin() {return links;}

      std::size_t capacity() const {return allocEnd - links;}

      // clear
      void clear() {for(auto &link : *this) link.~Link(); usedEnd = links;}

      // end
      Link *end() {return usedEnd;}

      void resize(std::size_t sizeNew);

      // size
      std::size_t size() const {return usedEnd - links;}

      // unlink
      void unlink() {for(auto &link : *this) link.unlink();}

      T *const obj;

   private:
      Link *allocEnd;
      Link *links;
      Link *usedEnd;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // ListLinkVector::resize
   //
   template<typename T>
   void ListLinkVector<T>::resize(std::size_t sizeNew)
   {
      auto allocOld = capacity();
      auto sizeOld  = size();

      // Expand underlying storage?
      if(sizeNew > allocOld)
      {
         auto allocNew = allocOld + allocOld / 2 + sizeNew;

         auto linksOld = links;
         auto linksNew = static_cast<Link *>(::operator new(allocNew * sizeof(Link)));

         for(auto itrOld = linksOld, endOld = itrOld + sizeOld, itrNew = linksNew; itrOld != endOld;)
            new(itrNew++) Link{obj, std::move(*itrOld++)};

         ::operator delete(linksOld);

         links    = linksNew;
         allocEnd = links + allocNew;
         usedEnd  = links + sizeOld;
      }

      // Destruct/construct elements.
      if(sizeNew <= sizeOld)
         for(; sizeOld != sizeNew; --sizeOld)
            (--usedEnd)->~Link();
      else
         for(; sizeOld != sizeNew; ++sizeOld)
            new(usedEnd++) Link{obj};
   }
}

#endif//DGE__Core__ListLinkVector_H__

