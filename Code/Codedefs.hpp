//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// CODEDEFS loading.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Codedefs_H__
#define DGE__Code__Codedefs_H__

#include "Code/Types.hpp"

#include "Core/String.hpp"

#include <GDCC/Core/Array.hpp>

#include <unordered_map>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   class NTSStream;
}

namespace DGE::FS
{
   class File;
}

namespace DGE::Code
{
   //
   // Loader
   //
   class Loader
   {
   public:
      using RawExp  = GDCC::Core::Array<char const *>;
      using RawExpA = GDCC::Core::Array<RawExp>;
      using RawCode = std::pair<GDCC::Core::String, RawExpA>;
      using RawInit = std::pair<Word, RawExpA>;

      //
      // RawFunc
      //
      struct RawFunc
      {
         Core::HashedStr glyph;
         char const     *label;
         Word            param;
         Word            local;
         std::size_t     index;
      };


      Loader();

      Word evalExp(RawExp const &exp);

      void gen(Program *prog);

      void loadCodedefs(FS::File *file);

      std::size_t loadFAIL;
      std::size_t loadPASS;

   private:
      void addCode(char const *code, RawExpA &&argv);

      void addGlobal(Core::HashedStr glyph, Word words, RawExpA &&inits);

      Word addFunction(Core::HashedStr glyph, char const *label, Word param, Word local);

      bool addLabel(Core::HashedStr label);

      Word evalExp(RawExp::const_iterator &itr, RawExp::const_iterator last);

      Word evalVal(char const *val);

      RawExp  *findDefine(Core::HashedStr glyph);
      RawFunc *findFunction(Core::HashedStr glyph);
      Word    *findGlobal(Core::HashedStr glyph);
      Word    *findLabel(Core::HashedStr glyph);

      void genCodes(Program *prog);
      void genFuncs(Program *prog);
      void genGlobals(Program *prog);

      Word getLabel(Core::HashedStr glyph);

      RawExp loadExp(Core::NTSStream &in);

      void loadKeyword(Core::NTSStream &in);
      void loadKeywordBlock(Core::NTSStream &in);
      void loadKeywordDefine(Core::NTSStream &in);
      void loadKeywordFunction(Core::NTSStream &in);
      void loadKeywordGlobal(Core::NTSStream &in);

      char const *loadVal(Core::NTSStream &in);

      std::vector<RawCode>                        codes;
      std::unordered_map<Core::HashedStr, RawExp> defines;
      std::vector<RawFunc>                        funcs;
      std::unordered_map<Core::HashedStr, Word>   labels;
      std::unordered_map<Core::HashedStr, Word>   globals;
      std::vector<RawInit>                        inits;

      Word codeCount;
      Word globalCount;
   };
}

#endif//DGE__Code__Codedefs_H__

