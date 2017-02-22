//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// JPEG/JFIF/etc texture loading using libjpeg.
//
//-----------------------------------------------------------------------------

#if DGE_Use_JPEG
#include "GL/Texture.hpp"

#include "FS/File.hpp"

#include "jpeglib.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // TextureLoader_JPEG
   //
   class TextureLoader_JPEG : public TextureLoader
   {
   public:
      TextureLoader_JPEG(FS::File *file_);

      virtual void data(TexturePixel *buf);
      virtual std::pair<TextureDim, TextureDim> size();

   private:
      FS::File *file;

      struct jpeg_decompress_struct inf;
      struct jpeg_error_mgr err;


      [[noreturn]] static void Error(j_common_ptr);
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // TextureLoader_JPEG constructor
   //
   TextureLoader_JPEG::TextureLoader_JPEG(FS::File *file_) :
      file{file_},
      inf{},
      err{}
   {
      // JPEG outputs crap to stdout by default so we have to define functions
      // that don't do that instead of using the standard ones
      err.error_exit      = Error;
      err.emit_message    = [](j_common_ptr, int){};
      err.output_message  = [](j_common_ptr){};
      err.format_message  = [](j_common_ptr, char *){};
      err.reset_error_mgr = [](j_common_ptr inf)
         {inf->err->num_warnings = inf->err->msg_code = 0;};

      err.trace_level = err.num_warnings = err.msg_code = 0;

      inf.err = &err;

      jpeg_create_decompress(&inf);
      jpeg_mem_src(&inf, reinterpret_cast<unsigned char const *>(file->data), file->size);
      jpeg_read_header(&inf, true);
   }

   //
   // TextureLoader_JPEG::data
   //
   void TextureLoader_JPEG::data(TexturePixel *buf)
   {
      jpeg_start_decompress(&inf);

      JDIMENSION width = inf.image_width;

      std::unique_ptr<unsigned char[]> lnbuf{new unsigned char[width * 3 * 64]};
      unsigned char *lnbufptr[32]; // GOOD CODE

      // GOOD CODE 2: WE HAVE ALWAYS BEEN AT WAR WITH GOODCODEIA
      for(int i = 0; i < 32; i++)
         lnbufptr[i] = lnbuf.get() + (width * 3 * i);

      while(inf.output_scanline < inf.output_height)
      {
         auto read = jpeg_read_scanlines(&inf, lnbufptr, 32);
         auto *ptr = lnbuf.get();

         for(JDIMENSION y = 0; y < read; y++, buf += width)
            for(JDIMENSION x = 0; x < width; x++, ptr += 3)
         {
            buf[x][0] = ptr[0] / 255.0f;
            buf[x][1] = ptr[1] / 255.0f;
            buf[x][2] = ptr[2] / 255.0f;
            buf[x][3] = 1.0f;
         }
      }
   }

   //
   // TextureLoader_JPEG::size
   //
   std::pair<TextureDim, TextureDim> TextureLoader_JPEG::size()
   {
      return {inf.image_width, inf.image_height};
   }

   //
   // TextureLoader_JPEG::Error
   //
   void TextureLoader_JPEG::Error(j_common_ptr)
   {
      throw TextureLoaderError("failed to decompress file");
   }

   //
   // CreateTextureLoader_JPEG
   //
   std::unique_ptr<TextureLoader> CreateTextureLoader_JPEG(FS::File *file)
   {
      std::unique_ptr<TextureLoader_JPEG> loader{new TextureLoader_JPEG{file}};
      return std::unique_ptr<TextureLoader>{loader.release()};
   }
}

#endif//DGE_Use_JPEG

