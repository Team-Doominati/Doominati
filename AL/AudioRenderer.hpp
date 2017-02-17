//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
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

#include "AL/Sound.hpp"
#include "AL/SoundSource.hpp"
#include "AL/OpenAL.hpp"

#include "Core/ResourceManager.hpp"

#include <stdexcept>
#include <forward_list>


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
   // AudioRenderer
   //
   class AudioRenderer
   {
   public:
      AudioRenderer();
      ~AudioRenderer();

      void listenerPos(float x, float y, float z);
      void listenerVel(float x, float y, float z);
      void listenerAng(float ang);

      auto soundGet(GDCC::Core::String name) {return &soundGetRaw(name)->data;}
      auto soundGet(char const *name)        {return &soundGetRaw(name)->data;}
      auto soundGet(std::size_t idx)         {return &sndMan .get(idx) ->data;}

      auto soundGetIdx(GDCC::Core::String name) {return soundGetRaw(name)->idx;}

      SoundSource *soundSrcCreate(float x, float y, float z);
      void soundSrcDestroy(unsigned id);
      SoundSource *soundSrcGet(unsigned id) {return sndSrcMap.find(id);}


      static AudioRenderer *GetCurrent();
      static void SetCurrent(AudioRenderer *audio);

   private:
      using Sound = Core::Resource<SoundData>;

      using SoundSourceMap = Core::HashMapKeyMem<unsigned, SoundSource,
         &SoundSource::id, &SoundSource::link>;
      using SoundSourceLst = std::forward_list<SoundSource>;

      Sound *soundGetRaw(GDCC::Core::String name);

      Sound *soundGet_File(GDCC::Core::String name);
      Sound *soundGet_None(GDCC::Core::String name);

      // AUDIO_TODO: global SoundSource that has like 128 channels for
      //             nonlocal sound
      SoundSourceMap sndSrcMap;
      SoundSourceLst sndSrcLst;
      unsigned       sndSrcId;

      Core::ResourceManager<SoundData> sndMan;

      ALCdevice  *device;
      ALCcontext *context;
   };
}

#endif//DGE__AL__AudioRenderer_H__

