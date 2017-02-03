//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Drawing text.
//
//-----------------------------------------------------------------------------

#include "GL/Renderer/PrivData.hpp"

#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
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

      while(itr < end)
      {
         char32_t ch;
         std::tie(ch, itr) = GDCC::Core::Str8To32(itr, end);

         if(ch == '\r') px = 0;

         auto &gly = fontCurrent->getChar(ch);
         int ox = x + px + gly.ox + fontCurrent->kernAmt;
         int oy = y + py + gly.oy;

         textureBind(&gly.data);
         drawRectangle(ox, oy, ox + gly.w, oy + gly.h);

         px += gly.ax;
         py += gly.ay;
      }

      return py + fontCurrent->height; // TODO: hack
   }

   //
   // Renderer::drawText
   //
   void Renderer::drawText(int x, int y, char const *str)
   {
      if(!fontCurrent) return;

      fontCurrent->kernReset();

      float py = y + (fontCurrent->height / 2.0f); // TODO: hack
      for(char const *itr = str;;)
      {
         if(char const *end = std::strchr(itr, '\n'))
         {
            py += textLine(x, py, itr, end);
            itr = end + 1;
         }
         else
         {
            end = itr + std::strlen(itr);
            py += textLine(x, py, itr, end);
            break;
         }
      }
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   void NativeLoad_DrawText() {}

   //
   // void DGE_DrawText(int x, int y, char const *str)
   //
   DGE_Code_NativeDefn(DGE_DrawText)
   {
      int x = static_cast<int>(argv[0]);
      int y = static_cast<int>(argv[1]);
      Code::MemPtr<Code::Byte const> str = {&task->prog->memory, argv[2]};
      Renderer::Current->drawText(x, y, MemStrDup(str).get());
      return false;
   }
}

// EOF

