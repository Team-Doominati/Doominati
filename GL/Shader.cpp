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
#include "GL/gl_2_1.h"

#include "Core/Time.hpp"

#include "FS/File.hpp"


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
      
      Shader::Shader(char const *f, char const *v) :
         Shader{}
      {
         compileFrag(f);
         compileVert(v);
         link();
      }
      
      Shader::Shader(Doom::FS::File *ffp, Doom::FS::File *vfp) :
         Shader{}
      {
         compileFragFile(ffp);
         compileVertFile(vfp);
         link();
      }

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

         glShaderSource(handle, 1, &data, nullptr);
         glCompileShader(handle);

         GLint compiled;
         glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

         if(compiled != GL_TRUE)
         {
            static GLchar err[1024];
            GLsizei errlen = 0;

            glGetShaderInfoLog(handle, 1024, &errlen, err);
            
            throw ShaderError(err);
         }

         return handle;
      }

      //
      // Shader::link
      //

      void Shader::link()
      {
         if(!handlef || !handlev)
            throw ShaderError("Shader::link: fragment or vertex handle is 0\n");

         if(program)
            throw ShaderError("Shader::link: already open");

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
            throw ShaderError(err);
         }

      }
      
      //
      // Shader::compileVert
      //
      
      void Shader::compileVert(char const *data)
      {
         if(handlev)
            throw ShaderError("Shader::compileVert: already open\n");
         
         handlev = CompileShader<GL_VERTEX_SHADER>(data);
      }
      
      //
      // Shader::compileFrag
      //
      
      void Shader::compileFrag(char const *data)
      {
         if(handlef)
            throw ShaderError("Shader::compileFrag: already open\n");
         
         handlef = CompileShader<GL_FRAGMENT_SHADER>(data);
      }

      //
      // Shader::compileFragFile
      //

      void Shader::compileFragFile(Doom::FS::File *fp)
      {
         if(!fp)
            throw ShaderError("Shader::compileFragFile: bad file\n");
         
         compileFrag(fp->data);
      }

      //
      // Shader::compileVertFile
      //

      void Shader::compileVertFile(Doom::FS::File *fp)
      {
         if(!fp)
            throw ShaderError("Shader::compileVertFile: bad file\n");
         
         compileVert(fp->data);
      }

      //
      // Shader::update
      //

      void Shader::update()
      {
         #define SetUniform(n, x) (glUniform1i(glGetUniformLocation(program, n), x))
         SetUniform("dge_ticks",    Core::GetTicks<Core::PlayTick<GLint>>());
         SetUniform("dge_mseconds", static_cast<GLint>(Core::GetTicks<Core::Second<double>>() * 1000.0));
         SetUniform("dge_seconds",  Core::GetTicks<Core::Second<GLint>>());
         #undef SetUniform
      }
   }
}

// EOF
