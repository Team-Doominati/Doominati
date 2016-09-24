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

namespace DGE
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
      GLuint CompileShader(char const *data, GLint size = -1)
      {
         GLuint handle = glCreateShader(Type);

         glShaderSource(handle, 1, &data, &size);
         glCompileShader(handle);

         GLint compiled;
         glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

         if(compiled != GL_TRUE)
         {
            static GLchar err[1024];
            glGetShaderInfoLog(handle, 1024, nullptr, err);
            throw ShaderError{err};
         }

         return handle;
      }

      //
      // Shader::link
      //

      void Shader::link()
      {
         if(!handlef || !handlev)
            throw ShaderError{"fragment or vertex handle is 0"};

         if(program)
            throw ShaderError{"already open"};

         program = glCreateProgram();

         glAttachShader(program, handlef);
         glAttachShader(program, handlev);

         glLinkProgram(program);

         GLint linked;
         glGetProgramiv(program, GL_LINK_STATUS, &linked);

         if(linked != GL_TRUE)
         {
            static GLchar err[1024];
            glGetProgramInfoLog(program, 1024, nullptr, err);
            program = 0;
            throw ShaderError{err};
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
            throw ShaderError{"already open"};

         handlef = CompileShader<GL_FRAGMENT_SHADER>(data);
      }

      //
      // Shader::compileVert
      //

      void Shader::compileVert(char const *data)
      {
         if(handlev)
            throw ShaderError{"already open"};

         handlev = CompileShader<GL_VERTEX_SHADER>(data);
      }

      //
      // Shader::compileFrag (file overload)
      //

      void Shader::compileFrag(FS::File *fp)
      {
         if(!fp)
            throw ShaderError{"bad file"};

         if(handlef)
            throw ShaderError{"already open"};

         handlef = CompileShader<GL_FRAGMENT_SHADER>(fp->data, fp->size);
      }

      //
      // Shader::compileVert (file overload)
      //

      void Shader::compileVert(FS::File *fp)
      {
         if(!fp)
            throw ShaderError{"bad file"};

         if(handlev)
            throw ShaderError{"already open"};

         handlev = CompileShader<GL_VERTEX_SHADER>(fp->data, fp->size);
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
