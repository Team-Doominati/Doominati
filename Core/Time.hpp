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

#ifndef Doom__Core__Time_H__
#define Doom__Core__Time_H__

#include <chrono>
#include <cmath>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Core
   {
      using PlayTick = std::chrono::duration<std::size_t, std::ratio<1, 50>>;
      using PlayTickFloat = std::chrono::duration<double, std::ratio<1, 50>>;
      using Millisecond = std::chrono::duration<double, std::ratio<1, 1000>>;
      using Second = std::chrono::duration<double, std::ratio<1>>;
   }
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace Doom
{
   namespace Core
   {
      //
      // GetTicks
      //

      template<typename T>
      static decltype(T().count()) GetTicks()
      {
         static std::chrono::time_point<std::chrono::steady_clock> base;

         if(base == std::chrono::time_point<std::chrono::steady_clock>())
            base = std::chrono::steady_clock::now();

         return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - base).count();
      }

      //
      // GetTickFract
      //

      template<typename T>
      static double GetTickFract()
      {
         return std::fmod(GetTicks<T>(), 1.0);
      }
   }
}

#endif//Doom__Core__Time_H__
