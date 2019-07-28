//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Sound data management.
//
//-----------------------------------------------------------------------------

#include "AL/Renderer.hpp"

#include "AL/Sound.hpp"

#include "FS/Dir.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::AL
{
   //
   // Renderer::soundGet
   //
   Sound *Renderer::soundGet(GDCC::Core::String name)
   {
      if(auto snd = sndMan.resMap.find(name))
         return snd;

      if(name[0] == '@')
         return soundGet_File(name);

      std::cerr << "unknown sound: " << name << std::endl;
      return soundGet_None(name);
   }

   //
   // Renderer::soundGet_File
   //
   Sound *Renderer::soundGet_File(GDCC::Core::String name)
   {
      auto filename = name.data() + 1;
      auto file = FS::Dir::Root->findFilePath(filename);

      if(!file)
      {
         std::cerr << "sound file not found: " << filename << std::endl;
         return soundGet_None(name);
      }

      try
      {
         auto loader = CreateSoundLoader(file);
         GDCC::Core::Array<SoundSample> buf(loader->numSamples() * (loader->isMono() ? 1 : 2));
         loader->data(buf.data());

         return sndMan.add({loader->sampleRate(), loader->isMono(), buf}, name);
      }
      catch(SoundLoaderError const &err)
      {
         std::cerr << "SoundLoaderError: " << filename
            << ": " << err.what() << std::endl;
         return soundGet_None(name);
      }
   }

   //
   // Renderer::soundGet_None
   //
   Sound *Renderer::soundGet_None(GDCC::Core::String name)
   {
      return sndMan.add({}, name);
   }
}

// EOF

