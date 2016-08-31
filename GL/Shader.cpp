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

#include <iostream>

//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
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
      
		//
		// Shader constructor
		//

		Shader::Shader(Doom::FS::File *ffp, Doom::FS::File *vfp) :
			handlef{},
			handlev{},
			program{}
		{
			openFrag(ffp);
			openVert(vfp);
         compile();
		}

		//
		// Shader constructor
		//

		Shader::Shader(Shader &&other) :
			handlef{other.handlef},
			handlev{other.handlev}
		{
			other.handlef = 0;
			other.handlev = 0;
		}

		//
		// OpenShader
		//

		template<GLenum Type>
		GLuint OpenShader(Doom::FS::File *fp)
		{
			GLuint handle = glCreateShader(Type);

			glShaderSource(handle, 1, fp->data, nullptr);
			glCompileShader(handle);

			GLint compiled;
			glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

			if(compiled != GL_TRUE)
			{
				GLsizei errlen = 0;
				GLchar err[1024];

				glGetShaderInfoLog(handlev, 1024, &errlen, err);
				std::cerr << "OpenShader: " << err << '\n';

				return 0;
			}

			return handle;
		}

		//
		// Shader::openFrag
		//

		void Shader::openFrag(Doom::FS::File *fp)
		{
			if(handlef)
			{
				std::cerr << "Shader::openFrag: already open\n";
				return;
			}

			handlef = OpenShader<GL_FRAGMENT_SHADER>(fp);
		}

		//
		// Shader::openVert
		//

		void Shader::openVert(Doom::FS::File *fp)
		{
			if(handlev)
			{
				std::cerr << "Shader::openVert: already open\n";
				return;
			}

			handlev = OpenShader<GL_VERTEX_SHADER>(fp);
		}

		//
		// Shader::compile
		//

		void Shader::compile()
		{
			if(!handlef || !handlev)
			{
				std::cerr << "Shader::compile: fragment or vertex handle is 0\n";
				return;
			}

			if(program)
			{
				std::cerr << "Shader::compile: already open";
				return;
			}

			program = glCreateProgram();

			glAttachShader(program, handlef);
			glAttachShader(program, handlev);

			glLinkProgram(program);

			GLint linked;
			glGetProgramiv(program, GL_LINK_STATUS, &linked);

			if(linked != GL_TRUE)
			{
				GLsizei errlen = 0;
				GLchar err[1024];

				glGetProgramInfoLog(program, 1024, &errlen, err);

				std::cerr << "Shader::compile: " << err << '\n';
				program = 0;
			}
		}
	}
}

// EOF
