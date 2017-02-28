//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Positioned Thinkers.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__PointThinker_H__
#define DGE__Game__PointThinker_H__

#include "Game/Thinker.hpp"
#include "Game/Types.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_PointThinker_GetMemberCases
//
#define DGE_Game_PointThinker_GetMemberCases() \
   DGE_Game_Thinker_GetMemberCases(); \
   case ThinkerMember::pitch: return pitch.raw(); \
   case ThinkerMember::x:     return x.raw(); \
   case ThinkerMember::y:     return y.raw(); \
   case ThinkerMember::yaw:   return yaw.raw(); \
   case ThinkerMember::z:     return z.raw()

//
// DGE_Game_PointThinker_SetMemberCases
//
#define DGE_Game_PointThinker_SetMemberCases() \
   DGE_Game_Thinker_SetMemberCases(); \
   case ThinkerMember::pitch: pitch.raw(val); break; \
   case ThinkerMember::x:     x.raw(val);     break; \
   case ThinkerMember::y:     y.raw(val);     break; \
   case ThinkerMember::yaw:   yaw.raw(val);   break; \
   case ThinkerMember::z:     z.raw(val);     break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // PointThinker
   //
   class PointThinker : public Thinker
   {
      DGE_Game_ThinkerPreamble(PointThinker, Thinker);

   public:
      PointThinker() : pitch{0}, yaw{0}, x{0}, y{0}, z{0} {}

      Angle pitch, yaw;
      Coord x, y, z;
   };
}

#endif//DGE__Game__PointThinker_H__

