//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Shader object handles.
//
//-----------------------------------------------------------------------------

#include "GL/Shader.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // Shader constructor
      //
      
      Shader::Shader() :
         handlef{},
         handlev{},
         program{}
      {
      }
      
      //
      // Shader constructor
      //
      
      Shader::Shader(char const *f, char const *v) :
         Shader{}
      {
         openFrag(f);
         openVert(v);
         link();
      }
      
      //
      // Guess what, another Shader constructor
      //

      Shader::Shader(Doom::FS::File *ffp, Doom::FS::File *vfp) :
         Shader{}
      {
         openFragFile(ffp);
         openVertFile(vfp);
         link();
      }

      //
      // Still a Shader constructor
      //

      Shader::Shader(Shader &&other) :
         handlef{other.handlef},
         handlev{other.handlev},
         program{other.program}
      {
         other.handlef = 0;
         other.handlev = 0;
         other.program = 0;
      }

      //
      // CompileShader
      //

      template<GLenum Type>
      GLuint CompileShader(char const *data)
      {
         GLuint handle = glCreateShader(Type);

         glShaderSource(handle, 1, data, nullptr);
         glCompileShader(handle);

         GLint compiled;
         glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

         if(compiled != GL_TRUE)
         {
            static GLchar err[1024];
            GLsizei errlen = 0;

            glGetShaderInfoLog(handlev, 1024, &errlen, err);
            
            throw new ShaderError(err);
         }

         return handle;
      }

      //
      // Shader::link
      //

      void Shader::link()
      {
         if(!handlef || !handlev)
            throw new ShaderError("Shader::link: fragment or vertex handle is 0\n");

         if(program)
            throw new ShaderError("Shader::link: already open");

         program = glCreateProgram();

         glAttachShader(program, handlef);
         glAttachShader(program, handlev);

         glLinkProgram(program);

         GLint linked;
         glGetProgramiv(program, GL_LINK_STATUS, &linked);

         if(linked != GL_TRUE)
         {
            static GLchar err[1024];
            GLsizei errlen = 0;

            program = 0;
            
            glGetProgramInfoLog(program, 1024, &errlen, err);
            throw new ShaderError(err);
         }
      }
      
      //
      // Shader::compileVert
      //
      
      void Shader::compileVert(char const *data)
      {
         if(handlev)
            throw new ShaderError("Shader::compileVert: already open\n");
         
         handlev = CompileShader<GL_VERTEX_SHADER>(data);
      }
      
      //
      // Shader::compileFrag
      //
      
      void Shader::compileFrag(char const *data)
      {
         if(handlef)
            throw new ShaderError("Shader::compileFrag: already open\n");
         
         handlef = CompileShader<GL_FRAGMENT_SHADER>(data);
      }

      //
      // Shader::compileFragFile
      //

      void Shader::compileFragFile(Doom::FS::File *fp)
      {
         if(!fp)
            throw new ShaderError("Shader::compileFragFile: bad file\n");
         
         openFrag(fp->data);
      }

      //
      // Shader::compileVertFile
      //

      void Shader::compileVertFile(Doom::FS::File *fp)
      {
         if(!fp)
            throw new ShaderError("Shader::compileVertFile: bad file\n");
         
         openVert(fp->data);
      }
   }
}

// EOF
