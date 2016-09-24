//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Timing utilities.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Time_H__
#define DGE__Core__Time_H__

#include <chrono>
#include <cmath>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   template<typename T = std::size_t>
   using PlayTick = std::chrono::duration<T, std::ratio<1, 50>>;

   template<typename T>
   using Second = std::chrono::duration<T, std::ratio<1>>;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // GetTime
   //
   // Returns duration since epoch.
   //
   inline std::chrono::steady_clock::duration GetTime()
   {
      static std::chrono::steady_clock::time_point epoch;
      if(epoch == std::chrono::steady_clock::time_point())
         epoch = std::chrono::steady_clock::now();
      return std::chrono::steady_clock::now() - epoch;
   }

   //
   // GetTicks
   //
   template<typename T>
   typename T::rep GetTicks()
   {
      return std::chrono::duration_cast<T>(GetTime()).count();
   }

   //
   // GetTickFract
   //
   template<typename T>
   double GetTickFract()
   {
      return std::fmod(static_cast<double>(GetTicks<T>()), 1.0);
   }
}

#endif//DGE__Core__Time_H__

