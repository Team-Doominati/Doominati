//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Texture management.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer.hpp"

#include "FS/Dir.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::textureBind
   //
   void Renderer::textureBind(TextureData const *tex)
   {
      if(!tex)
         textureUnbind();

      else if(!texBound || texBound->tex != tex->tex)
         glBindTexture(GL_TEXTURE_2D, (texBound = tex)->tex);
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
   TextureData *Renderer::textureGet(char const *name)
   {
      return &textureGetRaw(name)->data;
   }

   //
   // Renderer::textureGet
   //
   TextureData *Renderer::textureGet(std::size_t idx)
   {
      return &texMan.get(idx)->data;
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
      if(auto tex = texMan.resMap.find(name))
         return tex;

      if(name[0] == '@')
         return textureGet_File(name);

      std::cerr << "unknown texture: " << name << std::endl;
      return textureGet_NoFi(name);
   }

   //
   // Renderer::textureGet_File
   //
   Renderer::Texture *Renderer::textureGet_File(GDCC::Core::String name)
   {
      auto filename = name.data() + 1;
      auto file = FS::Dir::FindFile(filename);

      if(!file)
      {
         std::cerr << "texture file not found: " << filename << std::endl;
         return textureGet_NoFi(name);
      }

      try
      {
         auto loader = CreateTextureLoader(file);
         GLsizei  width, height;
         std::tie(width, height) = loader->size();
         std::unique_ptr<TexturePixel[]> buf{new TexturePixel[width * height]};
         loader->data(buf.get());

         return texMan.add({width, height, buf.get()}, name);
      }
      catch(TextureLoaderError const &err)
      {
         std::cerr << "TextureLoaderError: " << filename
            << ": " << err.what() << std::endl;
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

      return texMan.add({2, 2, data}, name);
   }

   //
   // Renderer::textureGet_NoFi
   //
   Renderer::Texture *Renderer::textureGet_NoFi(GDCC::Core::String name)
   {
      TexturePixel data[64*64]{};

      for(int i = 0; i < 4096; i++)
      {
         data[i][3] = 1;

         if(((i & 8) == 8) != ((i & 512) == 512))
            data[i][0] = data[i][2] = 1;
      }

      return texMan.add({64, 64, data}, name);
   }

   //
   // Renderer::textureUnbind
   //
   void Renderer::textureUnbind()
   {
      TextureData const *tex = &texMan.resNone->data;
      if(!texBound || texBound->tex != tex->tex)
         glBindTexture(GL_TEXTURE_2D, (texBound = tex)->tex);
   }
}

// EOF

