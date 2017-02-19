//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Shader management.
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
// Static Objects                                                             |
//

namespace DGE::GL
{
   //
   // BaseFragShader
   //
   static char const BaseFragShader[] = R"(
      #version 120

      uniform sampler2D DGE_Texture;

      varying vec4 color;
      varying vec4 texcoord;

      void main(void)
      {
         gl_FragColor = texture2D(DGE_Texture, texcoord.xy) * color;
      }
   )";

   //
   // BaseVertShader
   //
   static char const BaseVertShader[] = R"(
      #version 120

      varying vec4 color;
      varying vec4 texcoord;

      void main(void)
      {
         gl_Position = ftransform();

         texcoord = gl_MultiTexCoord0;
         color = gl_Color;
      }
   )";
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::shaderAdd
   //
   std::size_t Renderer::shaderAdd(GDCC::Core::String name, char const *f, char const *v)
   {
      try
         {return shdMan.add({f, v}, name)->idx;}

      catch(ShaderError const &err)
      {
         std::cerr << "ShaderError: " << err.what() << std::endl;
         return shaderGet_Base(name)->idx;
      }
   }

   //
   // Renderer::shaderAdd
   //
   std::size_t Renderer::shaderAdd(GDCC::Core::String name, FS::File *f, FS::File *v)
   {
      try
         {return shdMan.add({f, v}, name)->idx;}

      catch(ShaderError const &err)
      {
         std::cerr << "ShaderError: " << err.what() << std::endl;
         return shaderGet_Base(name)->idx;
      }
   }

   //
   // Renderer::shaderBind
   //
   void Renderer::shaderBind(ShaderData *shd)
   {
      if(!shd)
         shaderUnbind();

      else
      {
         if(!shdBound || shdBound->prog != shd->prog)
            glUseProgram((shdBound = shd)->prog);

         shdBound->update();
      }
   }

   //
   // Renderer::shaderGetRaw
   //
   Renderer::Shader *Renderer::shaderGetRaw(GDCC::Core::String name)
   {
      if(auto shd = shdMan.resMap.find(name))
         return shd;

      std::cerr << "unknown shader: " << name << std::endl;
      return shaderGet_Base(name);
   }

   //
   // Renderer::shaderGet_Base
   //
   Renderer::Shader *Renderer::shaderGet_Base(GDCC::Core::String name)
   {
      return shdMan.add({BaseFragShader, BaseVertShader}, name);
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   //
   // unsigned DGE_CreateShaderData(__str_ent *name, char const *frag, *vert)
   //
   DGE_Code_NativeDefn(DGE_CreateShaderData)
   {
      GDCC::Core::String str{argv[0]};
      
      auto fragD = Code::MemStrDup(Code::MemPtr<Code::Byte const>{&task->prog->memory, argv[1]});
      auto vertD = Code::MemStrDup(Code::MemPtr<Code::Byte const>{&task->prog->memory, argv[2]});
      
      task->dataStk.push(Renderer::GetCurrent()->shaderAdd(str, fragD.get(), vertD.get()));
      
      return false;
   }

   //
   // unsigned DGE_CreateShaderFile(__str_ent *name, char const *frag, *vert)
   //
   DGE_Code_NativeDefn(DGE_CreateShaderFile)
   {
      GDCC::Core::String str{argv[0]};
      
      auto fragD = Code::MemStrDup(Code::MemPtr<Code::Byte const>{&task->prog->memory, argv[1]});
      auto vertD = Code::MemStrDup(Code::MemPtr<Code::Byte const>{&task->prog->memory, argv[2]});
      
      auto fragF = FS::Dir::FindFile(fragD.get());
      auto vertF = FS::Dir::FindFile(vertD.get());
      
      task->dataStk.push(Renderer::GetCurrent()->shaderAdd(str, fragF, vertF));
      
      return false;
   }

   //
   // unsigned DGE_GetShader(__str_ent *name)
   //
   DGE_Code_NativeDefn(DGE_GetShader)
   {
      GDCC::Core::String str{argv[0]};
      task->dataStk.push(Renderer::GetCurrent()->shaderGetIdx(str));
      return false;
   }

   //
   // void DGE_DrawShader(unsigned shd)
   //
   DGE_Code_NativeDefn(DGE_DrawShader)
   {
      if(argv[0])
         Renderer::GetCurrent()->shaderBind(Renderer::GetCurrent()->shaderGet(argv[0]));
      else
         Renderer::GetCurrent()->shaderUnbind();

      return false;
   }
}

// EOF

