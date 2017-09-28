//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Portable Network Graphics texture loading.
//
//-----------------------------------------------------------------------------

#include "GL/Texture.hpp"
#include "GL/Color.hpp"

#include "Core/Zip.hpp"

#include "FS/File.hpp"

#include <GDCC/Core/Array.hpp>

#include <cstring>


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::GL
{
   //
   // PaethPred
   //
   static inline Core::Byte PaethPred(Core::Byte a, Core::Byte b, Core::Byte c)
   {
      int p  = a + b - c;
      int pa = std::abs(p - a);
      int pb = std::abs(p - b);
      int pc = std::abs(p - c);

      if(pa <= pb && pa <= pc)
         return a;
      else if(pb <= pc)
         return b;
      else
         return c;
   }
}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // PNGColorType
   //
   enum class PNGColorType
   {
      None,
      Greyscale,
      Truecolor,
      IndexedColor,
      GreyscaleWithAlpha,
      TruecolorWithAlpha,
      Max
   };

   //
   // PNGChunk
   //
   struct PNGChunk
   {
      char const    *data;
      std::uint32_t  length;
      char           type[4];
      bool           fancillary  : 1;
      bool           fprivate    : 1;
//    bool           freserved   : 1;
      bool           fsafetocopy : 1;
   };

   //
   // TextureLoader_PNG
   //
   class TextureLoader_PNG : public TextureLoader
   {
   public:
      TextureLoader_PNG(FS::File *file_) :
         chunkArray{},

         dataLength{0},

         file{file_},

         width {0},
         height{0},

         paletteColors  {},
         backgroundColor{0.0f, 0.0f, 0.0f, 0.0f},

         bitdepth   {0},
         colorType  {PNGColorType::None},
//       compression{PNGCompression::ZlibMethod8},
//       filter     {PNGFilter::Type0},
         interlace  {false},

         bitMult  {0},
         pixelLen {0},
         pixelSize{0},
         scanline {0}
      {
      }

      bool chunkerIHDR(PNGChunk const &chunk);
      bool chunkerPLTE(PNGChunk const &chunk);

      bool chunkIter(bool (TextureLoader_PNG::*chunker)(PNGChunk const &));

      std::size_t countChunks(char const *data, std::size_t len);

      virtual void data(TexturePixel *buf);

      void unfilter(Core::Byte ftype, Core::Byte *out);

      void loadChunks(char const *data, std::size_t len);
      void loadHeader();

      void readChunks();
      std::uint32_t readU32_BE(char const *data);

      virtual std::pair<TextureDim, TextureDim> size() {return {width, height};}

   private:
      GDCC::Core::Array<PNGChunk> chunkArray;

      std::size_t dataLength;

      FS::File *file;

      std::uint32_t width;
      std::uint32_t height;

      GDCC::Core::Array<Color> paletteColors;
      Color                    backgroundColor;

      Core::Byte     bitdepth;
      PNGColorType   colorType;
//    PNGCompression compression;
//    PNGFilter      filter;
      bool           interlace;

      std::size_t bitMult;
      std::size_t pixelLen;
      std::size_t pixelSize;
      std::size_t scanline;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // TextureLoader_PNG::chunkerIHDR
   //
   bool TextureLoader_PNG::chunkerIHDR(PNGChunk const &chunk)
   {
      if(std::memcmp(chunk.type, "IHDR", 4)) return false;

      if(chunk.length < 13)
         throw TextureLoaderError{"IHDR has invalid size"};

      std::size_t iter = 0;

      width  = readU32_BE(chunk.data + iter); iter += 4;
      height = readU32_BE(chunk.data + iter); iter += 4;

      bitdepth = chunk.data[iter++];

      switch(chunk.data[iter++])
      {
      case 0:
         if(bitdepth != 8 && bitdepth != 16 && bitdepth != 4 &&
            bitdepth != 2 && bitdepth != 1)

            throw TextureLoaderError("invalid bit depth for greyscale");
         colorType = PNGColorType::Greyscale;
         pixelLen = 1;
         break;
      case 2:
         if(bitdepth != 8 && bitdepth != 16)
            throw TextureLoaderError("invalid bit depth for truecolor");

         colorType = PNGColorType::Truecolor;
         pixelLen = 3;
         break;
      case 3:
         if(bitdepth != 8 && bitdepth != 2 && bitdepth != 4 &&
            bitdepth != 1)
            throw TextureLoaderError("invalid bit depth for indexed color");

         colorType = PNGColorType::IndexedColor;
         pixelLen = 1;
         break;
      case 4:
         if(bitdepth != 8 && bitdepth != 16)
            throw TextureLoaderError("invalid bit depth for greyscale");

         colorType = PNGColorType::GreyscaleWithAlpha;
         pixelLen = 2;
         break;
      case 6:
         if(bitdepth != 8 && bitdepth != 16)
            throw TextureLoaderError("invalid bit depth for truecolor");

         colorType = PNGColorType::TruecolorWithAlpha;
         pixelLen = 4;
         break;
      }

      if(bitdepth <= 8)
         bitMult = 1;
      else
         bitMult = bitdepth / 8;

      pixelSize = pixelLen * bitMult;
      scanline = 1 + (width * pixelSize);

      if(chunk.data[iter++] != 0)
         throw TextureLoaderError("unsupported compression method");
      if(chunk.data[iter++] != 0)
         throw TextureLoaderError("unsupported filter type");

      interlace = (chunk.data[iter++] != 0);
      if(interlace)
         throw TextureLoaderError("interlacing not supported");

      return true;
   }

   //
   // TextureLoader_PNG::chunkerPLTE
   //
   bool TextureLoader_PNG::chunkerPLTE(PNGChunk const &chunk)
   {
      if(std::memcmp(chunk.type, "PLTE", 4)) return false;

      if(paletteColors.data())
         throw TextureLoaderError("multiple PLTE chunks");

      if(colorType == PNGColorType::Greyscale ||
         colorType == PNGColorType::GreyscaleWithAlpha)
         throw TextureLoaderError("invalid color type with PLTE chunk");

      if((chunk.length % 3) != 0)
         throw TextureLoaderError("PLTE size is wrong");

      auto        data   = reinterpret_cast<Core::Byte const *>(chunk.data);
      std::size_t length = chunk.length / 3;

      paletteColors = GDCC::Core::Array<Color>(length);

      for(std::size_t i = 0; i < length; i++)
         paletteColors[i] = {data[(i * 3) + 0] / 255.0f,
                             data[(i * 3) + 1] / 255.0f,
                             data[(i * 3) + 2] / 255.0f,
                             1.0f};

      return true;
   }

   //
   // TextureLoader_PNG::chunkIter
   //
   bool TextureLoader_PNG::chunkIter(bool (TextureLoader_PNG::*chunker)(PNGChunk const &))
   {
      for(auto chunk : chunkArray)
         if((this->*chunker)(chunk))
            return true;

      return false;
   }

   //
   // TextureLoader_PNG::countChunks
   //
   std::size_t TextureLoader_PNG::countChunks(char const *data, std::size_t len)
   {
      std::size_t chunks = 0;

      for(std::size_t iter = 0; iter < len; chunks++)
      {
         if(len - iter < 8)
            throw TextureLoaderError("not enough data for chunk");

         iter += 8 + readU32_BE(data + iter) + 4;
      }

      return chunks;
   }

   //
   // TextureLoader_PNG::data
   //
   void TextureLoader_PNG::data(TexturePixel *buf)
   {
      // Get the input size and make a buffer.
      std::size_t insize = 0;
      for(auto chunk : chunkArray)
         if(!std::memcmp(chunk.type, "IDAT", 4))
            insize += chunk.length;

      GDCC::Core::Array<char> inData(insize);

      // Concatenate IDAT chunks into the buffer.
      std::size_t incursor = 0;
      for(auto chunk : chunkArray)
      {
         if(!std::memcmp(chunk.type, "IDAT", 4))
         {
            std::memmove(&inData[incursor], chunk.data, chunk.length);
            incursor += chunk.length;
         }
      }

      // Allocate the output data buffer.
      GDCC::Core::Array<Core::Byte> outData(scanline * height);

      // Load the DEFLATE data.
      Core::Zip zip{inData.data(), inData.size()};

      try
      {
         zip.loadHeader_Zlib();
         zip.loadStream(outData.data());
      }
      catch(Core::ZipError &exc)
      {
         throw TextureLoaderError(exc.what());
      }

      if(outData[0] > 1)
         throw TextureLoaderError("first scanline has a lookback filter");


      // Unfilter scanlines.
      for(std::size_t y = 0; y < height; y++)
         unfilter(outData[scanline * y], &outData[scanline * y] + 1);

      // Read pixels.
      float depth = (1 << bitdepth) - 1.0f;
      auto bufp = outData.data() + 1;
      for(std::size_t y = 0; y < height; y++, bufp++)
         for(std::size_t x = 0; x < width; x++)
      {
         auto pixel = &buf[x + y * width];

         switch(colorType)
         {
         case PNGColorType::Truecolor:
            (*pixel)[3] = 1.0f;
            [[fallthrough]];
         case PNGColorType::TruecolorWithAlpha:
            for(unsigned i = 0; i < pixelLen; i++)
            {
               unsigned num = 0;

               for(std::size_t b = 0; b < bitMult; b++)
                  num = (num << 8) | *bufp++;

               (*pixel)[i] = num / depth;
            }
            break;
         default: throw TextureLoaderError("unsupported color type");
         }
      }
   }

   //
   // TextureLoader_PNG::unfilter
   //
   void TextureLoader_PNG::unfilter(Core::Byte ftype, Core::Byte *out)
   {
      enum FilterType
      {
         None,    // = Filt(χ)
         Sub,     // = Filt(χ) + Recon(α)
         Up,      // = Filt(χ) + Recon(β)
         Average, // = Filt(χ) + floor((Recon(α) + Recon(β)) / 2)
         Paeth    // = Filt(χ) + PaethPredictor(Recon(α), Recon(β), Recon(γ))
      };

      auto ox = out;
      auto oa = out - pixelSize;
      auto ob = out - scanline;
      auto oc = oa  - scanline;

      std::size_t i = 0;

      switch(ftype)
      {
      case None:
         break;

      case Sub:
         for(i = pixelSize; i < scanline - 1; i++)
            ox[i] += oa[i];
         break;

      case Up:
         for(; i < scanline - 1; i++)
            ox[i] += ob[i];
         break;

      case Average:
         for(; i < pixelSize; i++)
            ox[i] += (0     + ob[i]) / 2;
         for(; i < scanline - 1; i++)
            ox[i] += (oa[i] + ob[i]) / 2;
         break;

      case Paeth:
         for(; i < pixelSize; i++)
            ox[i] += PaethPred(0,     ob[i], 0);
         for(; i < scanline - 1; i++)
            ox[i] += PaethPred(oa[i], ob[i], oc[i]);
         break;
      }
   }

   //
   // TextureLoader_PNG::loadChunks
   //
   void TextureLoader_PNG::loadChunks(char const *data, std::size_t len)
   {
      std::size_t chunks = countChunks(data, len);

      if(chunks < 3)
         throw TextureLoaderError{"not enough chunks in file"};

      chunkArray = GDCC::Core::Array<PNGChunk>(chunks);

      std::size_t iter = 0;
      for(auto it = chunkArray.begin(); it != chunkArray.end(); ++it)
      {
         PNGChunk chunk;

         chunk.data   = data + iter + 8;
         chunk.length = readU32_BE(data + iter);
         std::memcpy(chunk.type, &data[iter + 4], 4);
         chunk.fancillary  = !(chunk.type[0] & 0x20);
         chunk.fprivate    =   chunk.type[1] & 0x20 ;
//       chunk.freserved   =   chunk.type[2] & 0x20 ;
         chunk.fsafetocopy = !(chunk.type[3] & 0x20);

         iter += 8;
         iter += chunk.length;
         *it   = std::move(chunk);
         iter += 4;
      }
   }

   //
   // TextureLoader_PNG::loadHeader
   //
   void TextureLoader_PNG::loadHeader()
   {
      loadChunks(file->data + 8, file->size - 8);

      if(std::memcmp(chunkArray.front().type, "IHDR", 4))
         throw TextureLoaderError("IHDR isn't first chunk");
      if(std::memcmp(chunkArray.back().type,  "IEND", 4))
         throw TextureLoaderError("IEND isn't last chunk");

      chunkerIHDR(chunkArray.front());
      chunkIter(&TextureLoader_PNG::chunkerPLTE);

      if(colorType == PNGColorType::IndexedColor && !paletteColors.data())
         throw TextureLoaderError("no palette found with indexed color");
   }

   //
   // TextureLoader_PNG::readU32_BE
   //
   std::uint32_t TextureLoader_PNG::readU32_BE(char const *data)
   {
      auto udata = reinterpret_cast<Core::Byte const *>(data);
      return (udata[0] << 24) | (udata[1] << 16) | (udata[2] << 8) | udata[3];
   }

   //
   // CreateTextureLoader_PNG
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader_PNG(FS::File *file)
   {
      std::unique_ptr<TextureLoader_PNG> loader{new TextureLoader_PNG{file}};
      loader->loadHeader();
      return std::unique_ptr<TextureLoader>{loader.release()};
   }
}

// EOF

