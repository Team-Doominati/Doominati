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

#ifndef Doom__GL__Shader_H__
#define Doom__GL__Shader_H__

#include "GL/gl_2_1.h"

#include <stdexcept>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace FS
   {
      class File;
   }

   namespace GL
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
         friend class Window;

      public:
         Shader();
         Shader(char const *f, char const *v);
         Shader(FS::File *ffp, FS::File *vfp);
         Shader(Shader const &brother) = delete; // [Yholl]
         Shader(Shader &&other);

         void compileFrag(char const *data);
         void compileVert(char const *data);
         void compileFrag(FS::File *fp);
         void compileVert(FS::File *fp);
         void link();

      protected:
         GLuint handlef;
         GLuint handlev;
         GLuint program;

         void update();
      };
   }
}

#endif//Doom__GL__Shader_H__
