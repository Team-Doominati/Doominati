//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Drawing text.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer/PrivData.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::textLine
   //
   float Renderer::textLine(float x, float y, char const *itr, char const *end)
   {
      float px = 0, py = 0;
      while(itr <= end && *itr)
      {
         char32_t ch;
         std::tie(ch, itr) = GDCC::Core::Str8To32(itr, end);

         switch(ch)
         {
         case '\n': return py + fontCurrent->height;
         case '\r': px = 0; continue;
         }

         auto &gly = fontCurrent->getChar(ch);
         int ox = x + px + gly.ox + fontCurrent->kernAmt;
         int oy = y + py + gly.oy;

         textureBind(&gly.data);
         drawRectangle(ox, oy, ox + gly.w, oy + gly.h);

         px += gly.ax;
         py += gly.ay;
      }

      return py;
   }

   //
   // Renderer::drawText
   //
   void Renderer::drawText(int x, int y, char const *str)
   {
      if(!fontCurrent) return;

      fontCurrent->kernReset();

      float py = y + (fontCurrent->height / 2.0f); // TODO: hack
      for(char const *itr = str; *itr; itr++)
      {
         char const *end = std::strchr(itr, '\n');
         if(!end)    end = itr + std::strlen(itr);
         py += textLine(x, py, itr, end);
         itr = end;
      }
   }
}

// TODO: natives

// EOF

