##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-core.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-core-lib
##
add_library(dge-core-lib STATIC
   BitStream.cpp
   BitStream.hpp
   HashMapFixed.hpp
   HashMap.hpp
   List.hpp
   Math.hpp
   NTS.cpp
   NTS.hpp
   Search.hpp
   Stack.hpp
   Store.hpp
   String.hpp
   Time.hpp
   Types.hpp
   Vector2.hpp
   Zip.cpp
   Zip.hpp
)

target_link_libraries(dge-core-lib
   ${GDCC_LIBRARIES})

## EOF

