//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Renderer private data class.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Renderer__PrivData_H__
#define DGE__GL__Renderer__PrivData_H__

#include "GL/Renderer.hpp"

#include "GL/DynamicBuffer.hpp"
#include "GL/Texture.hpp"

#include "Core/ResourceManager.hpp"

#include "SDL.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // Renderer::PrivData
   //
   class Renderer::PrivData
   {
   public:
      PrivData() : texBound{nullptr} {}
      PrivData(PrivData const &other) = delete;

      Core::ResourceManager<TextureData> texMan;
      TextureData                 const *texBound;

      DynamicBuffer const *circleBuff, *circleLineBuff;

      std::unordered_map<int, DynamicBuffer> circleBuffers, circleLineBuffers;
   };
}

#endif//DGE__GL__Renderer__PrivData_H__

