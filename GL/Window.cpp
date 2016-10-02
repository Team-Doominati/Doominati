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

#include "GL/Window/PrivData.hpp"

#include "GL/Shader.hpp"

#include "Core/Vector4.hpp"
#include "Core/Time.hpp"

#include <GDCC/Core/Array.hpp>

#include <iostream>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::GL
{
   //
   // BaseFragShader
   //
   static char const BaseFragShader[] = R"(
      #version 120

      uniform sampler2D dge_texture;

      varying vec4 color;
      varying vec4 texcoord;

      void main(void)
      {
         gl_FragColor = texture2D(dge_texture, texcoord.xy) * color;
   //    gl_FragColor = vec4(texcoord.xy, 0.0, 1.0);
      }
   )";

   //
   // BaseVertShader
   //
   static char const BaseVertShader[] = R"(
      #version 120

      varying vec4 color;
      varying vec4 texcoord;

      void main(void)
      {
         gl_Position = ftransform();

         texcoord = gl_MultiTexCoord0;
         color = gl_Color;
      }
   )";
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::GL
{
   //
   // Circle_CalcBufSize
   //
   static void Circle_CalcBufSize(int subdivisions, std::size_t &bufsize)
   {
      if(!subdivisions) return;

      bufsize += 3;

      for(int i = 0; i < 2; i++)
         Circle_CalcBufSize(subdivisions - 1, bufsize);
   }

   //
   // Circle_CalcPoint
   //
   static void Circle_CalcPoint(float angl, VertexXYUV *&buf)
   {
      float s = std::sin(angl);
      float c = std::cos(angl);
      *buf++ = { s, c, s * 0.5f + 0.5f, c * 0.5f + 0.5f };
   }

   //
   // Circle_CalcFaces
   //
   static void Circle_CalcFaces(int subdivisions, float anglA, float anglC, VertexXYUV *&buf)
   {
      if(!subdivisions) return;

      float anglB = Core::Lerp(anglA, anglC, 0.5f);

      Circle_CalcPoint(anglA, buf);
      Circle_CalcPoint(anglB, buf);
      Circle_CalcPoint(anglC, buf);

      Circle_CalcFaces(subdivisions - 1, anglA, anglB, buf);
      Circle_CalcFaces(subdivisions - 1, anglB, anglC, buf);
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Window::PrivData constructor
   //
   Window::PrivData::PrivData(int w_, int h_) :
      window{}, gl{},
      texBound{},
      circleBuff{VertexXYUV::Layout},
      circleLineBuff{VertexXY::Layout, GL_LINE_LOOP}
   {
      int x = SDL_WINDOWPOS_UNDEFINED;
      int y = SDL_WINDOWPOS_UNDEFINED;

      // Set up window.
      if(!(window = SDL_CreateWindow("Doominati", x, y, w_, h_, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
      {
         SDL_QuitSubSystem(SDL_INIT_VIDEO);
         std::cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
         throw EXIT_FAILURE;
      }

      // Set up OpenGL context.
      if(!(gl = SDL_GL_CreateContext(window)))
      {
         SDL_DestroyWindow(window);
         SDL_QuitSubSystem(SDL_INIT_VIDEO);
         std::cerr << "SDL_GL_CreateContext: " << SDL_GetError() << '\n';
         throw EXIT_FAILURE;
      }
   }

   //
   // Window::PrivData destructor
   //
   Window::PrivData::~PrivData()
   {
      SDL_GL_DeleteContext(gl);
      SDL_DestroyWindow(window);
   }

   //
   // Window constructor
   //
   Window::Window(int w_, int h_) :
      realw{w_}, realh{h_},
      w{1280}, h{720},
      privdata{new PrivData(w_, h_)}
   {
      // Set up OpenGL server (device).
      glClearColor(0.23f, 0.23f, 0.23f, 1.0f);

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glEnable(GL_CULL_FACE); // Cull CCW-sided back facing triangles.

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glActiveTexture(GL_TEXTURE0);

      // Set up matrices.
      resize(w, h);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      // Set up base shader.
      shaderBase.reset(new Shader{BaseFragShader, BaseVertShader});
      shaderDrop();
      drawColorSet(1.0, 1.0, 1.0);

      // Set up basic no-texture.
      textureGet_None("TEXNULL");

      // Set up VBOs.
      circlePrecision(4);
   }

   //
   // Window destructor
   //
   // Needs to exist here specifically, because of the way
   // unique_ptr works with incomplete types.
   //
   Window::~Window()
   {
   }

   //
   // Window::circleCreateLines
   //
   void Window::circleCreateLines(int subdivisions)
   {
      std::size_t bufsize = 3 * 2;

      for(int i = 0; i < 4; i++)
         Circle_CalcBufSize(subdivisions, bufsize);

      GDCC::Core::Array<VertexXY> bufarray{bufsize};

      for(std::size_t i = 0; i < bufsize; i++)
      {
         float angl = Core::Lerp(0.0f, Core::Tau, i / float(bufsize));
         float s = std::sin(angl);
         float c = std::cos(angl);
         bufarray[i] = { s, c };
      }

      // Generate the VBO.
      privdata->circleLineBuff.setupData(bufarray.size(), bufarray.data(), GL_DYNAMIC_DRAW);
   }

   //
   // Window::circleCreateTris
   //
   void Window::circleCreateTris(int subdivisions)
   {
      std::size_t bufsize = 3 * 2;

      for(int i = 0; i < 4; i++)
         Circle_CalcBufSize(subdivisions, bufsize);

      GDCC::Core::Array<VertexXYUV> bufarray{bufsize};
      VertexXYUV *buf = bufarray.data();

      // First, make a diamond out of two tris.
      Circle_CalcPoint(Core::Pi + Core::Pi2, buf);
      Circle_CalcPoint(0, buf);
      Circle_CalcPoint(Core::Pi2, buf);

      Circle_CalcPoint(Core::Pi + Core::Pi2, buf);
      Circle_CalcPoint(Core::Pi2, buf);
      Circle_CalcPoint(Core::Pi, buf);

      // Then create a fractal of triangles around that, for each quarter.
      for(int i = 0; i < 4; i++)
         Circle_CalcFaces(subdivisions, Core::Pi2 * i, (Core::Pi2 * i) + Core::Pi2, buf);

      // Generate the VBO.
      privdata->circleBuff.setupData(bufarray.size(), bufarray.data(), GL_DYNAMIC_DRAW);
   }

   //
   // Window::circlePrecision
   //
   void Window::circlePrecision(int subdivisions)
   {
      circleCreateTris(subdivisions);
      circleCreateLines(subdivisions);
   }

   //
   // Window::drawParticleSystem
   //
   void Window::drawParticleSystem(ParticleSystem const &ps)
   {
      glPushMatrix();

      glMultMatrixf(ps.mat.data());

      float frac = Core::GetTickFract<Core::PlayTick<float>>();

      textureBind(ps.texname.get());

      for(auto &p : ps.particles)
      {
         float x = Core::Lerp(p.oldposition.x, p.position.x, frac);
         float y = Core::Lerp(p.oldposition.y, p.position.y, frac);

         float sx = 8 * p.scale.x;
         float sy = 8 * p.scale.y;

         drawColorSet(p.color);
         drawRectangle(x - sx, y - sy, x + sx, y + sy, p.rot);
      }

      glPopMatrix();
   }

   //
   // Window:lineSmooth
   //
   void Window::lineSmooth(bool on)
   {
      if(on) glEnable(GL_LINE_SMOOTH);
      else   glDisable(GL_LINE_SMOOTH);
   }

   //
   // Window::lineWidth
   //
   void Window::lineWidth(int width)
   {
      glLineWidth(width);
   }

   //
   // Window::renderBegin
   //
   void Window::renderBegin()
   {
      // Check if window has been resized.
      {
         int newW, newH;
         SDL_GetWindowSize(privdata->window, &newW, &newH);

         if(w != newW || h != newH)
            resize(newW, newH);
      }

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   //
   // Window::renderEnd
   //
   void Window::renderEnd()
   {
      SDL_GL_SwapWindow(privdata->window);
   }

   //
   // Window::shaderSwap
   //
   void Window::shaderSwap(Shader *sp)
   {
      if(sp)
      {
         shaderCurrent = sp;
         glUseProgram(shaderCurrent->program);
      }
   }

   //
   // Window::shaderDrop
   //
   void Window::shaderDrop()
   {
      shaderSwap(shaderBase.get());
   }

   //
   // Window::shaderUpdate
   //
   void Window::shaderUpdate()
   {
      shaderCurrent->update();
   }

   //
   // Window::resize
   //
   void Window::resize(int w_, int h_)
   {
      realw = w_;
      realh = h_;

      glViewport(0, 0, realw, realh);

      // Reset the projection matrix with
      // orthogonal perspective and device coordinates.
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, w, h, 0, 0, 0.01f);

      glMatrixMode(GL_MODELVIEW);
   }
}

// EOF

