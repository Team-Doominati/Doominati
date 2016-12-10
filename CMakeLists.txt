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

cmake_minimum_required(VERSION 3.0)

project(doominati)

include(CheckCXXCompilerFlag)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)

if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
endif()


##----------------------------------------------------------------------------|
## Functions                                                                  |
##

##
## TRY_CXX_COMPILER_FLAG
##
function(TRY_CXX_COMPILER_FLAG flag name)
   CHECK_CXX_COMPILER_FLAG(${flag} ${name})

   if(${name})
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
   endif()
endfunction()


##----------------------------------------------------------------------------|
## Environment Detection                                                      |
##

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   TRY_CXX_COMPILER_FLAG(-Wall       FLAG_CXX_Wall)
   TRY_CXX_COMPILER_FLAG(-Wextra     FLAG_CXX_Wextra)
   TRY_CXX_COMPILER_FLAG(-Wshadow    FLAG_CXX_Wshadow)
   TRY_CXX_COMPILER_FLAG(-ffast-math FLAG_CXX_ffast_math)

   TRY_CXX_COMPILER_FLAG(-std=c++1z FLAG_CXX_STD_CXX17)
endif()

if(MSVC)
   # Fucking MSVC.
   add_definitions(-D_SCL_SECURE_NO_WARNINGS /fp:fast /std:c++latest)
   add_definitions(/wd4996 /wd4800 /wd4244 /wd4146 /wd4055 /wd4054 /wd4805)
#  set(CMAKE_CXX_FLAGS_DEBUG_INIT          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
   set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
   set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")

   if(CMAKE_BUILD_TYPE MATCHES DEBUG)
      message(SEND_ERROR
         "Debug builds with MSVC don't work due to bugs in MSVC's STL."
         "Please select a different build type. (RelWithDebInfo works.)")
   endif()
endif()


##----------------------------------------------------------------------------|
## Environment Configuration                                                  |
##

set(DGE_CONSOLE_APPLICATION true CACHE BOOL "Build as a console application.")
set(DGE_OUT_DIR Bin CACHE STRING "Directory for targets to output to.")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DGE_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DGE_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DGE_OUT_DIR})

find_package(Eigen3 REQUIRED)
find_package(GDCC   REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2   REQUIRED)

if(NOT DGE_CONSOLE_APPLICATION AND NOT SDL2_MAIN_FOUND)
   message(FATAL_ERROR
      "SDL2main not found, and not compiling as console application.\n"
      "Please either set SDL2_MAIN_LIBRARIES or set DGE_CONSOLE_APPLICATION.")
elseif(NOT DGE_CONSOLE_APPLICATION)
   set(DGE_MAIN_LIBRARIES ${SDL2_MAIN_LIBRARIES})
   set(DGE_MAIN_ENTRY WIN32)
else()
   add_definitions(-DDGE_MAIN_HANDLED=1)
   unset(DGE_MAIN_LIBRARIES)
   unset(DGE_MAIN_ENTRY)
endif()

include_directories(${CMAKE_SOURCE_DIR})

include_directories(SYSTEM
   ${GDCC_INCLUDE_DIRS}
   ${OPENGL_INCLUDE_DIR}
   ${SDL2_INCLUDE_DIRS}
   ${EIGEN3_INCLUDE_DIRS})


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

add_subdirectory(Code)
add_subdirectory(Core)
add_subdirectory(doominati)
add_subdirectory(FS)
add_subdirectory(GL)
add_subdirectory(Game)

## EOF

