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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
include(CheckCXXCompilerFlag)


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

if(   CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
   OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   TRY_CXX_COMPILER_FLAG(-Wall      FLAG_CXX_Wall     )
   TRY_CXX_COMPILER_FLAG(-Wextra    FLAG_CXX_Wextra   )
   TRY_CXX_COMPILER_FLAG(-Wpedantic FLAG_CXX_Wpedantic)
   TRY_CXX_COMPILER_FLAG(-Wshadow   FLAG_CXX_Wshadow  )

   TRY_CXX_COMPILER_FLAG(-std=c++11 FLAG_CXX_STD_CXX11)
endif()

if(MSVC)
   add_definitions(-D_SCL_SECURE_NO_WARNINGS /fp:fast
                   /wd4996 /wd4800 /wd4244 /wd4800 /wd4146)
   set(CMAKE_CXX_FLAGS_DEBUG_INIT          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
   set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
   set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")
endif()


##----------------------------------------------------------------------------|
## Environment Configuration                                                  |
##

include_directories(${CMAKE_SOURCE_DIR})

find_package(GDCC   REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2   REQUIRED)


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

add_subdirectory(doominati)

## EOF

