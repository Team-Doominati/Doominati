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

#include "GL/ShaderData.hpp"
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
   // ShaderData constructor
   //
   ShaderData::ShaderData(char const *f, char const *v) : ShaderData{}
   {
      compile(frag, f, GL_FRAGMENT_SHADER);
      compile(vert, v, GL_VERTEX_SHADER);
      link();
   }

   //
   // ShaderData constructor
   //
   ShaderData::ShaderData(FS::File *f, FS::File *v) : ShaderData{}
   {
      if(!f || !v) throw ShaderError{"invalid file"};

      compile(frag, f->data, GL_FRAGMENT_SHADER, f->size);
      compile(vert, v->data, GL_VERTEX_SHADER,   v->size);
      link();
   }

   //
   // ShaderData destructor
   //
   ShaderData::~ShaderData()
   {
      if(glIsProgram(prog)) glDeleteProgram(prog);
      if( glIsShader(frag))  glDeleteShader(frag);
      if( glIsShader(vert))  glDeleteShader(vert);
   }

   //
   // ShaderData::compile
   //
   void ShaderData::compile(GLuint &handle, char const *data, GLenum type,
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
   // ShaderData::link
   //
   void ShaderData::link()
   {
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
   // ShaderData::postLink
   //
   void ShaderData::postLink()
   {
      // Temporarily change programs to get locations.
      GLint prev; glGetIntegerv(GL_CURRENT_PROGRAM, &prev);

      glUseProgram(prog);

      glUniform1i(glGetUniformLocation(prog, "DGE_Texture"), 0);

      // Get device data locations.
      u_ticks   = glGetUniformLocation(prog, "DGE_Ticks");
      u_seconds = glGetUniformLocation(prog, "DGE_Seconds");

      // Go back to previous program.
      glUseProgram(prev);
   }

   //
   // ShaderData::update
   //
   void ShaderData::update()
   {
      glUniform1i(u_ticks,   Core::GetTicks<Core::PlayTick<GLint>>());
      glUniform1f(u_seconds, Core::GetTicks<Core::Second<GLfloat>>());
   }
}

// EOF
