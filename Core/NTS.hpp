//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Null-Terminated String data handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__NTS_H__
#define DGE__Core__NTS_H__

#include <GDCC/Core/Array.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE
{
   namespace Core
   {
      //
      // NTSArray
      //
      class NTSArray
      {
      public:
         using iterator = GDCC::Core::Array<char const *>::const_iterator;


         NTSArray() : str0{""}, str1{""}, str2{""} {}
         NTSArray(char const *data, std::size_t size);

         iterator begin() const {return strs.begin();}
         iterator end()   const {return strs.end();}

         // Header information.
         char const *getFormat()  {return str0;}
         char const *getType()    {return str1;}
         char const *getVersion() {return str2;}

         std::size_t size() {return strs.size();}

      private:
         GDCC::Core::Array<char const *> strs;
         char const *str0, *str1, *str2;
      };

      //
      // NTSStream
      //
      // Transforms an NTSArray into a bounds-checked stream.
      //
      class NTSStream
      {
      public:
         NTSStream() : itr{nullptr}, end{nullptr} {}
         NTSStream(NTSArray const &arr) : itr(arr.begin()), end(arr.end()) {}

         void drop() {if(itr != end) ++itr;}

         // Drops next token if value matches.
         bool drop(char const *tok);

         // Throws GDCC::Core::ParseException if next token does not match.
         void expect(char const *tok);

         char const *get() {return itr != end ? *itr++ : "";}

         explicit operator bool () {return itr != end;}

         char const *peek() {return itr != end ? *itr : "";}

         // Returns true if token value is next.
         bool peek(char const *tok);

      private:
         NTSArray::iterator itr, end;
      };
   }
}

#endif//DGE__Core__NTS_H__

