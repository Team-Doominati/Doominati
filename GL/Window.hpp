//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Graphics Library environment.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Window_H__
#define DGE__GL__Window_H__

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   class Shader;
   class TextureData;

   //
   // Window
   //
   class Window
   {
   public:
      Window(int w, int h);
      ~Window();

      void renderBegin();
      void renderEnd();

      int w, h;

      static Window *Current;

   private:
      class PrivData;

      void resize(int w, int h);

      std::unique_ptr<PrivData> privdata;
   };
}

#endif//DGE__GL__Window_H__

