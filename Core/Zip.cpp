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

#include "Core/Zip.hpp"
#include "Core/BitStream.hpp"

#include <GDCC/Core/StringBuf.hpp>

#include <cstring>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Core
{
   static constexpr int FixedLitCodes = 288;
   static constexpr int MaxBits       = 15;
   static constexpr int MaxLitCodes   = 286;
   static constexpr int MaxDstCodes   = 30;

   static struct StaticHuffmanTables
   {
      StaticHuffmanTables() :
         tableLen{FixedLitCodes},
         tableDst{MaxDstCodes}
      {
         unsigned buffer[FixedLitCodes];

         // Don't ask.
         std::size_t i = 0;
         while(i < 144)           buffer[i++] = 8;
         while(i < 256)           buffer[i++] = 9;
         while(i < 280)           buffer[i++] = 7;
         while(i < FixedLitCodes) buffer[i++] = 8;

         tableLen.construct(buffer, FixedLitCodes);

         for(i = 0; i < MaxDstCodes;) buffer[i++] = 5;

         tableDst.construct(buffer, MaxDstCodes);
      }

      HuffmanTable tableLen;
      HuffmanTable tableDst;
   } Tables;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // HuffmanTable::construct
   //
   void HuffmanTable::construct(unsigned const *lengths, int num)
   {
      for(int i = 0; i < MaxBits; i++)
         nsymbols[i] = 0;

      for(int i = 0; i < num; i++)
         nsymbols[lengths[i]]++;

      // incomplete so we ignore this block
      if(nsymbols[0] == num)
         return;

      int offsets[MaxBits + 1] = {};
      for(int i = 1; i < MaxBits; i++)
         offsets[i + 1] = offsets[i] + nsymbols[i];

      for(int i = 0; i < num; i++)
         if(lengths[i] != 0)
            symbols[offsets[lengths[i]]++] = i;
   }

   //
   // HuffmanTable::decode
   //
   unsigned HuffmanTable::decode(BitStreamLE &in) const
   {
      int code  = 0;
      int first = 0;
      int index = 0;

      for(unsigned i = 1; i <= MaxBits; i++)
      {
         code |= in.get();

         int count = nsymbols[i];

         if(code - count < first)
            return symbols[index + (code - first)];

         index  += count;
         first  += count;
         first <<= 1;
         code  <<= 1;
      }

      throw ZipError("not enough codes");
   }

   //
   // Zip::loadHeader_Gzip
   //
   Zip::GzipHeader Zip::loadHeader_Gzip()
   {
      static constexpr std::uint32_t ftext    = 0b00001;
      static constexpr std::uint32_t fhcrc    = 0b00010;
      static constexpr std::uint32_t fextra   = 0b00100;
      static constexpr std::uint32_t fname    = 0b01000;
      static constexpr std::uint32_t fcomment = 0b10000;

      GzipHeader ret;

      if(size < 10)
         throw ZipError("not enough data in gzip header");

      size -= 10;

      if(data[0] != '\037' || data[1] != '\213')
         throw ZipError("invalid identification in gzip header");

      data += 2;

      if(*data++ != 8)
         throw ZipError("unknown compression method");

      Byte flg = *data++;
      ret.text = flg & ftext;

      for(int i = 0; i < 4; i++)
         ret.mtime |= *data++ << ((4 - i) * 8);

      Byte xfl = *data++;

      if(xfl == 2) ret.compressionLevel = FLevel_Maximum;
      if(xfl == 4) ret.compressionLevel = FLevel_Fastest;

      ret.os = static_cast<GzipOS>(*data++);

      if(flg & fextra)
      {
         // TODO: parse extra data
         std::uint16_t xlen = data[0] | (data[1] << 8);
         data += xlen + 2;
         size -= xlen + 2;
      }

      if(flg & fname)
      {
         ret.origName = data; // let's just assume this is OK
         data += ret.origName.size() + 1;
         size -= ret.origName.size() + 1;
      }

      if(flg & fcomment)
      {
         ret.comment = data;
         data += ret.comment.size() + 1;
         size -= ret.comment.size() + 1;
      }

      if(flg & fhcrc)
      {
         // TODO: use the CRC16 for checking header validity
         data += 2;
         size -= 2;
      }

      return ret;
   }

   //
   // Zip::loadHeader_Zlib
   //
   Zip::ZlibHeader Zip::loadHeader_Zlib()
   {
      ZlibHeader ret;

      if(size < 2)
         throw ZipError("not enough data in zlib header");

      size -= 2;

      unsigned   cmf   = data[0];
      Byte       cm    = (cmf & 0x0F);
      Byte       cinfo = (cmf & 0xF0) >> 4;

      if(cm != 8)
         throw ZipError("unknown compression method");

      if(cinfo > 7)
         throw ZipError("LZ77 window size logarithm invalid");

      Byte     flg    = data[1];
      unsigned fcheck = (cmf * 256) + flg;
      unsigned flevel = (flg & 0b11000000) >> 6;

      if((fcheck % 31) != 0)
         throw ZipError("FCHECK is incorrect");

      data += 2;

      if(flg & 0x20)
      {
         if(size < 4)
            throw ZipError("not enough data in zlib header");

         size -= 4;

         for(int i = 0; i < 4; i++)
            ret.dictID = (ret.dictID << 8) | *data++;
      }

      ret.windowSize = 1u << (cinfo + 8);
      ret.compressionLevel = FLevel(flevel);

      return ret;
   }

   //
   // Zip::loadStream
   //
   void Zip::loadStream(Byte *out)
   {
      GDCC::Core::StringBuf buf{data, size};
      BitStreamLE           in {buf};

      unsigned bfinal;
      unsigned btype;

      do
      {
         bfinal = in.get();
         btype  = in.get(2);

              if(btype == 0b10) streamHuffmanDynamic(out, in);
         else if(btype == 0b00) streamLiteral(out, in);
         else if(btype == 0b01) streamHuffmanStatic(out, in);
         else throw ZipError("bad btype in DEFLATE stream");
      }
      while(!bfinal);
   }

   //
   // Zip::outputTables
   //
   void Zip::outputTables(Byte *&out, BitStreamLE &in, HuffmanTable const &len,
      HuffmanTable const &dst)
   {
      static constexpr unsigned lenbase[29] = {
         3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
         35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
      };

      static constexpr unsigned lenextra[29] = {
         0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
         3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
      };

      static constexpr unsigned dstbase[30] = {
         1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
         257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
         8193, 12289, 16385, 24577
      };

      static constexpr unsigned dstextra[30] = {
         0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
         7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
         12, 12, 13, 13
      };

      unsigned symbol;

      do
      {
         symbol = len.decode(in);

         if(symbol < 256)
            *out++ = static_cast<Byte>(symbol);
         else if(symbol > 256)
         {
            symbol -= 257;

            if(symbol >= 29)
               throw ZipError("invalid fixed code");

            unsigned length = lenbase[symbol] + in.get(lenextra[symbol]);
            unsigned dist;
            symbol = dst.decode(in);
            dist = dstbase[symbol] + in.get(dstextra[symbol]);

            for(; length--; ++out)
               *out = *(out - dist);
         }
      }
      while(symbol != 256);
   }

   //
   // Zip::streamHuffmanDynamic
   //
   void Zip::streamHuffmanDynamic(Byte *&out, BitStreamLE &in)
   {
      static unsigned const huffmanOrders = 19;
      static unsigned const huffmanOrdering[huffmanOrders] = {
         16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
      };

      unsigned hlit  = in.get(5) + 257;
      unsigned hdist = in.get(5) + 1;
      unsigned hclen = in.get(4) + 4;

      GDCC::Core::Array<unsigned> hlen(MaxLitCodes + MaxDstCodes);

      for(unsigned i = 0; i < hclen; i++)
         hlen[huffmanOrdering[i]] = in.get(3);

      for(unsigned i = hclen; i < huffmanOrders; i++)
         hlen[huffmanOrdering[i]] = 0;

      HuffmanTable tableLen{MaxLitCodes}, tableDst{MaxDstCodes};

      tableLen.construct(hlen.data(), huffmanOrders);

      for(unsigned i = 0; i < hlit + hdist;)
      {
         unsigned symbol = tableLen.decode(in);

         if(symbol < 16)
            hlen[i++] = symbol;
         else
         {
            unsigned len = 0;

            switch(symbol)
            {
            case 16:
               if(i == 0)
                  throw ZipError("no previous length");
               len = hlen[i - 1];
               symbol = 3 + in.get(2);
               break;
            case 17: symbol = 3  + in.get(3); break;
            case 18: symbol = 11 + in.get(7); break;
            }

            if(i + symbol > hlit + hdist)
               throw ZipError("too much data to process");

            while(symbol--)
               hlen[i++] = len;
         }
      }

      if(hlen[256] == 0)
         throw ZipError("block has no end");

      tableLen.construct(hlen.data(),        hlit);
      tableDst.construct(hlen.data() + hlit, hdist);

      outputTables(out, in, tableLen, tableDst);
   }

   //
   // Zip::streamHuffmanStatic
   //
   void Zip::streamHuffmanStatic(Byte *&out, BitStreamLE &in)
   {
      outputTables(out, in, Tables.tableLen, Tables.tableDst);
   }

   //
   // Zip::streamLiteral
   //
   void Zip::streamLiteral(Byte *&out, BitStreamLE &in)
   {
      unsigned len = in.getB(); len |= in.getB() << 8;

      // Skip complement.
      in.getB(); in.getB();

      while(len--)
         *out++ = in.getB();
   }
}

// EOF

