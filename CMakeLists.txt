##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2019 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Root CMake file.
##
##-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.4)

cmake_policy(VERSION 3.11)

project(doominati)

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)


##----------------------------------------------------------------------------|
## Options                                                                    |
##

set(DGE_CONSOLE_APPLICATION ON  CACHE BOOL   "Build as a console application.")
set(DGE_OUT_DIR             bin CACHE STRING "Directory for target output.")


##----------------------------------------------------------------------------|
## CMake Configuration                                                        |
##

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DGE_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DGE_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DGE_OUT_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "release")
endif()

set(CMAKE_CXX_STANDARD 17)


##----------------------------------------------------------------------------|
## Functions                                                                  |
##

##
## DGE_INSTALL_PART
##
function(DGE_INSTALL_PART name wantExe wantLib)
   if(wantExe AND DGE_INSTALL_EXE)
      install(TARGETS dge-${name}
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
      )
   endif()

   if(wantLib AND DGE_INSTALL_LIB)
      if(DGE_INSTALL_API)
         install(TARGETS dge-${name}-lib
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
         )
      elseif(DGE_SHARED)
         install(TARGETS dge-${name}-lib
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
         )
      endif()
   endif()
endfunction()

##
## Try_CXX_Compiler_Flag
##
function(Try_CXX_Compiler_Flag flag name)
   check_cxx_compiler_flag(${flag} ${name})

   if(${name})
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
   endif()
endfunction()

##
## Add_Optional_Library
##
function(Add_Optional_Library out def found lib inc)
   if(${found})
      set(${out} "${${out}};${lib}" PARENT_SCOPE)
      include_directories(SYSTEM ${inc})
      add_definitions("-D${def}=1")
   endif()
endfunction()


##----------------------------------------------------------------------------|
## Environment Detection                                                      |
##

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   Try_CXX_Compiler_Flag(-Wall       FLAG_CXX_Wall)
   Try_CXX_Compiler_Flag(-Wextra     FLAG_CXX_Wextra)
   Try_CXX_Compiler_Flag(-ffast-math FLAG_CXX_ffast_math)

   Try_CXX_Compiler_Flag(-Wno-implicit-fallthrough FLAG_CXX_Wno_implicit_fallthrough)
endif()

if(MSVC)
   ## Fucking MSVC.
   add_definitions(
      /fp:fast
      /std:c++latest
      -D_SCL_SECURE_NO_WARNINGS
      /wd4996  ## Deprecation warnings
      /wd4800  ## Boolean casting
      /wd4244  ## Forcing argument to correct type
      /wd4146  ## Unary minus on unsigned
      /wd4805  ## Casting issues
      /wd4819) ## Unicode bullshit

   set(CMAKE_CXX_FLAGS_DEBUG_INIT          "/MP4 /MTd /Zi /Od /Ob0 /RTC1 /D_DEBUG")
   set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MP4 /MT      /O1 /Ob1       /D NDEBUG")
   set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MP4 /MT      /O2 /Ob2       /D NDEBUG")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MP4 /MT  /Zi /O2 /Ob1       /D NDEBUG")
endif()

check_cxx_source_compiles("
   int main()
   {
      auto ptr = &&label;

      goto *ptr;
      return 1;

   label:
      return 0;
   }"
   Has_GNU_Dynamic_Goto)

if(Has_GNU_Dynamic_Goto)
   add_definitions(-DDGE_Code_GNUC_DynamicGoto=1)
endif()


##----------------------------------------------------------------------------|
## Variables                                                                  |
##

##
## DGE_INSTALL_API
##
if(NOT DEFINED DGE_INSTALL_API)
   set(DGE_INSTALL_API ON CACHE BOOL "Install headers.")
endif()

##
## DGE_INSTALL_DOC
##
if(NOT DEFINED DGE_INSTALL_DOC)
   set(DGE_INSTALL_DOC ON CACHE BOOL "Install documentation.")
endif()

##
## DGE_INSTALL_EXE
##
if(NOT DEFINED DGE_INSTALL_EXE)
   set(DGE_INSTALL_EXE ON CACHE BOOL "Install executables.")
endif()

##
## DGE_INSTALL_LIB
##
if(NOT DEFINED DGE_INSTALL_LIB)
   set(DGE_INSTALL_LIB ON CACHE BOOL "Install libraries.")
endif()

##
## DGE_SHARED
##
## If true (or equivalent), libraries will be built as SHARED. Otherwise,
## they are built as STATIC.
##
if(NOT DEFINED DGE_SHARED)
   set(DGE_SHARED OFF CACHE BOOL "Build libraries as shared objects.")
endif()

##
## DGE_SHARED_DECL
##
## Used internally for convenience in add_library commands.
##
if(DGE_SHARED)
   set(DGE_SHARED_DECL SHARED)
else()
   set(DGE_SHARED_DECL STATIC)
endif()


##----------------------------------------------------------------------------|
## Environment Configuration                                                  |
##

##
## Required packages
##
find_package(Eigen3   REQUIRED)
find_package(Freetype REQUIRED)
find_package(GDCC     REQUIRED)
find_package(OpenAL   REQUIRED)
find_package(OpenGL   REQUIRED)
find_package(SDL2     REQUIRED)
find_package(Threads  REQUIRED)

## Global includes
include_directories(${CMAKE_SOURCE_DIR})

include_directories(SYSTEM
   ${EIGEN3_INCLUDE_DIRS}
   ${FREETYPE_INCLUDE_DIRS}
   ${GDCC_INCLUDE_DIRS}
   ${OPENAL_INCLUDE_DIR}
   ${OPENGL_INCLUDE_DIR}
   ${SDL2_INCLUDE_DIRS})

##
## Optional packages
##
find_package(FLAC)
find_package(Opus)
find_package(Vorbis)
find_package(JPEG)
find_package(OpenMP)

if(${OPENMP_FOUND})
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

## Optional defs for target AL
Add_Optional_Library(DGE_AL_LIBRARIES DGE_AL_Use_FLAC
   "${FLAC_FOUND}" "${FLAC_LIBRARIES}" "${FLAC_INCLUDE_DIRS}")
Add_Optional_Library(DGE_AL_LIBRARIES DGE_AL_Use_Opus
   "${OPUS_FOUND}" "${OPUS_LIBRARIES}" "${OPUS_INCLUDE_DIRS}")
Add_Optional_Library(DGE_AL_LIBRARIES DGE_AL_Use_Vorbis
   "${VORBIS_FOUND}" "${VORBIS_LIBRARIES}" "${VORBIS_INCLUDE_DIRS}")

## Optional defs for target GL
Add_Optional_Library(DGE_GL_LIBRARIES DGE_GL_Use_JPEG
   "${JPEG_FOUND}" "${JPEG_LIBRARIES}" "${JPEG_INCLUDE_DIR}")

##
## Non-package libraries
##
find_library(STDCXXFS stdc++fs)

##
## Entry point
##
if(NOT DGE_CONSOLE_APPLICATION AND NOT SDL2_MAIN_FOUND)
   set(DGE_CONSOLE_APPLICATION TRUE)
endif()

if(NOT DGE_CONSOLE_APPLICATION)
   set(DGE_MAIN_LIBRARIES ${SDL2_MAIN_LIBRARIES})
   set(DGE_MAIN_ENTRY WIN32)
else()
   add_definitions(-DDGE_MAIN_HANDLED=1)
   unset(DGE_MAIN_LIBRARIES)
   unset(DGE_MAIN_ENTRY)
endif()


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

add_subdirectory(AL)
add_subdirectory(Code)
add_subdirectory(Core)
add_subdirectory(Defs)
add_subdirectory(doominati)
add_subdirectory(FS)
add_subdirectory(GL)
add_subdirectory(Game)
add_subdirectory(Sh)

if(DGE_INSTALL_API)
   install(DIRECTORY AL Code Core Defs FS GL Game Sh
      DESTINATION include/DGE
      FILES_MATCHING PATTERN "*.hpp")
endif()

if(DGE_INSTALL_DOC)
   install(DIRECTORY doc/ DESTINATION share/dge/doc)
   install(FILES COPYING DESTINATION share/dge/doc)
endif()

## EOF

