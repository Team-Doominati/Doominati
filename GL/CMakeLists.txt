##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-gl.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-gl-lib
##
add_library(dge-gl-lib ${DGE_SHARED_DECL}
   Defs.cpp
   Defs.hpp
   DynamicBuffer.cpp
   DynamicBuffer.hpp
   Font.cpp
   Font.hpp
   OpenGL2.1.c
   OpenGL2.1.h
   Renderer.cpp
   Renderer.hpp
   Renderer/Draw.cpp
   Renderer/DrawText.cpp
   Renderer/Font.cpp
   Renderer/Shader.cpp
   Renderer/Texture.cpp
   ShaderData.cpp
   ShaderData.hpp
   Texture.cpp
   Texture.hpp
   Texture/JPEG.cpp
   Texture/PAM.cpp
   Texture/PNG.cpp
   Types.hpp
   Window.cpp
   Window.hpp
)

target_link_libraries(dge-gl-lib
   dge-code-lib
   dge-core-lib
   ${FREETYPE_LIBRARIES}
   ${GDCC_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${SDL2_LIBRARIES}
   ${DGE_GL_LIBRARIES})

DGE_INSTALL_PART(gl FALSE TRUE)

## EOF

