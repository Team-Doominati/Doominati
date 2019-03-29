//-----------------------------------------------------------------------------
//
// Copyright (C) 2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Filesystem commands.
//
//-----------------------------------------------------------------------------

#include "Sh/Command.hpp"

#include "Sh/Option.hpp"

#include "Code/Native.hpp"
#include "Code/Process.hpp"

#include "FS/Dir.hpp"

#include <fstream>
#include <iostream>


//----------------------------------------------------------------------------|
// Natives                                                                    |
//
namespace DGE::Sh
{
   DGE_Code_NativeLoaderDefn(Command_File);

   //
   // fs_export
   //
   DGE_Sh_CommandDefn(fs_export)
   {
      DGE_Sh_OptionsDecl("fs_export");
      DGE_Sh_OptionsProcess();

      if(opts.args.size() == 2)
      {
         char const *inName  = opts.args.data()[0];
         char const *outName = opts.args.data()[1];

         FS::File *in;

         if(*inName == '/')
            in = FS::Dir::Root->findFilePath(inName + 1);
         else
            in = Code::Process::GetMain()->getWorkDir()->findFilePath(inName);

         if(!in)
            return std::cerr << "unable to open '" << inName << "'\n", 1;

         std::ofstream out{outName, std::ios_base::out | std::ios_base::binary};

         if(!out)
            return std::cerr << "unable to open '" << outName << "'\n", 1;

         out.write(in->data, in->size);
      }
      else
         return std::cerr << "usage: fs_export <vfs-file> <host-file>\n", 1;

      return 0;
   }

   //
   // fs_flush
   //
   DGE_Sh_CommandDefn(fs_flush)
   {
      DGE_Sh_OptionsDecl("fs_flush");
      DGE_Sh_OptionsProcess();

      FS::Dir::Root->flush();

      return 0;
   }
}

// EOF

