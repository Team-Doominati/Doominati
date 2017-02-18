##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
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
   AudioRenderer.cpp
   AudioRenderer.hpp
   AudioRenderer/Sound.cpp
   OpenAL.hpp
   Sound.cpp
   Sound.hpp
   Sound/WAVE.cpp
   SoundSource.cpp
   SoundSource.hpp
   SoundSourceDynamic.hpp
   SoundSourceFixed.hpp
)

target_link_libraries(dge-al-lib
   dge-code-lib
   dge-core-lib
   ${GDCC_LIBRARIES}
   ${OPENAL_LIBRARY})

## EOF

