##-----------------------------------------------------------------------------
##
## Copyright (C) 2016 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Root CMake file.
##
##-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.4)

project(doominati)

include(CheckCXXCompilerFlag)


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

if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
endif()


##----------------------------------------------------------------------------|
## Functions                                                                  |
##

##
## Try_CXX_Compiler_Flag
##
function(Try_CXX_Compiler_Flag flag name)
   CHECK_CXX_COMPILER_FLAG(${flag} ${name})

   if(${name})
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
   endif()
endfunction()

##
## Add_Optional_Library
##
function(Add_Optional_Library out def lib libout libinc)
   find_package(${lib} QUIET)

   if(${${lib}_FOUND})
      set(${out} "${${out}};${${lib}_${libout}}" PARENT_SCOPE)
      include_directories(SYSTEM ${${lib}_${libinc}})
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
   Try_CXX_Compiler_Flag(-Wshadow    FLAG_CXX_Wshadow)
   Try_CXX_Compiler_Flag(-ffast-math FLAG_CXX_ffast_math)

   Try_CXX_Compiler_Flag(-std=c++1z FLAG_CXX_STD_CXX17)
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


##----------------------------------------------------------------------------|
## Environment Configuration                                                  |
##

##
## Required libraries
##
find_package(Eigen3   REQUIRED)
find_package(GDCC     REQUIRED)
find_package(OpenAL   REQUIRED)
find_package(OpenGL   REQUIRED)
find_package(SDL2     REQUIRED)
find_package(Freetype REQUIRED)

## Global includes
include_directories(${CMAKE_SOURCE_DIR})

include_directories(SYSTEM
   ${EIGEN3_INCLUDE_DIRS}
   ${GDCC_INCLUDE_DIRS}
   ${OPENAL_INCLUDE_DIR}
   ${OPENGL_INCLUDE_DIR}
   ${SDL2_INCLUDE_DIRS}
   ${FREETYPE_INCLUDE_DIRS})

##
## Optional libraries
##
Add_Optional_Library(DGE_GL_LIBRARIES DGE_USE_JPEG JPEG LIBRARIES INCLUDE_DIR)
Add_Optional_Library(DGE_AL_LIBRARIES DGE_USE_FLAC FLAC LIBRARIES INCLUDE_DIRS)

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
add_subdirectory(doominati)
add_subdirectory(FS)
add_subdirectory(GL)
add_subdirectory(Game)

## EOF

