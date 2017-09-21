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

#include "GL/Renderer.hpp"

#include "Code/Convert.hpp"
#include "Code/MemStr.hpp"
#include "Code/Program.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Type Definitions                                                           |
//

namespace DGE::GL
{
   //
   // TextLine
   //
   struct TextLine
   {
      float       w;
      char const *beg, *end;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Renderer::drawText
   //
   void Renderer::drawText(float x, float y, char const *str, float maxwidth)
   {
      if(!fntBound || !str || !*str) return;

      TextureSaver texSave{*this, texBound};

      FontFace &fnt = fntBound->data;

      // Split text into lines.
      std::vector<TextLine> lines;
      float maxw = 0;

      for(char const *itr = str, *end = str + std::strlen(str); itr < end;)
      {
         char const *beg = itr, *lend;
         float lnw = 0;
         fnt.kernReset();

         // Parse the line, getting its width, beginning and ending.
         for(;;)
         {
            char const *nex;
            char32_t ch; std::tie(ch, nex) = GDCC::Core::Str8To32(itr, end);

            if(ch == '\n') {lend = itr; itr = nex; break;}

            auto &gly = fnt.getChar(ch);
            auto  glw = gly.ax + fnt.getKernAmount();

            if(maxwidth && lnw + glw > maxwidth)
               {lend = itr; break;}
            else
               lnw += glw;

            if(nex >= end) {lend = itr = end; break;}
            else                   itr = nex;
         }

         if(lnw > maxw) maxw = lnw;
         lines.emplace_back(TextLine{lnw, beg, lend});
      }

      // Get dimensions.
      float height = fnt.getHeight() * lines.size();
      float cx, cy = fnt.getHeight();
      float ox;

      // Set up vertical alignment.
      switch(textAlignV)
      {
      default:
      case AlignVert::Top:    cy += y;                   break;
      case AlignVert::Bottom: cy += y -  height;         break;
      case AlignVert::Center: cy += y - (height / 2.0f); break;
      }

      for(auto &ln : lines)
      {
         // Set up horizontal alignment.
         fnt.kernReset();

         switch(textAlignH)
         {
         default:
         case AlignHorz::Left:      ox = cx = x;                          break;
         case AlignHorz::Right:     ox = cx = x -          ln.w;          break;
         case AlignHorz::Center:    ox = cx = x -         (ln.w / 2.0f);  break;
         case AlignHorz::RightBox:  ox = cx = x + (maxw -  ln.w);         break;
         case AlignHorz::CenterBox: ox = cx = x + (maxw - (ln.w / 2.0f)); break;
         }

         // Draw each character in the line, advancing the cursor position.
         for(char const *itr = ln.beg; itr < ln.end;)
         {
            char32_t ch; std::tie(ch, itr) = GDCC::Core::Str8To32(itr, ln.end);

            if(ch == '\r')
               {cx = ox; continue;}

            auto &gly = fnt.getChar(ch);

            auto kern = fnt.getKernAmount();
            auto dx = cx + gly.ox + kern;
            auto dy = cy + gly.oy;

            textureBind(texMan.get(gly.tex));
            drawRectangle(dx, dy, dx + gly.w, dy + gly.h);

            cx += gly.ax + kern;
         }

         cy += fnt.getHeight();
      }
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::GL
{
   DGE_Code_NativeLoaderDefn(Renderer_DrawText);

   //
   // void DGE_Draw_Text(short _Accum x, y, char const *str[,
   //    short _Accum maxwidth])
   //
   DGE_Code_NativeDefn(DGE_Draw_Text)
   {
      auto x = Code::SAccumToHost(argv[0]);
      auto y = Code::SAccumToHost(argv[1]);
      auto maxwidth = argc > 3 ? Code::SAccumToHost(argv[3]) : 0;

      Code::MemPtr<Code::Byte const> str = {&task->prog->memory, argv[2]};

      Renderer::GetCurrent()->drawText(x, y, MemStrDup(str).get(), maxwidth);
      return false;
   }

   //
   // void DGE_Draw_SetTextAlignment(int h[, int v])
   //
   DGE_Code_NativeDefn(DGE_Draw_SetTextAlignment)
   {
      AlignHorz h = static_cast<AlignHorz>(argv[0]);
      if(h != AlignHorz::Keep) Renderer::GetCurrent()->textAlignH = h;

      if(argc > 1)
      {
         AlignVert v = static_cast<AlignVert>(argv[1]);
         if(v != AlignVert::Keep) Renderer::GetCurrent()->textAlignV = v;
      }

      return false;
   }
}

// EOF

