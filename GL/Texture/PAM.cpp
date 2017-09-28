//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// PAM texture loading.
//
//-----------------------------------------------------------------------------

#include "GL/Texture.hpp"

#include "FS/File.hpp"

#include <cstring>
#include <cctype>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // TextureLoader_PAM
   //
   class TextureLoader_PAM : public TextureLoader
   {
   public:
      TextureLoader_PAM(FS::File *file_) :
         file  {file_},

         pixelBegin{nullptr},
         pixelEnd  {nullptr},
         pixelItr  {nullptr},

         maxval{0.0f},
         bitpos{0},
         depth {0},
         height{0},
         width {0},

         bits  {false},
         plain {false}
      {
      }

      virtual void data(TexturePixel *buf);

      void endValueRow();

      void loadHeaderPAM();
      void loadHeaderPNM();

      char readHeaderChar();

      unsigned int readHeaderInt();

      char const *readHeaderLine();

      void readHeaderTUPLTYPE(char const *tupltype);

      float readValue();

      virtual std::pair<TextureDim, TextureDim> size() {return {width, height};}

   private:
      FS::File *file;

      char const *pixelBegin, *pixelEnd, *pixelItr;

      float maxval;
      unsigned bitpos;
      unsigned depth;
      unsigned height;
      unsigned width;

      bool bits  : 1;
      bool plain : 1;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // TextureLoader_PAM::endValueRow
   //
   void TextureLoader_PAM::endValueRow()
   {
      if(bits)
         bitpos = 0;
   }

   //
   // TextureLoader_PAM::data
   //
   void TextureLoader_PAM::data(TexturePixel *buf)
   {
      pixelItr = pixelBegin;

      for(unsigned int y = 0; y != height; ++y, endValueRow())
         for(unsigned int x = 0; x != width; ++x)
      {
         TexturePixel *pixel = &buf[x + y * width];

         switch(depth)
         {
         case 1:
            (*pixel)[0] = (*pixel)[1] = (*pixel)[2] = readValue();
            (*pixel)[3] = 1.0f;
            break;

         case 2:
            (*pixel)[0] = (*pixel)[1] = (*pixel)[2] = readValue();
            (*pixel)[3] = readValue();
            break;

         case 3:
            (*pixel)[0] = readValue();
            (*pixel)[1] = readValue();
            (*pixel)[2] = readValue();
            (*pixel)[3] = 1.0f;
            break;

         case 4:
            (*pixel)[0] = readValue();
            (*pixel)[1] = readValue();
            (*pixel)[2] = readValue();
            (*pixel)[3] = readValue();
            break;
         }
      }
   }

   //
   // TextureLoader_PAM::loadHeaderPAM
   //
   void TextureLoader_PAM::loadHeaderPAM()
   {
      pixelEnd = (pixelItr = pixelBegin = file->data) + file->size;

      while(char const *head = readHeaderLine())
      {
         if(!std::memcmp(head, "ENDHDR", 6))
            return;

         else if(!std::memcmp(head, "HEIGHT", 6))
            height = std::strtoul(head + 6, nullptr, 10);

         else if(!std::memcmp(head, "WIDTH", 5))
            width = std::strtoul(head + 5, nullptr, 10);

         else if(!std::memcmp(head, "DEPTH", 5))
            depth = std::strtoul(head + 5, nullptr, 10);

         else if(!std::memcmp(head, "MAXVAL", 6))
            maxval = std::strtoul(head + 6, nullptr, 10);

         else if(!std::memcmp(head, "TUPLTYPE", 8))
            readHeaderTUPLTYPE(head + 8);

         // Extension: Interpret pixel data as plain-text.
         else if(!std::memcmp(head, "PLAIN", 5))
            plain = true;
      }

      throw TextureLoaderError("expected ENDHDR");
   }

   //
   // TextureLoader_PAM::loadHeaderPNM
   //
   void TextureLoader_PAM::loadHeaderPNM()
   {
      if(file->size < 2)
         throw TextureLoaderError("PNM too small");

      pixelEnd = (pixelItr = pixelBegin = file->data)++ + file->size;

      switch(*pixelItr++)
      {
      case '1': plain = true; [[fallthrough]];
      case '4':
         maxval = 1.0f;
         depth  = 1;
         bits   = true;
         break;

      case '2': plain = true; [[fallthrough]];
      case '5':
         depth = 1;
         break;

      case '3': plain = true; [[fallthrough]];
      case '6':
         depth = 3;
         break;

      default:
         throw TextureLoaderError("unknown PNM number");
      }

      width  = readHeaderInt();
      height = readHeaderInt();

      if(!bits) maxval = readHeaderInt();

      if(readHeaderChar() != '\n')
         throw TextureLoaderError("expected end of header");

      pixelBegin = pixelItr;
   }

   //
   // TextureLoader_PAM::readHeaderChar
   //
   char TextureLoader_PAM::readHeaderChar()
   {
      if(pixelItr == pixelEnd) return '\0';

      while(*pixelItr == '#')
      {
         while(*pixelItr++ != '\n') {if(pixelItr == pixelEnd) return '\0';}

         if(pixelItr == pixelEnd) return '\0';
      }

      return *pixelItr++;
   }

   //
   // TextureLoader_PAM::readHeaderInt
   //
   unsigned TextureLoader_PAM::readHeaderInt()
   {
      unsigned i = 0;
      char c;

      // Skip whitespace.
      while(std::isspace(c = readHeaderChar())) {}

      for(; std::isdigit(c); c = readHeaderChar())
         i = i * 10 + c - '0';

      --pixelItr;

      return i;
   }

   //
   // TextureLoader_PAM::readHeaderLine
   //
   char const *TextureLoader_PAM::readHeaderLine()
   {
      // Empty line.
      if(*pixelItr == '\n') return ++pixelItr, "";

      // Comment line.
      if(*pixelItr == '#')
      {
         while(*pixelItr++ != '\n') {if(pixelItr == pixelEnd) return nullptr;}
         return "";
      }

      while(std::isspace(*pixelItr)) {if(++pixelItr == pixelEnd) return nullptr;}

      char const *head = pixelItr;

      while(*pixelItr++ != '\n') {if(pixelItr == pixelEnd) return nullptr;}
      return head;
   }

   //
   // TextureLoader_PAM::readHeaderTUPLTYPE
   //
   void TextureLoader_PAM::readHeaderTUPLTYPE(char const *tupltype)
   {
      while(std::isspace(*tupltype) && *tupltype != '\n') ++tupltype;

      if(*tupltype == '\n') return;

      // PBM equivalent.
      if(!std::memcmp(tupltype, "BLACKANDWHITE\n", 14))
         depth = 1, maxval = 1.0f;
      else if(!std::memcmp(tupltype, "BLACKANDWHITE_ALPHA\n", 20))
         depth = 2, maxval = 1.0f;

      // PGM equivalent.
      else if(!std::memcmp(tupltype, "GRAYSCALE\n", 10))
         depth = 1;
      else if(!std::memcmp(tupltype, "GRAYSCALE_ALPHA\n", 16))
         depth = 2;

      // PPM equivalent.
      else if(!std::memcmp(tupltype, "RGB\n", 4))
         depth = 3;
      else if(!std::memcmp(tupltype, "RGB_ALPHA\n", 10))
         depth = 4;
   }

   //
   // TextureLoader_PAM::readValue
   //
   float TextureLoader_PAM::readValue()
   {
      unsigned value;

      if(pixelItr == pixelEnd) return 0.0f;

      if(plain)
      {
         while(std::isspace(*pixelItr)) {if(++pixelItr == pixelEnd) return 0.0f;}

         for(value = 0; pixelItr != pixelEnd && std::isdigit(*pixelItr);)
            value = value * 10 + *pixelItr++ - '0';

         return bits ? !value : value / maxval;
      }
      else if(bits)
      {
         if(!bitpos) {bitpos = 8; ++pixelItr;}

         return !((*pixelItr >> --bitpos) & 1);
      }
      else
      {
         value = static_cast<unsigned char>(*pixelItr++);

         if(maxval <= 255.0f)
            return value / maxval;

         if(pixelItr == pixelEnd) return 0.0f;

         value = (value << 8) + static_cast<unsigned char>(*pixelItr++);

         if(maxval <= 65535.0f)
            return value / maxval;

         if(pixelItr == pixelEnd) return 0.0f;

         value = (value << 8) + static_cast<unsigned char>(*pixelItr++);

         if(maxval <= 16777215.0f)
            return value / maxval;

         return ((value << 8) + static_cast<unsigned char>(*pixelItr++)) / maxval;
      }
   }

   //
   // CreateTextureLoader_PAM
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader_PAM(FS::File *file)
   {
      std::unique_ptr<TextureLoader_PAM> loader{new TextureLoader_PAM{file}};
      loader->loadHeaderPAM();
      return std::unique_ptr<TextureLoader>{loader.release()};
   }

   //
   // CreateTextureLoader_PBM
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader_PBM(FS::File *file)
   {
      std::unique_ptr<TextureLoader_PAM> loader{new TextureLoader_PAM{file}};
      loader->loadHeaderPNM();
      return std::unique_ptr<TextureLoader>{loader.release()};
   }

   //
   // CreateTextureLoader_PGM
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader_PGM(FS::File *file)
   {
      std::unique_ptr<TextureLoader_PAM> loader{new TextureLoader_PAM{file}};
      loader->loadHeaderPNM();
      return std::unique_ptr<TextureLoader>{loader.release()};
   }

   //
   // CreateTextureLoader_PPM
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader_PPM(FS::File *file)
   {
      std::unique_ptr<TextureLoader_PAM> loader{new TextureLoader_PAM{file}};
      loader->loadHeaderPNM();
      return std::unique_ptr<TextureLoader>{loader.release()};
   }
}

// EOF

