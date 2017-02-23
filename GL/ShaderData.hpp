//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Shader object handles.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__ShaderData_H__
#define DGE__GL__ShaderData_H__

#include "GL/OpenGL2.1.h"

#include "Core/Types.hpp"

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
   class ShaderData;

   using Shader = Core::Resource<ShaderData>;

   //
   // ShaderError
   //
   class ShaderError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };

   //
   // ShaderData
   //
   class ShaderData
   {
   public:
      ShaderData() :
         prog{}, frag{}, vert{}, u_ticks{-1}, u_seconds{-1} {}

      ShaderData(ShaderData const &) = delete;
      ShaderData(char const *f, char const *v);
      ShaderData(FS::File *f, FS::File *v);

      ShaderData(ShaderData &&s) :
         prog{s.prog}, frag{s.frag}, vert{s.vert}, u_ticks{-1}, u_seconds{-1}
         {s.prog = s.frag = s.vert = 0; postLink();}

      ~ShaderData();

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

#endif//DGE__GL__ShaderData_H__
