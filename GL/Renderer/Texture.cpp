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

#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include "FS/Dir.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::textureAdd
   //
   std::size_t Renderer::textureAdd(GDCC::Core::String name, FS::File *fp)
   {
      Core::ResourceSaver<TextureData> texSave{texMan, texBound};

      return textureGet_File(name, fp)->idx;
   }

   //
   // Renderer::textureBind
   //
   void Renderer::textureBind(Texture const *tex)
   {
      if(!tex)
         textureUnbind();

      else if(texBound != tex)
         glBindTexture(GL_TEXTURE_2D, (texBound = tex)->data.tex);
   }

   //
   // Renderer::textureGet
   //
   Texture *Renderer::textureGet(GDCC::Core::String name)
   {
      if(auto tex = texMan.resMap.find(name))
         return tex;

      Core::ResourceSaver<TextureData> texSave{texMan, texBound};

      if(name[0] == '@')
         return textureGet_File(name, FS::Dir::FindFile(name.data() + 1));

      std::cerr << "unknown texture: " << name << std::endl;
      return textureGet_NoFi(name);
   }

   //
   // Renderer::textureGet_File
   //
   Texture *Renderer::textureGet_File(GDCC::Core::String name, FS::File *file)
   {
      if(!file)
      {
         std::cerr << "texture file not found in " << name << std::endl;
         return textureGet_NoFi(name);
      }

      try
      {
         auto loader = CreateTextureLoader(file);
         TextureDim width, height;
         std::tie  (width, height) = loader->size();
         std::unique_ptr<TexturePixel[]> buf{new TexturePixel[width * height]};
         loader->data(buf.get());

         return texMan.add({width, height, buf.get()}, name);
      }
      catch(TextureLoaderError const &err)
      {
         std::cerr << "TextureLoaderError: " << file->name
            << ": " << err.what() << std::endl;
         return textureGet_None(name);
      }
   }

   //
   // Renderer::textureGet_None
   //
   Texture *Renderer::textureGet_None(GDCC::Core::String name)
   {
      TexturePixel const data[4] =
         {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

      return texMan.add({2, 2, data}, name);
   }

   //
   // Renderer::textureGet_NoFi
   //
   Texture *Renderer::textureGet_NoFi(GDCC::Core::String name)
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
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // unsigned DGE_Texture_Create(__str_ent *name, char const *fname)
   //
   DGE_Code_NativeDefn(DGE_Texture_Create)
   {
      GDCC::Core::String str{argv[0]};

      auto fname = Code::MemStrDup(Code::MemPtr<Code::Byte const>{&task->prog->memory, argv[1]});
      auto fp = FS::Dir::FindFile(fname.get());

      task->dataStk.push(Renderer::GetCurrent()->textureAdd(str, fp));

      return false;
   }

   //
   // unsigned DGE_Texture_Get(__str_ent *name)
   //
   DGE_Code_NativeDefn(DGE_Texture_Get)
   {
      GDCC::Core::String str{argv[0]};
      task->dataStk.push(Renderer::GetCurrent()->textureGet(str)->idx);
      return false;
   }

   //
   // void DGE_Texture_Bind(unsigned tex)
   //
   DGE_Code_NativeDefn(DGE_Texture_Bind)
   {
      if(argv[0])
         Renderer::GetCurrent()->textureBind(Renderer::GetCurrent()->textureGet(argv[0]));
      else
         Renderer::GetCurrent()->textureUnbind();

      return false;
   }

   //
   // DGE_Point2I DGE_Texture_GetSize(unsigned tex);
   //
   DGE_Code_NativeDefn(DGE_Texture_GetSize)
   {
      auto tex = Renderer::GetCurrent()->textureGet(argv[0]);
      task->dataStk.push(tex->data.w);
      task->dataStk.push(tex->data.h);
      return false;
   }
}

// EOF

