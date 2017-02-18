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


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::GL
{
   //
   // ThrowGLError
   //
   [[noreturn]]
   static void ThrowGLError(GLuint &handle)
   {
      thread_local GLchar err[1024];
      glGetShaderInfoLog(handle, 1024, nullptr, err);
      handle = 0;
      throw ShaderError{err};
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Shader constructor
   //
   Shader::Shader(char const *f, char const *v) : Shader{}
   {
      compile(frag, f, GL_FRAGMENT_SHADER);
      compile(vert, v, GL_VERTEX_SHADER);
      link();
   }

   //
   // Shader constructor
   //
   Shader::Shader(FS::File *f, FS::File *v) : Shader{}
   {
      if(!f || !v) throw ShaderError{"invalid file"};

      compile(frag, f->data, GL_FRAGMENT_SHADER, f->size);
      compile(vert, v->data, GL_VERTEX_SHADER,   v->size);
      link();
   }

   //
   // Shader destructor
   //
   Shader::~Shader()
   {
      if(glIsProgram(prog)) glDeleteProgram(prog);
      if( glIsShader(frag))  glDeleteShader(frag);
      if( glIsShader(vert))  glDeleteShader(vert);
   }

   //
   // Shader::compile
   //
   void Shader::compile(GLuint &handle, char const *data, GLenum type,
      GLint size)
   {
      if(!glIsShader(handle))
      {
         handle = glCreateShader(type);

         glShaderSource(handle, 1, &data, &size);
         glCompileShader(handle);

         GLint compiled;
         glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

         if(compiled != GL_TRUE)
            ThrowGLError(handle);
      }
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
      u_ticks   = glGetUniformLocation(prog, "dge_ticks");
      u_seconds = glGetUniformLocation(prog, "dge_seconds");

      // Go back to previous program.
      glUseProgram(idprev);
   }

   //
   // Shader::update
   //
   void Shader::update()
   {
      glUniform1i(u_ticks,   Core::GetTicks<Core::PlayTick<GLint>>());
      glUniform1f(u_seconds, Core::GetTicks<Core::Second<GLfloat>>());
   }
}

// EOF
