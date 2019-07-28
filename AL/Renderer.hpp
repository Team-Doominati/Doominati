//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Audio renderer handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__AL__AudioRenderer_H__
#define DGE__AL__AudioRenderer_H__

#include "../AL/Sound.hpp"
#include "../AL/SoundSource.hpp"
#include "../AL/SoundSourceDynamic.hpp"

#include "../Core/ResourceManager.hpp"

#include <stdexcept>
#include <forward_list>

#define DGE_AL_GlobalSound 1


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::AL
{
   //
   // AudioInitError
   //
   class AudioInitError : public std::runtime_error
   {
   public:
      using std::runtime_error::runtime_error;
   };

   //
   // Renderer
   //
   class Renderer
   {
   public:
      Renderer();
      ~Renderer();

      void listenerPos(float x, float y, float z);
      void listenerVel(float x, float y, float z);
      void listenerAng(float ang);

      Sound *soundGet(GDCC::Core::String name);
      Sound *soundGet(std::size_t idx) {return sndMan.get(idx);}

      SoundSource *soundSrcCreate(float x, float y, float z);
      void soundSrcDestroy(unsigned id);
      SoundSource *soundSrcGet(unsigned id);

      void dopplerSpeed(float speed);


      static Renderer *GetCurrent();
      static void SetCurrent(Renderer *audio);

   private:
      using SoundSourceMap = Core::HashMapKeyMem<unsigned, SoundSource,
         &SoundSource::id, &SoundSource::link>;

      Sound *soundGet_File(GDCC::Core::String name);
      Sound *soundGet_None(GDCC::Core::String name);

      Core::ResourceManager<SoundData> sndMan;

      SoundSourceDynamic sndSrcGbl;
      SoundSourceMap     sndSrcMap;
      unsigned           sndSrcId;

      ALCdevice  *device;
      ALCcontext *context;
   };
}

#endif//DGE__AL__AudioRenderer_H__

