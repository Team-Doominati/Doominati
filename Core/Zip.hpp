//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// DEFLATE-centric data processing.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Zip_H__
#define DGE__Core__Zip_H__

#include "Types.hpp"

#include <GDCC/Core/Array.hpp>

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   class BitStreamLE;

   //
   // HuffmanTable
   //
   class HuffmanTable
   {
   public:
      explicit HuffmanTable(std::size_t size) :
         nsymbols(16),
         symbols(size)
      {
         nsymbols.fill(0);
         symbols.fill(0);
      }

      void construct(unsigned const *lengths, int num);
      unsigned decode(BitStreamLE &in) const;

      GDCC::Core::Array<int>      nsymbols;
      GDCC::Core::Array<unsigned> symbols;
   };

   //
   // ZipError
   //
   class ZipError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };

   //
   // Zip
   //
   class Zip
   {
   public:
      Zip(Zip const &) = delete;
      Zip(char const *data_, std::size_t size_) :
         data{data_},
         size{size_}
      {
      }

      void loadHeader_Gzip();
      void loadHeader_Zlib();
      void loadStream(Byte *out);

   private:
      void outputTables(Byte *&out, BitStreamLE &in, HuffmanTable const &len,
         HuffmanTable const &dst);

      void streamHuffmanDynamic(Byte *&out, BitStreamLE &in);
      void streamHuffmanStatic(Byte *&out, BitStreamLE &in);
      void streamLiteral(Byte *&out, BitStreamLE &in);

      char const *data;
      std::size_t size;
   };
}

#endif//DGE__Core__Zip_H__

