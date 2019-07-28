##-----------------------------------------------------------------------------
##
## Copyright (C) 2017-2019 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-al.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-al-lib
##
add_library(dge-al-lib STATIC
   OpenAL.hpp
   Renderer.cpp
   Renderer.hpp
   Renderer/Sound.cpp
   Sound.cpp
   Sound.hpp
   Sound/FLAC.cpp
   Sound/Opus.cpp
   Sound/Vorbis.cpp
   Sound/WAVE.cpp
   SoundSource.cpp
   SoundSource.hpp
   SoundSourceDynamic.hpp
   SoundSourceFixed.hpp
   Types.hpp
)

target_link_libraries(dge-al-lib
   dge-code-lib
   dge-core-lib
   ${GDCC_LIBRARIES}
   ${OPENAL_LIBRARY}
   ${DGE_AL_LIBRARIES})

## EOF

