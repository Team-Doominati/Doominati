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
#include "GL/OpenGL2.1.h"

#include "Core/Time.hpp"

#include "FS/File.hpp"

#define GenCompile(handle, type, ...) \
   if(glIsShader(handle)) throw ShaderError(#type " already open"); \
   handle = CompileShader<type>(__VA_ARGS__)


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::GL
{
   //
   // ThrowGLError
   //
   static void ThrowGLError(GLuint &handle)
   {
      thread_local GLchar err[1024];
      glGetShaderInfoLog(handle, 1024, nullptr, err);
      handle = 0;
      throw ShaderError{err};
   }

   //
   // CompileShader
   //
   template<GLenum Type>
   static GLuint CompileShader(char const *data, GLint size = -1)
   {
      GLuint handle = glCreateShader(Type);

      glShaderSource(handle, 1, &data, &size);
      glCompileShader(handle);

      GLint compiled;
      glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

      if(compiled != GL_TRUE)
         ThrowGLError(handle);

      return handle;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Shader destructor
   //
   Shader::~Shader()
   {
      if(glIsProgram(prog)) glDeleteProgram(prog);
      if(glIsShader(frag))  glDeleteShader(frag);
      if(glIsShader(vert))  glDeleteShader(vert);
   }

   //
   // Shader::compileFrag
   //
   void Shader::compileFrag(char const *data)
   {
      GenCompile(frag, GL_FRAGMENT_SHADER, data);
   }

   void Shader::compileFrag(FS::File *fp)
   {
      if(!fp) throw ShaderError{"bad file"};
      GenCompile(frag, GL_FRAGMENT_SHADER, fp->data, fp->size);
   }

   //
   // Shader::compileVert
   //
   void Shader::compileVert(char const *data)
   {
      GenCompile(vert, GL_VERTEX_SHADER, data);
   }

   void Shader::compileVert(FS::File *fp)
   {
      if(!fp) throw ShaderError{"bad file"};
      GenCompile(vert, GL_VERTEX_SHADER, fp->data, fp->size);
   }

   //
   // Shader::link
   //
   void Shader::link()
   {
      if(!glIsShader(frag) || !glIsShader(vert))
         throw ShaderError{"fragment or vertex handle is invalid"};

      if(glIsProgram(prog))
         throw ShaderError{"program already created"};

      // Create the program and attach shaders.
      prog = glCreateProgram();

      glAttachShader(prog, frag);
      glAttachShader(prog, vert);

      // Link and check for errors.
      glLinkProgram(prog);

      GLint linked; glGetProgramiv(prog, GL_LINK_STATUS, &linked);
      if(linked != GL_TRUE)
         ThrowGLError(prog);

      postLink();
   }

   //
   // Shader::postLink
   //
   void Shader::postLink()
   {
      // Temporarily change programs to get locations.
      GLint idprev; glGetIntegerv(GL_CURRENT_PROGRAM, &idprev);

      glUseProgram(prog);

      glUniform1i(glGetUniformLocation(prog, "dge_texture"), 0);

      // Get device data locations.
      u_ticks    = glGetUniformLocation(prog, "dge_ticks");
      u_mseconds = glGetUniformLocation(prog, "dge_mseconds");
      u_seconds  = glGetUniformLocation(prog, "dge_seconds");

      // Done -- go back to previous program.
      glUseProgram(idprev);
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

   //
   // Shader::setCurrent
   //
   void Shader::setCurrent()
   {
      glUseProgram(prog);
   }
}

// EOF
