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

      void construct(unsigned const *lengths, std::size_t num);
      unsigned decode(BitStreamLE &in) const;

      GDCC::Core::Array<unsigned> nsymbols;
      GDCC::Core::Array<unsigned> symbols;
   };

   //
   // ZipError
   //
   class ZipError : public std::runtime_error
   {
      using std::runtime_error::runtime_error;
   };

   //
   // Zip
   //
   class Zip
   {
   public:
      enum FLevel
      {
         FLevel_Fastest,
         FLevel_Fast,
         FLevel_Default,
         FLevel_Maximum
      };

      enum GzipOS
      {
         GzipOS_FAT,
         GzipOS_Amiga,
         GzipOS_VMS,
         GzipOS_Unix,
         GzipOS_VMCMS,
         GzipOS_TOS,
         GzipOS_HPFS,
         GzipOS_Macintosh,
         GzipOS_ZSystem,
         GzipOS_CPM,
         GzipOS_TOPS20,
         GzipOS_NTFS,
         GzipOS_QDOS,
         GzipOS_RISCOS,
         GzipOS_Unknown = 255
      };

      struct ZlibHeader
      {
         unsigned      windowSize       = 0;
         FLevel        compressionLevel = FLevel_Fastest;
         std::uint32_t dictID           = 0;
//       std::uint32_t adler32          = 0;
      };

//    struct GzipSubfield
//    {
//       std::uint16_t           id;
//       GDCC::Core::Array<Byte> data;
//    };

      struct GzipHeader
      {
         bool          text             = false;
         std::uint32_t mtime            = 0;
         FLevel        compressionLevel = FLevel_Default;
         GzipOS        os               = GzipOS_Unknown;
//       std::uint32_t crc32            = 0;
//       std::uint32_t inputSize        = 0;

//       GDCC::Core::Array<GzipSubfield> extraData;

         std::string origName;
         std::string comment;
      };

      Zip(Zip const &) = delete;
      Zip(char const *data_, std::size_t size_) :
         data{data_},
         size{size_}
      {
      }

      GzipHeader loadHeader_Gzip();
      ZlibHeader loadHeader_Zlib();
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

