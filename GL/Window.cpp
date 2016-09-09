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

#include "GL/Window.hpp"
#include "GL/Shader.hpp"
#include "Core/Matrix4.hpp"
#include "Core/Time.hpp"

#include <iostream>

#include "GL/gl_2_1.h"


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static char const baseFragShader[] = R"(
   #version 120

   varying vec4 color;

   void main(void)
   {
      gl_FragColor = color;
   }
)";

static char const baseVertShader[] = R"(
   #version 120

   varying vec4 color;

   void main(void)
   {
      gl_Position = ftransform();
      color = gl_Color;
   }
)";


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // Window constructor
      //

      Window::Window(int w_, int h_) :
         realw{w_}, realh{h_},
         w{1280}, h{720}
      {
         int x = SDL_WINDOWPOS_UNDEFINED;
         int y = SDL_WINDOWPOS_UNDEFINED;

         // Set up window and OpenGL context.
         if(!(window = SDL_CreateWindow("Doominati", x, y, realw, realh, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
         {
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            std::cerr << "SDL_CreateWindow: " << SDL_GetError() << '\n';
            throw EXIT_FAILURE;
         }

         if(!(gl = SDL_GL_CreateContext(window)))
         {
            SDL_DestroyWindow(window);
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            std::cerr << "SDL_GL_CreateContext: " << SDL_GetError() << '\n';
            throw EXIT_FAILURE;
         }

         // Set up OpenGL server (device).
         glClearColor(0.23f, 0.23f, 0.23f, 1.0f);

         glEnable(GL_TEXTURE_2D);
         glEnable(GL_BLEND);
         glEnable(GL_CULL_FACE); // Cull CCW-sided back facing triangles.

         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

         // Set up OpenGL client.
         glEnableClientState(GL_COLOR_ARRAY);
         glEnableClientState(GL_TEXTURE_COORD_ARRAY);
         glEnableClientState(GL_VERTEX_ARRAY);

         // Set up matrices and shaders.
         resize(w, h);

         shaderBase = new Shader{baseFragShader, baseVertShader};
         shaderDrop();
         drawColorSet(1.0, 1.0, 1.0);

         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();
      }

      //
      // Window destructor
      //

      Window::~Window()
      {
         SDL_GL_DeleteContext(gl);
         SDL_DestroyWindow(window);
      }

      //
      // Window::renderBegin
      //

      void Window::renderBegin()
      {
         // Check if window has been resized.
         {
            int newW, newH;
            SDL_GetWindowSize(window, &newW, &newH);

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
         SDL_GL_SwapWindow(window);
      }

      //
      // Window::drawColorSet
      //

      void Window::drawColorSet(float r, float g, float b, float a)
      {
         glColor4f(r, g, b, a);
         cr = r;
         cg = g;
         cb = b;
         ca = a;
      }

      //
      // Window::drawColorSet
      //

      void Window::drawColorSet(Color const &col)
      {
         drawColorSet(float(col.r), float(col.g), float(col.b), float(col.a));
      }

      //
      // Window::drawColorGet
      //

      Color Window::drawColorGet()
      {
         return { cr, cg, cb, ca };
      }

      //
      // Window::drawLine
      //

      void Window::drawLine(int x1, int y1, int x2, int y2)
      {
         glBegin(GL_LINES);

         glVertex2f(x1, y1);
         glVertex2f(x2, y2);

         glEnd();
      }

      //
      // Window::drawRectangle
      //

      void Window::drawRectangle(int x1, int y1, int x2, int y2, float rot, bool line)
      {
         if(x1 > x2) std::swap(x1, x2);
         if(y1 > y2) std::swap(y1, y2);

         Core::Vector2 v[4] = {
            {float(x1), float(y1)},
            {float(x2), float(y1)},
            {float(x2), float(y2)},
            {float(x1), float(y2)}
         };

         if(rot)
         {
            float c = std::cos(rot);
            float s = std::sin(rot);

            float sx = x1 + ((x2 - x1) / 2.0f);
            float sy = y1 + ((y2 - y1) / 2.0f);

            for(int i = 0; i < 4; i++)
            {
               float x = v[i].x - sx;
               float y = v[i].y - sy;
               v[i].x = ((x * c) - (y * s)) + sx;
               v[i].y = ((x * s) + (y * c)) + sy;
            }
         }

         if(line)
         {
            glBegin(GL_LINE_LOOP);

            // A--B
            // |  |
            // D--C

            for(int i = 0; i < 4; i++)
               glVertex2f(v[i].x, v[i].y);

            glEnd();
         }
         else
         {
            glBegin(GL_TRIANGLES);

            // B--A
            // | /
            // C
            glVertex2f(v[1].x, v[1].y);
            glVertex2f(v[0].x, v[0].y);
            glVertex2f(v[3].x, v[3].y);

            //    A
            //  / |
            // B--C
            glVertex2f(v[1].x, v[1].y);
            glVertex2f(v[3].x, v[3].y);
            glVertex2f(v[2].x, v[2].y);

            glEnd();
         }
      }

      //
      // Window::drawParticleSystem
      //

      void Window::drawParticleSystem(ParticleSystem &ps)
      {
         Core::Matrix4 mtx{};
         
         glPushMatrix();

         glLoadMatrixf(ps.mat.getConstPointer());

         float frac = Core::GetTickFract<Core::PlayTick<float>>();

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
         shaderSwap(shaderBase);
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
         glOrtho(0, w, h, 0, 0, 1);

         glMatrixMode(GL_MODELVIEW);
      }
   }
}

// EOF

