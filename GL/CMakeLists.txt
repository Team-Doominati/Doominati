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
add_library(dge-gl-lib STATIC
   Color.cpp
   Color.hpp
   DynamicBuffer.cpp
   DynamicBuffer.hpp
   gl_2_1.c
   gl_2_1.h
   Particle.cpp
   Particle.hpp
   Renderer.cpp
   Renderer.hpp
   Renderer/Draw.cpp
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
   Window/PrivData.hpp
)

target_link_libraries(dge-gl-lib dge-code-lib
   ${OPENGL_LIBRARIES}
   ${SDL2_LIBRARIES})

## EOF

