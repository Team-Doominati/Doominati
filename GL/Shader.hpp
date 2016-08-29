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

#include "SDL_opengl.h"


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
		// Shader
		//

		class Shader
		{
			friend class Window;

		public:
			Shader() = delete;
			Shader(Doom::FS::File *ffp, Doom::FS::File *vfp);
			Shader(Shader &other) = delete;
			Shader(Shader const &&other);

			void openFrag(Doom::FS::File *fp);
			void openVert(Doom::FS::File *fp);
			void compile();

		protected:
			GLuint handlef;
			GLuint handlev;
			GLuint program;
		};
	}
}

#endif//Doom__GL__Shader_H__
