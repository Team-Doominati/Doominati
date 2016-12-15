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
      Shader() = delete;
      Shader(char const *f, char const *v)
         {compileFrag(f); compileVert(v); link();}
      Shader(FS::File *f, FS::File *v)
         {compileFrag(f); compileVert(v); link();}
      Shader(Shader const &) = delete;
      Shader(Shader &&s) : frag{s.frag}, vert{s.vert}, prog{s.prog}
         {s.frag = s.vert = s.prog = 0; postLink();}
      ~Shader();

      void update();
      void setCurrent();

   private:
      void compileFrag(char const *data);
      void compileFrag(FS::File *fp);

      void compileVert(char const *data);
      void compileVert(FS::File *fp);

      void link();
      void postLink();

      GLuint frag;
      GLuint vert;
      GLuint prog;

      GLint u_ticks;
      GLint u_mseconds;
      GLint u_seconds;
   };
}

#endif//DGE__GL__Shader_H__
