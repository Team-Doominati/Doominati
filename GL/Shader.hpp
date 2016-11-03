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

#include "GL/gl_2_1.h"

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
   class Window;

   //
   // ShaderError
   //

   class ShaderError : public std::runtime_error
   {
      using std::runtime_error::runtime_error;
   };

   //
   // Shader
   //

   class Shader
   {
   public:
      Shader();
      Shader(char const *f, char const *v);
      Shader(FS::File *ffp, FS::File *vfp);
      Shader(Shader const &other) = delete;
      Shader(Shader &&other) = default;

      void compileFrag(char const *data);
      void compileVert(char const *data);
      void compileFrag(FS::File *fp);
      void compileVert(FS::File *fp);
      void link();
      void update();

      GLuint handlef;
      GLuint handlev;
      GLuint program;

      GLint u_ticks;
      GLint u_mseconds;
      GLint u_seconds;
   };
}

#endif//DGE__GL__Shader_H__
