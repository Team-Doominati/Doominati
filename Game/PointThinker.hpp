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

#include "../Game/Thinker.hpp"
#include "../Game/Types.hpp"

#include "../Core/Fixed.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_PointThinker_GetMemberCases
//
#define DGE_Game_PointThinker_GetMemberCases() \
   DGE_Game_Thinker_GetMemberCases(); \
   case ObjectMember::pitch: return pitch.raw(); \
   case ObjectMember::x:     return x.raw(); \
   case ObjectMember::y:     return y.raw(); \
   case ObjectMember::yaw:   return yaw.raw(); \
   case ObjectMember::z:     return z.raw()

//
// DGE_Game_PointThinker_SetMemberCases
//
#define DGE_Game_PointThinker_SetMemberCases() \
   DGE_Game_Thinker_SetMemberCases(); \
   case ObjectMember::pitch: pitch.raw(val); break; \
   case ObjectMember::x:     x.raw(val);     break; \
   case ObjectMember::y:     y.raw(val);     break; \
   case ObjectMember::yaw:   yaw.raw(val);   break; \
   case ObjectMember::z:     z.raw(val);     break


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
      PointThinker(Code::Word *emv_, std::size_t emc_) :
         Thinker{emv_, emc_}, pitch{0}, yaw{0}, x{0}, y{0}, z{0} {}

      Angle pitch, yaw;
      Fixed x, y, z;


      static This *Create(Code::Word ext) {return CreateT<This>(ext);}
   };
}

#endif//DGE__Game__PointThinker_H__

