##-----------------------------------------------------------------------------
##
## Copyright (C) 2016 Team Doominati
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
add_library(dge-gl-lib SHARED
   Color.cpp
   Color.hpp
   DynamicBuffer.cpp
   DynamicBuffer.hpp
   Font.cpp
   Font.hpp
   OpenGL2.1.c
   OpenGL2.1.h
   Particle.cpp
   Particle.hpp
   Renderer.cpp
   Renderer.hpp
   Renderer/Draw.cpp
   Renderer/DrawText.cpp
   Renderer/PrivData.hpp
   Renderer/Texture.cpp
   Shader.cpp
   Shader.hpp
   Texture.cpp
   Texture.hpp
   Texture/PAM.cpp
   Texture/PNG.cpp
   Window.cpp
   Window.hpp
)

target_link_libraries(dge-gl-lib dge-code-lib
   ${OPENGL_LIBRARIES}
   ${SDL2_LIBRARIES}
   ${FREETYPE_LIBRARIES})

## EOF

