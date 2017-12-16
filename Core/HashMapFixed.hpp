//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// HashMapFixed class.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__HashMapFixed_H__
#define DGE__Core__HashMapFixed_H__

#include "../Core/Types.hpp"

#include <functional>
#include <new>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // HashMapFixed
   //
   // Non-resizable hash map.
   // Also acts as an array for element access by index.
   //
   template<typename Key, typename T, typename Hash>
   class HashMapFixed
   {
   public:
      struct Elem
      {
         Key   key;
         T     val;
         Elem *next;
      };

      using const_iterator = Elem const *;
      using iterator       = Elem *;
      using size_type      = std::size_t;
      using value_type     = Elem;


      HashMapFixed() : hasher{}, table{nullptr}, elemV{nullptr}, elemC{0} {}
      HashMapFixed(HashMapFixed const &) = delete;

      //
      // Move constructor
      //
      HashMapFixed(HashMapFixed &&m) :
         hasher{std::move(m.hasher)},
         table {m.table},
         elemV {m.elemV},
         elemC {m.elemC}
      {
         m.table = nullptr;
         m.elemV = nullptr;
         m.elemC = 0;
      }

      //
      // constructor
      //
      template<typename Ctor>
      HashMapFixed(size_type count, Ctor ctor) :
         hasher{}, table{nullptr}, elemV{nullptr}, elemC{0}
      {
         reset(count, std::move(ctor));
      }

      //
      // constructor
      //
      HashMapFixed(std::initializer_list<std::pair<Key, T>> init) :
         hasher{}, table{nullptr}, elemV{nullptr}, elemC{0}
      {
         auto itr = init.begin();
         reset(init.size(),[&itr](Elem *elem)
            {new(elem) Elem{itr->first, itr->second, nullptr}; ++itr;});
      }

      ~HashMapFixed() {reset();}

      // operator [ std::size_t ]
      T       &operator [] (std::size_t i)       {return elemV[i].val;}
      T const &operator [] (std::size_t i) const {return elemV[i].val;}

      // begin
      iterator       begin()       {return elemV;}
      const_iterator begin() const {return elemV;}

      // empty
      bool empty() const {return !elemC;}

      // end
      iterator       end()       {return elemV + elemC;}
      const_iterator end() const {return elemV + elemC;}

      //
      // find
      //
      T *find(Key const &key) const
      {
         if(auto elem = findItr(key))
            return &elem->val;

         return nullptr;
      }

      //
      // findItr
      //
      Elem *findItr(Key const &key) const
      {
         if(!table) return nullptr;

         for(Elem *elem = table[hasher(key) % elemC]; elem; elem = elem->next)
         {
            if(elem->key == key)
               return elem;
         }

         return nullptr;
      }

      //
      // findVal
      //
      T findVal(Key const &key) const
      {
         if(auto elem = findItr(key))
            return elem->val;

         return T{};
      }

      //
      // reset
      //
      void reset()
      {
         for(Elem *elem = elemV + elemC; elem != elemV;)
            (--elem)->~Elem();

         table = nullptr;

         free();
      }

      //
      // reset
      //
      template<typename Ctor>
      void reset(size_type count, Ctor ctor)
      {
         reset();

         alloc(count);

         Elem *elem = elemV, *elemEnd = elemV + elemC;
         try
         {
            while(elem != elemEnd)
               ctor(elem++);
         }
         catch(...)
         {
            while(elem != elemV)
               (--elem)->~Elem();
            free();

            throw;
         }

         build();
      }

      // size
      size_type size() const {return elemC;}

   private:
      //
      // alloc
      //
      void alloc(size_type count)
      {
         if(!count) return;

         size_type sizeRaw = sizeof(Elem) * count + sizeof(Elem *) * count;

         elemC = count;
         elemV = static_cast<Elem *>(::operator new(sizeRaw));
      }

      //
      // build
      //
      void build()
      {
         // Initialize table.
         table = reinterpret_cast<Elem **>(elemV + elemC);
         for(Elem **elem = table + elemC; elem != table;)
            *--elem = nullptr;

         // Insert elements.
         for(Elem &elem : *this)
         {
            size_type hash = hasher(elem.key) % elemC;

            elem.next = table[hash];
            table[hash] = &elem;
         }
      }

      //
      // free
      //
      void free()
      {
         ::operator delete(elemV);

         elemV = nullptr;
         elemC = 0;
      }

      Hash hasher;

      Elem    **table;
      Elem     *elemV;
      size_type elemC;
   };
}

#endif//DGE__Core__HashMapFixed_H__

