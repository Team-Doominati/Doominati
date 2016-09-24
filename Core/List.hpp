//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Linked list handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__List_H__
#define DGE__Core__List_H__

#include <cstddef>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // ListLink
   //
   template<typename T>
   class ListLink
   {
   private:
      //
      // IteratorBase
      //
      template<typename Obj>
      class IteratorBase
      {
      public:
         IteratorBase<Obj> &operator ++ () {link = link->next; return *this;}
         IteratorBase<Obj> operator ++ (int) {auto i = *this; ++*this; return i;}

         Obj &operator * () const {return *link->obj;}
         Obj *operator -> () const {return link->obj;}

         bool operator == (IteratorBase<Obj> const &iter) const
            {return iter.link == link;}
         bool operator != (IteratorBase<Obj> const &iter) const
            {return iter.link != link;}


         friend class ListLink;

      private:
         IteratorBase(ListLink<T> const *link_) : link{link_} {}

         ListLink<T> const *link;
      };

   public:
      ListLink() : obj{nullptr}, prev{this}, next{this} {}
      ListLink(ListLink<T> const &) = delete;
      ListLink(T *obj_) : obj{obj_}, prev{this}, next{this} {}
      ListLink(T *obj_, ListLink<T> &&link) :
         obj{obj_}, prev{link.prev}, next{link.next}
         {prev->next = next->prev = this; link.prev = link.next = &link;}
      ~ListLink() {unlink();}

      // begin
      IteratorBase<T>       begin()       {return next;}
      IteratorBase<T const> begin() const {return next;}

      // empty
      bool empty() const {return next == this;}

      // end
      IteratorBase<T>       end()       {return this;}
      IteratorBase<T const> end() const {return this;}

      //
      // insert
      //
      void insert(ListLink<T> *head)
      {
         (prev = head->prev)->next = this;
         (next = head      )->prev = this;
      }

      void relink(ListLink<T> *head) {unlink(); insert(head);}

      //
      // size
      //
      std::size_t size() const
      {
         std::size_t count = 0;
         for(auto const &o : *this) (void)o, ++count;
         return count;
      }

      //
      // unlink
      //
      void unlink()
      {
         prev->next = next;
         next->prev = prev;

         prev = next = this;
      }

      T *const obj;
      ListLink<T> *prev, *next;
   };
}

#endif//DGE__Core__List_H__

