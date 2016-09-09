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

#include <exception>


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
      
      class ShaderError : public std::exception
      {
      public:
         ShaderError(char const *whatstr_) : whatstr{whatstr_} { }
         ShaderError &operator=(ShaderError const &other) = default;
         char const *what() const noexcept { return whatstr; }
         
      private:
         char const *whatstr;
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
         Shader(Doom::FS::File *ffp, Doom::FS::File *vfp);
         Shader(Shader const &brother) = delete; // [Yholl]
         Shader(Shader &&other);

         void compileVert(char const *data);
         void compileFrag(char const *data);
         void compileFragFile(Doom::FS::File *fp);
         void compileVertFile(Doom::FS::File *fp);
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
