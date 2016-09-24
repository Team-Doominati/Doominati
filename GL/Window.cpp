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

#include "GL/DynamicBuffer.hpp"
#include "GL/Shader.hpp"
#include "GL/Texture.hpp"

#include "Core/Vector4.hpp"
#include "Core/Time.hpp"

#include "FS/Dir.hpp"

#include <GDCC/Core/Array.hpp>

#include "SDL.h"

#include <iostream>
#include <unordered_map>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static char const baseFragShader[] = R"(
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

static char const baseVertShader[] = R"(
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

namespace DGE
{
   namespace GL
   {
      //
      // TextureNoneData
      //
      static TexturePixel const TextureNoneData[4] =
         {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};
   }
}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE
{
   namespace GL
   {
      //
      // Window::PrivData
      //
      // Private instance data.
      //

      class Window::PrivData
      {
      public:
         using TextureHashmap = std::unordered_map<std::string, TextureData>;

         PrivData() = delete;
         PrivData(PrivData const &other) = delete;
         PrivData(int w, int h);

         ~PrivData();

         SDL_Window     *window;
         SDL_GLContext   gl;
         GLuint          textureCurrent;
         TextureHashmap  textures;
         DynamicBuffer   circleBuff, circleLineBuff;
      };
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE
{
   namespace GL
   {
      //
      // Window::PrivData constructor
      //

      Window::PrivData::PrivData(int w, int h) :
         window{}, gl{},
         textureCurrent{}, textures{},
         circleBuff{VertexXYUV::Layout},
         circleLineBuff{VertexXY::Layout, GL_LINE_LOOP}
      {
         int x = SDL_WINDOWPOS_UNDEFINED;
         int y = SDL_WINDOWPOS_UNDEFINED;

         // Set up window.
         if(!(window = SDL_CreateWindow("Doominati", x, y, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)))
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
         privdata{new PrivData{w_, h_}}
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
         shaderBase.reset(new Shader{baseFragShader, baseVertShader});
         shaderDrop();
         drawColorSet(1.0, 1.0, 1.0);

         // Set up basic no-texture.
         textureNone =
            &privdata->textures.emplace(std::piecewise_construct,
               std::forward_as_tuple("TEXNULL"),
               std::forward_as_tuple(2, 2, TextureNoneData)).first->second;

         // Set up VBOs.
         circlePrecision(4);
      }

      //
      // Window destructor
      //

      Window::~Window()
      {
         // Needs to exist for very specific reasons.
      }

      //
      // CalcBufSize
      //

      static void CalcBufSize(int subdivisions, std::size_t &bufsize)
      {
         if(!subdivisions) return;

         bufsize += 3;

         for(int i = 0; i < 2; i++)
            CalcBufSize(subdivisions - 1, bufsize);
      }

      //
      // Window::circleCreateLines
      //

      void Window::circleCreateLines(int subdivisions)
      {
         std::size_t bufsize = 3 * 2;

         for(int i = 0; i < 4; i++)
            CalcBufSize(subdivisions, bufsize);

         GDCC::Core::Array<VertexXY> bufarray{bufsize};

         for(std::size_t i = 0; i < bufsize; i++)
         {
            float angl = Core::Lerp(0.0f, Core::tau, i / float(bufsize));
            float s = std::sin(angl);
            float c = std::cos(angl);
            bufarray[i] = { s, c };
         }

         // Generate the VBO.
         privdata->circleLineBuff.setupData(bufarray.size(), bufarray.data(), GL_DYNAMIC_DRAW);
      }

      //
      // CalcPoint
      //

      static void CalcPoint(float angl, VertexXYUV *&buf)
      {
         float s = std::sin(angl);
         float c = std::cos(angl);
         *buf++ = { s, c, s * 0.5f + 0.5f, c * 0.5f + 0.5f };
      }

      //
      // CalcFaces
      //

      static void CalcFaces(int subdivisions, float anglA, float anglC, VertexXYUV *&buf)
      {
         if(!subdivisions) return;

         float anglB = Core::Lerp(anglA, anglC, 0.5f);

         CalcPoint(anglA, buf);
         CalcPoint(anglB, buf);
         CalcPoint(anglC, buf);

         CalcFaces(subdivisions - 1, anglA, anglB, buf);
         CalcFaces(subdivisions - 1, anglB, anglC, buf);
      }

      //
      // Window::circleCreateTris
      //

      void Window::circleCreateTris(int subdivisions)
      {
         std::size_t bufsize = 3 * 2;

         for(int i = 0; i < 4; i++)
            CalcBufSize(subdivisions, bufsize);

         GDCC::Core::Array<VertexXYUV> bufarray{bufsize};
         VertexXYUV *buf = bufarray.data();

         // First, make a diamond out of two tris.
         CalcPoint(Core::pi + Core::pi2, buf);
         CalcPoint(0, buf);
         CalcPoint(Core::pi2, buf);

         CalcPoint(Core::pi + Core::pi2, buf);
         CalcPoint(Core::pi2, buf);
         CalcPoint(Core::pi, buf);

         // Then create a fractal of triangles around that, for each quarter.
         for(int i = 0; i < 4; i++)
            CalcFaces(subdivisions, Core::pi2 * i, (Core::pi2 * i) + Core::pi2, buf);

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
      // Window::drawCircle
      //

      void Window::drawCircle(int x, int y, int radius, bool line) const
      {
         glPushMatrix();

         glMultMatrixf(Eigen::Affine3f{Eigen::Translation3f(x, y, 0.0f) * Eigen::Scaling(float(radius))}.data());

         if(!line)
            privdata->circleBuff.bindAndDraw();
         else
            privdata->circleLineBuff.bindAndDraw();


         glPopMatrix();
      }

      //
      // Window::drawEllipse
      //

      void Window::drawEllipse(int x1, int y1, int x2, int y2, bool line) const
      {
         if(x1 > x2) std::swap(x1, x2);
         if(y1 > y2) std::swap(y1, y2);

         float rx = (x2 - x1) * 0.5f;
         float ry = (y2 - y1) * 0.5f;

         glPushMatrix();

         glMultMatrixf(Eigen::Affine3f{Eigen::Translation3f(x1 + rx, y1 + ry, 0.0f) * Eigen::Scaling(rx, ry, 1.0f)}.data());

         if(!line)
            privdata->circleBuff.bindAndDraw();
         else
            privdata->circleLineBuff.bindAndDraw();

         glPopMatrix();
      }

      //
      // Window::drawLine
      //

      void Window::drawLine(int x1, int y1, int x2, int y2) const
      {
         glBegin(GL_LINES);

         glVertex2f(x1, y1);
         glVertex2f(x2, y2);

         glEnd();
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
      // Window::drawRectangle
      //

      void Window::drawRectangle(int x1, int y1, int x2, int y2, float rot, bool line) const
      {
         if(x1 > x2) std::swap(x1, x2);
         if(y1 > y2) std::swap(y1, y2);

         Core::Vector2 v[4] = {
            { float(x1), float(y1) },
            { float(x2), float(y1) },
            { float(x2), float(y2) },
            { float(x1), float(y2) }
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

         if(!line)
         {
            glBegin(GL_TRIANGLES);

            // B--A
            // | /
            // C
            glTexCoord2f(1, 0);
            glVertex2f(v[1].x, v[1].y);
            glTexCoord2f(0, 0);
            glVertex2f(v[0].x, v[0].y);
            glTexCoord2f(0, 1);
            glVertex2f(v[3].x, v[3].y);

            //    A
            //  / |
            // B--C
            glTexCoord2f(1, 0);
            glVertex2f(v[1].x, v[1].y);
            glTexCoord2f(0, 1);
            glVertex2f(v[3].x, v[3].y);
            glTexCoord2f(1, 1);
            glVertex2f(v[2].x, v[2].y);

            glEnd();
         }
         else
         {
            glBegin(GL_LINE_LOOP);

            // A--B
            // |  |
            // D--C

            for(int i = 0; i < 4; i++)
               glVertex2f(v[i].x, v[i].y);

            glEnd();
         }
      }

      //
      // Window::drawTriangle
      //

      void Window::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool line) const
      {
         if(!line)
            glBegin(GL_TRIANGLES);
         else
            glBegin(GL_LINE_LOOP);

         glTexCoord2f(0.5f, 0);
         glVertex2f(x1, y1);
         glTexCoord2f(0, 1);
         glVertex2f(x2, y2);
         glTexCoord2f(1, 1);
         glVertex2f(x3, y3);

         glEnd();
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
         drawColorSet(col.r, col.g, col.b, col.a);
      }

      //
      // Window::drawColorGet
      //

      Color Window::drawColorGet() const
      {
         return { cr, cg, cb, ca };
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
      // Window::textureBind
      //

      void Window::textureBind(TextureData *tex)
      {
         if(privdata->textureCurrent != tex->tex)
         {
            glBindTexture(GL_TEXTURE_2D, tex->tex);
            privdata->textureCurrent = tex->tex;
         }

         textureMinMax = tex->minmax;
      }

      //
      // Window::textureGet
      //
      TextureData *Window::textureGet(char const *name)
      {
         auto itr = privdata->textures.find(name);
         if(itr != privdata->textures.end())
            return &itr->second;

         if(*name == '@')
            return textureGet_File(name + 1);

         std::cerr << "unknown texture: " << name << '\n';
         return textureGet_None(name);
      }

      //
      // Window::textureGet_File
      //
      TextureData *Window::textureGet_File(char const *name)
      {
         FS::File *file = FS::Dir::FindFile(name);

         if(!file)
         {
            std::cerr << "texture file not found: " << name << '\n';
            return textureGet_None(name - 1);
         }

         try
         {
            auto loader = CreateTextureLoader(file);
            GLsizei width, height;
            std::tie(width, height) = loader->size();
            std::unique_ptr<TexturePixel[]> buf{new TexturePixel[width * height]};
            loader->data(buf.get());

            return &privdata->textures.emplace(std::piecewise_construct,
               std::forward_as_tuple(name - 1),
               std::forward_as_tuple(width, height, buf.get())).first->second;
         }
         catch(TextureLoaderError const &err)
         {
            std::cerr << "TextureLoaderError: " << name
               << ": " << err.what() << '\n';
            return textureGet_None(name - 1);
         }
      }

      //
      // Window::textureGet_None
      //
      TextureData *Window::textureGet_None(char const *name)
      {
         return &privdata->textures.emplace(std::piecewise_construct,
            std::forward_as_tuple(name),
            std::forward_as_tuple(2, 2, TextureNoneData)).first->second;
      }

      //
      // Window::textureUnbind
      //

      void Window::textureUnbind()
      {
         TextureData const *tex = textureNone;
         if(privdata->textureCurrent != tex->tex)
         {
            glBindTexture(GL_TEXTURE_2D, tex->tex);
            privdata->textureCurrent = tex->tex;
         }
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
}

// EOF

