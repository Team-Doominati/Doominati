//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Map geometry sectors.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Sector_H__
#define DGE__Game__Sector_H__

#include "Game/Object.hpp"
#include "Game/Types.hpp"

#include "Code/Types.hpp"

#include "Core/ListLinkVector.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Sector_GetMemberCases
//
#define DGE_Game_Sector_GetMemberCases() \
   DGE_Game_Object_GetMemberCases(); \
   case ObjectMember::frictair: return frictair.raw(); \
   case ObjectMember::friction: return friction.raw(); \
   case ObjectMember::gx:       return gx.raw(); \
   case ObjectMember::gy:       return gy.raw(); \
   case ObjectMember::gz:       return gz.raw(); \
   case ObjectMember::pc:       return pc; \
   case ObjectMember::texc:     return texc; \
   case ObjectMember::texf:     return texf; \
   case ObjectMember::xl:       return xl.raw(); \
   case ObjectMember::xu:       return xu.raw(); \
   case ObjectMember::yl:       return yl.raw(); \
   case ObjectMember::yu:       return yu.raw(); \
   case ObjectMember::zl:       return zl.raw(); \
   case ObjectMember::zu:       return zu.raw()

//
// DGE_Game_Sector_SetMemberCases
//
#define DGE_Game_Sector_SetMemberCases() \
   DGE_Game_Object_SetMemberCases(); \
   case ObjectMember::frictair: frictair.raw(val); break; \
   case ObjectMember::friction: friction.raw(val); break; \
   case ObjectMember::gx:       gx.raw(val);       break; \
   case ObjectMember::gy:       gy.raw(val);       break; \
   case ObjectMember::gz:       gz.raw(val);       break; \
   case ObjectMember::pc:       (void)val;         break; \
   case ObjectMember::texc:     texc = val;        break; \
   case ObjectMember::texf:     texf = val;        break; \
   case ObjectMember::xl:       (void)val;         break; \
   case ObjectMember::xu:       (void)val;         break; \
   case ObjectMember::yl:       (void)val;         break; \
   case ObjectMember::yu:       (void)val;         break; \
   case ObjectMember::zl:       zl.raw(val);       break; \
   case ObjectMember::zu:       zu.raw(val);       break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // Sector
   //
   class Sector : public Object
   {
      DGE_Game_ObjectPreamble(Sector, Object);

   public:
      void calcBounds();

      Core::ListLinkVector<Sector> blockLinks;

      std::size_t const pc;
      Point2     *const pv;

      Code::Word texc, texf;

      Fract frictair;
      Fract friction;

      Coord gx, gy, gz; // Gravity (X/Y/Z)
      Coord xl, yl, xu, yu;
      Coord zl, zu;

      bool rect : 1;


      static Sector *Create(std::size_t pc, std::size_t ext);

   private:
      Sector(std::size_t pc, Point2 *pv);
   };
}

#endif//DGE__Game__Sector_H__

