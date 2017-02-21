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
add_library(dge-gl-lib STATIC
   Color.cpp
   Color.hpp
   DynamicBuffer.cpp
   DynamicBuffer.hpp
   Font.cpp
   Font.hpp
   Native.cpp
   OpenGL2.1.c
   OpenGL2.1.h
   Particle.cpp
   Particle.hpp
   Renderer.cpp
   Renderer.hpp
   Renderer/Draw.cpp
   Renderer/DrawText.cpp
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
   ${GDCC_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${SDL2_LIBRARIES}
   ${FREETYPE_LIBRARIES}
   ${DGE_GL_LIBRARIES})

## EOF

