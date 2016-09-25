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
   // Window::textureBind
   //
   void Window::textureBind(TextureData *tex)
   {
      if(privdata->textureCurrent != tex->tex)
      {
         glBindTexture(GL_TEXTURE_2D, tex->tex);
         privdata->textureCurrent = tex->tex;
      }

      textureMinMax = tex->minmax;
   }

   //
   // Window::textureGet
   //
   TextureData *Window::textureGet(char const *name)
   {
      auto itr = privdata->textures.find(name);
      if(itr != privdata->textures.end())
         return &itr->second;

      if(*name == '@')
         return textureGet_File(name + 1);

      std::cerr << "unknown texture: " << name << '\n';
      return textureGet_None(name);
   }

   //
   // Window::textureGet_File
   //
   TextureData *Window::textureGet_File(char const *name)
   {
      FS::File *file = FS::Dir::FindFile(name);

      if(!file)
      {
         std::cerr << "texture file not found: " << name << '\n';
         return textureGet_None(name - 1);
      }

      try
      {
         auto loader = CreateTextureLoader(file);
         GLsizei width, height;
         std::tie(width, height) = loader->size();
         std::unique_ptr<TexturePixel[]> buf{new TexturePixel[width * height]};
         loader->data(buf.get());

         return &privdata->textures.emplace(std::piecewise_construct,
            std::forward_as_tuple(name - 1),
            std::forward_as_tuple(width, height, buf.get())).first->second;
      }
      catch(TextureLoaderError const &err)
      {
         std::cerr << "TextureLoaderError: " << name
            << ": " << err.what() << '\n';
         return textureGet_None(name - 1);
      }
   }

   //
   // Window::textureGet_None
   //
   TextureData *Window::textureGet_None(char const *name)
   {
      TexturePixel const data[4] =
         {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

      return &privdata->textures.emplace(std::piecewise_construct,
         std::forward_as_tuple(name),
         std::forward_as_tuple(2, 2, data)).first->second;
   }

   //
   // Window::textureUnbind
   //

   void Window::textureUnbind()
   {
      TextureData const *tex = textureNone;
      if(privdata->textureCurrent != tex->tex)
      {
         glBindTexture(GL_TEXTURE_2D, tex->tex);
         privdata->textureCurrent = tex->tex;
      }
   }
}

// EOF

