//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Texture management.
//
//-----------------------------------------------------------------------------

#include "GL/Window/PrivData.hpp"

#include "FS/Dir.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Window::PrivData::texAdd
   //
   Window::Texture *Window::PrivData::texAdd(GLsizei w, GLsizei h,
      TexturePixel const *data, GDCC::Core::String name)
   {
      std::size_t idx = texVec.size();
      texVec.emplace_back(TextureData(w, h, data), name, idx);
      texMap.insert(&texVec.back());

      texNone = &texVec.front();

      return &texVec.back();
   }

   //
   // Window::textureBind
   //
   void Window::textureBind(TextureData *tex)
   {
      if(privdata->texBound != tex->tex)
      {
         glBindTexture(GL_TEXTURE_2D, tex->tex);
         privdata->texBound = tex->tex;
      }
   }

   //
   // Window::textureGet
   //
   TextureData *Window::textureGet(char const *name)
   {
      return &textureGetRaw(name)->data;
   }

   //
   // Window::textureGet
   //
   TextureData *Window::textureGet(GDCC::Core::String name)
   {
      return &textureGetRaw(name)->data;
   }

   //
   // Window::textureGet
   //
   TextureData *Window::textureGet(std::size_t idx)
   {
      if(idx < privdata->texVec.size())
         return &privdata->texVec[idx].data;

      return &privdata->texNone->data;
   }

   //
   // Window::textureGetIdx
   //
   std::size_t Window::textureGetIdx(GDCC::Core::String name)
   {
      return textureGetRaw(name)->idx;
   }

   //
   // Window::textureGetRaw
   //
   Window::Texture *Window::textureGetRaw(GDCC::Core::String name)
   {
      if(auto tex = privdata->texMap.find(name))
         return tex;

      if(name[0] == '@')
         return textureGet_File(name);

      std::cerr << "unknown texture: " << name << '\n';
      return textureGet_None(name);
   }

   //
   // Window::textureGet_File
   //
   Window::Texture *Window::textureGet_File(GDCC::Core::String name)
   {
      char const *filename = name.data() + 1;
      FS::File *file = FS::Dir::FindFile(filename);

      if(!file)
      {
         std::cerr << "texture file not found: " << filename << '\n';
         return textureGet_None(name);
      }

      try
      {
         auto loader = CreateTextureLoader(file);
         GLsizei width, height;
         std::tie(width, height) = loader->size();
         std::unique_ptr<TexturePixel[]> buf{new TexturePixel[width * height]};
         loader->data(buf.get());

         return privdata->texAdd(width, height, buf.get(), name);
      }
      catch(TextureLoaderError const &err)
      {
         std::cerr << "TextureLoaderError: " << filename
            << ": " << err.what() << '\n';
         return textureGet_None(name);
      }
   }

   //
   // Window::textureGet_None
   //
   Window::Texture *Window::textureGet_None(GDCC::Core::String name)
   {
      TexturePixel const data[4] =
         {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

      return privdata->texAdd(2, 2, data, name);
   }

   //
   // Window::textureUnbind
   //
   void Window::textureUnbind()
   {
      TextureData const *tex = &privdata->texNone->data;
      if(privdata->texBound != tex->tex)
      {
         glBindTexture(GL_TEXTURE_2D, tex->tex);
         privdata->texBound = tex->tex;
      }
   }
}

// EOF

