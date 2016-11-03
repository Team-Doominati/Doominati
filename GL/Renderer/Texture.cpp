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

#include "GL/Renderer/PrivData.hpp"

#include "FS/Dir.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::PrivData::texAdd
   //
   Renderer::Texture *Renderer::PrivData::texAdd(GLsizei texw, GLsizei texh,
      TexturePixel const *data, GDCC::Core::String name)
   {
      std::size_t idx = texVec.size();
      texVec.emplace_back(TextureData(texw, texh, data), name, idx);
      texMap.insert(&texVec.back());

      texNone = &texVec.front();

      return &texVec.back();
   }

   //
   // Renderer::textureBind
   //
   void Renderer::textureBind(TextureData *tex)
   {
      if(privdata->texBound != tex->tex)
      {
         glBindTexture(GL_TEXTURE_2D, tex->tex);
         privdata->texBound = tex->tex;
      }
   }

   //
   // Renderer::textureGet
   //
   TextureData *Renderer::textureGet(char const *name)
   {
      return &textureGetRaw(name)->data;
   }

   //
   // Renderer::textureGet
   //
   TextureData *Renderer::textureGet(GDCC::Core::String name)
   {
      return &textureGetRaw(name)->data;
   }

   //
   // Renderer::textureGet
   //
   TextureData *Renderer::textureGet(std::size_t idx)
   {
      if(idx < privdata->texVec.size())
         return &privdata->texVec[idx].data;

      return &privdata->texNone->data;
   }

   //
   // Renderer::textureGetIdx
   //
   std::size_t Renderer::textureGetIdx(GDCC::Core::String name)
   {
      return textureGetRaw(name)->idx;
   }

   //
   // Renderer::textureGetRaw
   //
   Renderer::Texture *Renderer::textureGetRaw(GDCC::Core::String name)
   {
      if(auto tex = privdata->texMap.find(name))
         return tex;

      if(name[0] == '@')
         return textureGet_File(name);

      std::cerr << "unknown texture: " << name << '\n';
      return textureGet_None(name);
   }

   //
   // Renderer::textureGet_File
   //
   Renderer::Texture *Renderer::textureGet_File(GDCC::Core::String name)
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
   // Renderer::textureGet_None
   //
   Renderer::Texture *Renderer::textureGet_None(GDCC::Core::String name)
   {
      TexturePixel const data[4] =
         {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

      return privdata->texAdd(2, 2, data, name);
   }

   //
   // Renderer::textureUnbind
   //
   void Renderer::textureUnbind()
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

