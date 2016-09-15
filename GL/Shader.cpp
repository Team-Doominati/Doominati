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
      
      Shader::Shader(FS::File *ffp, FS::File *vfp) :
         Shader{}
      {
         compileFrag(ffp);
         compileVert(vfp);
         link();
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

         GLint idprev;
         glGetIntegerv(GL_CURRENT_PROGRAM, &idprev);

         glUseProgram(program);

         glUniform1i(glGetUniformLocation(program, "dge_texture"), 0);

         // Get device data locations.
         u_ticks    = glGetUniformLocation(program, "dge_ticks");
         u_mseconds = glGetUniformLocation(program, "dge_mseconds");
         u_seconds  = glGetUniformLocation(program, "dge_seconds");

         glUseProgram(idprev);
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
      // Shader::compileVert
      //
      
      void Shader::compileVert(char const *data)
      {
         if(handlev)
            throw ShaderError("Shader::compileVert: already open\n");
         
         handlev = CompileShader<GL_VERTEX_SHADER>(data);
      }
      
      //
      // Shader::compileFrag (file overload)
      //

      void Shader::compileFrag(FS::File *fp)
      {
         if(!fp)
            throw ShaderError("Shader::compileFragFile: bad file\n");
         
         compileFrag(fp->data);
      }

      //
      // Shader::compileVert (file overload)
      //

      void Shader::compileVert(FS::File *fp)
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
         glUniform1i(u_ticks,    Core::GetTicks<Core::PlayTick<GLint>>());
         glUniform1i(u_mseconds, static_cast<GLint>(Core::GetTicks<Core::Second<double>>() * 1000.0));
         glUniform1i(u_seconds,  Core::GetTicks<Core::Second<GLint>>());
      }
   }
}

// EOF
