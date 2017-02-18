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

#ifndef DGE__GL__Shader_H__
#define DGE__GL__Shader_H__

#include "GL/OpenGL2.1.h"

#include <stdexcept>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::FS
{
   class File;
}

namespace DGE::GL
{
   //
   // ShaderError
   //
   class ShaderError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };

   //
   // Shader
   //
   class Shader
   {
   public:
      Shader() :
         prog{}, frag{}, vert{}, u_ticks{}, u_seconds{} {}

      Shader(Shader const &) = delete;
      Shader(char const *f, char const *v);
      Shader(FS::File *f, FS::File *v);

      Shader(Shader &&s) : frag{s.frag}, vert{s.vert}, prog{s.prog}
         {s.frag = s.vert = s.prog = 0; postLink();}

      ~Shader();

      void update();

      GLuint prog;

   private:
      void compile(GLuint &handle, char const *data, GLenum type,
         GLint size = -1);

      void link();
      void postLink();

      GLuint frag;
      GLuint vert;

      GLint u_ticks;
      GLint u_seconds;
   };
}

#endif//DGE__GL__Shader_H__
