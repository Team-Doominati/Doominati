//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Font management.
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
   // Renderer::fontAdd
   //
   std::size_t Renderer::fontAdd(GDCC::Core::String name, FS::File *fp, int ptSize)
   {
      Core::ResourceSaver<FontFace> fntSave{fntMan, fntBound};

      try
         {return fntMan.add({texMan, fp, ptSize}, name)->idx;}
      catch(FontLoadError const &err)
      {
         std::cerr << "FontLoadError: " << err.what() << std::endl;
         return static_cast<std::size_t>(-1);
      }
   }

   //
   // Renderer::fontBind
   //
   void Renderer::fontBind(Font *fnt)
   {
      if(!fnt) fontUnbind();
      else     fntBound = fnt;
   }

   //
   // Renderer::fontGet
   //
   Font *Renderer::fontGet(GDCC::Core::String name)
   {
      if(auto fnt = fntMan.resMap.find(name))
         return fnt;

      std::cerr << "unknown font: " << name << std::endl;
      return nullptr;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   DGE_Code_NativeLoaderDefn(Renderer_Font);

   //
   // unsigned DGE_Font_Create(__str_ent *name, char const *fname, int ptSize)
   //
   DGE_Code_NativeDefn(DGE_Font_Create)
   {
      GDCC::Core::String str{argv[0]};

      auto fname = Code::MemStrDup(Code::MemPtr<Code::Byte const>{&task->prog->memory, argv[1]});
      auto fp = FS::Dir::FindFile(fname.get());

      task->dataStk.push(Renderer::GetCurrent()->fontAdd(str, fp, argv[2]));

      return false;
   }

   //
   // unsigned DGE_Font_Get(__str_ent *name)
   //
   DGE_Code_NativeDefn(DGE_Font_Get)
   {
      GDCC::Core::String str{argv[0]};
      task->dataStk.push(Renderer::GetCurrent()->fontGet(str)->idx);
      return false;
   }

   //
   // void DGE_Font_Bind(unsigned fnt)
   //
   DGE_Code_NativeDefn(DGE_Font_Bind)
   {
      if(argv[0])
         Renderer::GetCurrent()->fontBind(Renderer::GetCurrent()->fontGet(argv[0]));
      else
         Renderer::GetCurrent()->fontUnbind();

      return false;
   }
}

// EOF

