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
#include "GL/gl_2_1.h"

#include "Core/Vector4.hpp"
#include "Core/Time.hpp"

#include "FS/Dir.hpp"

#include <GDCC/Core/Array.hpp>

#include "SDL.h"

#include <GL/glu.h>

#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
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


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // TextureLoadError
      //

      class TextureLoadError : public std::runtime_error
      {
         using std::runtime_error::runtime_error;
      };

      //
      // Texture
      //

      class Texture
      {
      public:
         Texture() = delete;
         Texture(Texture const &other) = default;
         Texture(Texture &&other) = default;
         Texture(GLsizei width, GLsizei height, float *texdata);
         Texture(char const *name);

//       void loadPBM(FS::File *fp);
//       void loadPGM(FS::File *fp);
//       void loadPNG(FS::File *fp);
         void loadPPM(FS::File *fp);

         void genTexture(GLsizei width, GLsizei height, float *texdata);

         GLuint        textureid;
         Core::Vector4 minmax;
      };

      //
      // Texture constructor
      //

      Texture::Texture(char const *name)
      {
         FS::File *fp = FS::Dir::FindFile(name);

         if(!fp)
            throw TextureLoadError("no file");

         switch(fp->format)
         {
//       case FS::Format::PBM: loadPBM(fp); break;
//       case FS::Format::PGM: loadPGM(fp); break;
//       case FS::Format::PNG: loadPNG(fp); break;
         case FS::Format::PPM: loadPPM(fp); break;
         default:
            throw TextureLoadError("invalid file format");
         }
      }

      Texture::Texture(GLsizei width, GLsizei height, float *texdata)
      {
         genTexture(width, height, texdata);
      }

      //
      // Texture::loadPPM
      //

      void Texture::loadPPM(FS::File *fp)
      {
         char const *data = fp->data;

         if(data[1] == '6')
            throw TextureLoadError("P6 binary PPM format not supported");

         // skip header
         data += 2;

         std::unique_ptr<float[]> texdata;

         long width  = 0;
         long height = 0;
         long range  = 255;

         for(std::size_t i = 0, itnum = 3; i < itnum;)
         {
            // skip whitespace
            while(std::isspace(*data)) ++data;

            // skip comments
            if(*data == '#')
            {
               ++data;
               while(*data != '\n') ++data;
               continue;
            }

            // parse number
            char *end;
            long num = std::strtol(data, &end, 10);

            // what the hell did you DO even
            if(!std::isspace(*end) && !std::isdigit(*end) && *end != '#')
               throw TextureLoadError("invalid character in PPM file");

            switch(i)
            {
            case 0:
               width  = num; // first number is width
               break;

            case 1:
               height = num;                // second is height
               itnum += width * height * 3; // now we know how much to process
               texdata.reset(new float[width * height * 3]);
               break;

            case 2:
               range = num; // and finally the maximum value.
               break;

            default:
               texdata[i - 3] = num / float(range);
               break;
            }

            // carry on here.
            data = end;
            ++i;
         }

         genTexture(width, height, texdata.get());
      }

      //
      // Texture::genTexture
      //

      void Texture::genTexture(GLsizei width, GLsizei height, float *texdata)
      {
         glGenTextures(1, &textureid);
         glBindTexture(GL_TEXTURE_2D, textureid);

         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, texdata);

         gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_FLOAT, texdata);
      }

      //
      // Window::PrivData
      //
      // Private instance data.
      //

      class Window::PrivData
      {
      public:
         using TextureHashmap = std::unordered_map<std::string, Texture>;

         PrivData() = delete;
         PrivData(PrivData const &other) = delete;
         PrivData(int w, int h);

         ~PrivData();

         SDL_Window     *window;
         SDL_GLContext   gl;
         GLuint          textureCurrent;
         TextureHashmap  textures;
         DynamicBuffer   circleBuff;
      };

      //
      // Window::PrivData constructor
      //

      Window::PrivData::PrivData(int w, int h) :
         window{}, gl{},
         textureCurrent{}, textures{},
         circleBuff{}
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

         // Set up OpenGL client.
         glEnableClientState(GL_COLOR_ARRAY);
         glEnableClientState(GL_TEXTURE_COORD_ARRAY);
         glEnableClientState(GL_VERTEX_ARRAY);

         // Set up matrices.
         resize(w, h);

         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();

         // Set up base shader.
         shaderBase.reset(new Shader{baseFragShader, baseVertShader});
         shaderDrop();
         drawColorSet(1.0, 1.0, 1.0);

         // Set up basic no-texture.
         {
            float buff[12] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
            privdata->textures.emplace(std::piecewise_construct,
               std::forward_as_tuple("#NOTEXTURE"),
               std::forward_as_tuple(2, 2, buff));
         }

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
      // CalcPoint
      //

      static void CalcPoint(float angl, Vertex *&buf)
      {
         float s = std::sin(angl);
         float c = std::cos(angl);
         *buf++ = { s, c, 0 };
      }

      //
      // CalcFaces
      //

      static void CalcFaces(int subdivisions, float anglA, float anglC, Vertex *&buf)
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
      // Window::circlePrecision
      //

      void Window::circlePrecision(int subdivisions)
      {
         std::size_t bufsize = 0;

         std::function<void(int)> calcBufSizeFaces;
         calcBufSizeFaces = [&] (int mysubdivisions)
         {
            if(!mysubdivisions) return;

            bufsize += 3;

            for(int i = 0; i < 2; i++)
               calcBufSizeFaces(mysubdivisions - 1);
         };

         for(int i = 0; i < 4; i++)
         {
            bufsize += 3;
            calcBufSizeFaces(subdivisions);
         }

         GDCC::Core::Array<Vertex> bufarray{bufsize};
         Vertex *buf = bufarray.data();
         float angl = 0.0f;

         for(int i = 0; i < 4; i++)
         {
            *buf++ = { 0, 0, 0 };
            CalcPoint(              angl,                   buf);
            CalcPoint(                    angl + Core::pi2, buf);
            CalcFaces(subdivisions, angl, angl + Core::pi2, buf);
            angl += Core::pi2;
         }

         auto &vbo = privdata->circleBuff.buffer;

         if(!vbo)
            glGenBuffers(1, &vbo);

         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * bufsize, nullptr, GL_DYNAMIC_DRAW);
         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * bufsize, bufarray.data());

         privdata->circleBuff.size = bufsize;
      }

      //
      // Window::drawCircle
      //

      void Window::drawCircle(int x, int y, int radius)
      {
         glPushMatrix();

         glLoadMatrixf(Core::Matrix4{}.scale(radius, radius).translate(x, y).getConstPointer());

         glVertexPointer(3, GL_FLOAT, 0, nullptr);
         glEnableClientState(GL_VERTEX_ARRAY);
         glDrawArrays(GL_TRIANGLES, 0, privdata->circleBuff.size);

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
      // Window::textureSet
      //

      void Window::textureSet(char const *name)
      {
         auto it = privdata->textures.find(name);
         Texture *tex;

         if(it == privdata->textures.end())
         {
            try
            {
               it = privdata->textures.emplace(name, name).first;
            }
            catch(TextureLoadError err)
            {
               std::cerr << "TextureLoadError: " << err.what() << '\n';
               it = privdata->textures.emplace(name, privdata->textures.at("#NOTEXTURE")).first;
            }
         }

         tex = &it->second;

         if(privdata->textureCurrent != tex->textureid)
         {
            glBindTexture(GL_TEXTURE_2D, tex->textureid);
            privdata->textureCurrent = tex->textureid;
         }

         textureMinMax = tex->minmax;
      }

      //
      // Window::textureUnbind
      //

      void Window::textureUnbind()
      {
         Texture const &tex = privdata->textures.at("#NOTEXTURE");
         if(privdata->textureCurrent != tex.textureid)
         {
            glBindTexture(GL_TEXTURE_2D, tex.textureid);
            privdata->textureCurrent = tex.textureid;
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
         glOrtho(0, w, h, 0, 0, 1);

         glMatrixMode(GL_MODELVIEW);
      }
   }
}

// EOF

