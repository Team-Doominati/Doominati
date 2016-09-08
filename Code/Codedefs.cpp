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

#include "Code/Codedefs.hpp"

#include "Code/Native.hpp"
#include "Code/Program.hpp"

#include "Core/NTS.hpp"

#include "FS/Dir.hpp"

#include <GDCC/Core/Exception.hpp>
#include <GDCC/Core/String.hpp>
#include <GDCC/Core/Token.hpp>

#include <iostream>
#include <cctype>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // CodeTrans
      //
      class CodeTrans
      {
      public:
         void      (*gen)(Loader *, OpCode *, Loader::RawExpA const &);
         std::size_t argc;
         std::size_t count;
         OpCode::Op  op;
      };
   }
}


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

namespace Doom
{
   namespace Code
   {
      static void GenCode(Loader *loader, OpCode *code, Loader::RawExpA const &args);
      static void GenCodeHW(Loader *loader, OpCode *code, Loader::RawExpA const &args);
      static void GenCodeW(Loader *loader, OpCode *code, Loader::RawExpA const &args);
      static void GenCodeWW(Loader *loader, OpCode *code, Loader::RawExpA const &args);
   }
}


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace Doom
{
   namespace Code
   {
      static std::unordered_map<GDCC::Core::String, CodeTrans> CodeTransTab
      {
         {"Nop",      {GenCode,   0, 1, OpCode::Nop}},
         {"Kill",     {GenCodeHW, 2, 1, OpCode::Kill}},

         {"AddU",     {GenCode,   0, 1, OpCode::AddU}},
         {"AndU",     {GenCode,   0, 1, OpCode::AndU}},
         {"CmpI_GE",  {GenCode,   0, 1, OpCode::CmpI_GE}},
         {"CmpI_GT",  {GenCode,   0, 1, OpCode::CmpI_GT}},
         {"CmpI_LE",  {GenCode,   0, 1, OpCode::CmpI_LE}},
         {"CmpI_LT",  {GenCode,   0, 1, OpCode::CmpI_LT}},
         {"CmpU_EQ",  {GenCode,   0, 1, OpCode::CmpU_EQ}},
         {"CmpU_GE",  {GenCode,   0, 1, OpCode::CmpU_GE}},
         {"CmpU_GT",  {GenCode,   0, 1, OpCode::CmpU_GT}},
         {"CmpU_LE",  {GenCode,   0, 1, OpCode::CmpU_LE}},
         {"CmpU_LT",  {GenCode,   0, 1, OpCode::CmpU_LT}},
         {"CmpU_NE",  {GenCode,   0, 1, OpCode::CmpU_NE}},
         {"Call",     {GenCodeW,  1, 1, OpCode::Call}},
         {"Cnat",     {GenCodeHW, 2, 1, OpCode::Cnat}},
         {"Copy",     {GenCode,   0, 1, OpCode::Copy}},
         {"DivI",     {GenCode,   0, 1, OpCode::DivI}},
         {"DivU",     {GenCode,   0, 1, OpCode::DivU}},
         {"Drop_Nul", {GenCode,   0, 1, OpCode::Drop_Nul}},
         {"Drop_Ptr", {GenCode,   0, 1, OpCode::Drop_Ptr}},
         {"Drop_Reg", {GenCodeW,  1, 1, OpCode::Drop_Reg}},
         {"InvU",     {GenCode,   0, 1, OpCode::InvU}},
         {"Jcnd_Lit", {GenCodeWW, 2, 2, OpCode::Jcnd_Lit}},
         {"Jcnd_Nil", {GenCodeW,  1, 1, OpCode::Jcnd_Nil}},
         {"Jcnd_Tru", {GenCodeW,  1, 1, OpCode::Jcnd_Tru}},
         {"Jump",     {GenCode,   0, 1, OpCode::Jump}},
         {"Jump_Lit", {GenCodeW,  1, 1, OpCode::Jump_Lit}},
         {"LNot",     {GenCode,   0, 1, OpCode::LNot}},
         {"ModI",     {GenCode,   0, 1, OpCode::ModI}},
         {"ModU",     {GenCode,   0, 1, OpCode::ModU}},
         {"MulU",     {GenCode,   0, 1, OpCode::MulU}},
         {"NegI",     {GenCode,   0, 1, OpCode::NegI}},
         {"OrIU",     {GenCode,   0, 1, OpCode::OrIU}},
         {"OrXU",     {GenCode,   0, 1, OpCode::OrXU}},
         {"Push_Lit", {GenCodeW,  1, 1, OpCode::Push_Lit}},
         {"Push_Ptr", {GenCode,   0, 1, OpCode::Push_Ptr}},
         {"Push_Reg", {GenCodeW,  1, 1, OpCode::Push_Reg}},
         {"Retn",     {GenCode,   0, 1, OpCode::Retn}},
         {"ShLU",     {GenCode,   0, 1, OpCode::ShLU}},
         {"ShRI",     {GenCode,   0, 1, OpCode::ShRI}},
         {"ShRU",     {GenCode,   0, 1, OpCode::ShRU}},
         {"SubU",     {GenCode,   0, 1, OpCode::SubU}},
         {"Swap",     {GenCode,   0, 1, OpCode::Swap}},
      };
   }
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
      //
      // GenCode_Kill
      //
      static void GenCode_Kill(OpCode *code, HWord type, Word data)
      {
         code->op  = OpCode::Kill;
         code->h.h = type;
         code->w.w = data;

         std::cerr << "OpCode: " << code->op << '(' << code->h.h << ',' << code->w.w << ")\n";
      }

      //
      // GenCode
      //
      static void GenCode(Loader *, OpCode *code, Loader::RawExpA const &)
      {
         std::cerr << "OpCode: " << code->op << "()\n";
      }

      //
      // GenCodeHW
      //
      static void GenCodeHW(Loader *loader, OpCode *code, Loader::RawExpA const &args)
      {
         code->h.h = loader->evalExp(args[0]);
         code->w.w = loader->evalExp(args[1]);

         std::cerr << "OpCode: " << code->op << '(' << code->h.h << ',' << code->w.w << ")\n";
      }

      //
      // GenCodeW
      //
      static void GenCodeW(Loader *loader, OpCode *code, Loader::RawExpA const &args)
      {
         code->w.w = loader->evalExp(args[0]);

         std::cerr << "OpCode: " << code->op << '(' << code->w.w << ")\n";
      }

      //
      // GenCodeWW
      //
      static void GenCodeWW(Loader *loader, OpCode *code, Loader::RawExpA const &args)
      {
         code[0].w.w = loader->evalExp(args[0]);
         code[1].w.w = loader->evalExp(args[1]);

         code[1].op = OpCode::Nop;

         std::cerr << "OpCode: " << code->op
            << '(' << code[0].w.w << ',' << code[1].w.w << ")\n";
      }
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
      //
      // Loader constructor
      //
      Loader::Loader() :
         loadFAIL{0},
         loadPASS{0},

         codeCount{1},
         globalCount{MemBlock::SizeW}
      {
      }

      //
      // Loader::addCode
      //
      void Loader::addCode(char const *codeStr, RawExpA &&argv)
      {
         auto code = CodeTransTab.find(GDCC::Core::String::Find(codeStr));
         if(code == CodeTransTab.end())
            throw GDCC::Core::ParseExceptExpect({}, "code", codeStr, false);

         // TODO: Better error message.
         if(argv.size() != code->second.argc)
            throw GDCC::Core::ParseExceptStr({}, "invalid argument count");

         codes.emplace_back(code->first, std::move(argv));
         codeCount += code->second.count;
      }

      //
      // Loader::addFunction
      //
      Word Loader::addFunction(Core::HashedStr glyph, char const *label,
         Word param, Word local)
      {
         if(auto func = findFunction(glyph))
            return func->index;

         funcs.push_back({glyph, label, param, local, funcs.size()});
         return funcs.back().index;
      }

      //
      // Loader::addLabel
      //
      bool Loader::addLabel(Core::HashedStr label)
      {
         if(labels.count(label)) return false;

         labels[label] = codeCount;
         return true;
      }

      //
      // Loader::addGlobal
      //
      void Loader::addGlobal(Core::HashedStr glyph, Word words, RawExpA &&initi)
      {
         if(globals.count(glyph))
            return;

         globals[glyph] = globalCount;
         inits.emplace_back(globalCount, std::move(initi));
         globalCount += words;
      }

      //
      // Loader::evalExp
      //
      Word Loader::evalExp(RawExp const &exp)
      {
         auto itr = exp.begin();
         return evalExp(itr, exp.end());
      }

      //
      // Loader::evalExp
      //
      Word Loader::evalExp(RawExp::const_iterator &itr, RawExp::const_iterator last)
      {
         // TODO: Throw parse exception.
         if(itr == last)
            return 0;

         #define EvalBin(op) \
            { \
               Word l = evalExp(++itr, last); \
               Word r = evalExp(  itr, last); \
               return l op r; \
            }

         switch(GDCC::Core::String::Find(*itr))
         {
         case GDCC::Core::STR_Add: EvalBin(+);
         case GDCC::Core::STR_Div: EvalBin(/);
         case GDCC::Core::STR_Mul: EvalBin(*);
         case GDCC::Core::STR_Sub: EvalBin(-);
         default:
            return evalVal(*itr++);
         }

         #undef EvalBin
      }

      //
      // Loader::evalVal
      //
      Word Loader::evalVal(char const *val)
      {
         // Number.
         if(std::isdigit(val[0]))
         {
            char *valEnd;
            Word res = std::strtoul(val, &valEnd, 16);

            // TODO: Throw parse exception.
            if(!valEnd || *valEnd)
               return 0;

            return res;
         }

         // Glyph.
         if(std::isalpha(val[0]) || val[0] == '_')
         {
            Core::HashedStr glyph = val;

            if(auto global = findGlobal(glyph))
               return *global;

            if(auto label = findLabel(glyph))
               return *label;

            if(auto define = findDefine(glyph))
               return evalExp(*define);

            if(auto native = Natives.findItr(glyph))
               return static_cast<Word>(native - Natives.begin());

            if(auto fn = findFunction(glyph))
               return fn->index;

            // TODO: Throw parse exception.
            std::cerr << "unrecognized glyph: '" << val << "'\n";
            return 0;
         }

         // TODO: Throw parse exception.
         std::cerr << "unrecognized val: '" << val << "'\n";
         return 0;
      }

      //
      // Loader::findDefine
      //
      Loader::RawExp *Loader::findDefine(Core::HashedStr glyph)
      {
         auto itr = defines.find(glyph);
         return itr != defines.end() ? &itr->second : nullptr;
      }

      //
      // Loader::findFunction
      //
      Loader::RawFunc *Loader::findFunction(Core::HashedStr glyph)
      {
         for(auto &func : funcs)
            if(func.glyph == glyph) return &func;

         return nullptr;
      }

      //
      // Loader::findGlobal
      //
      Word *Loader::findGlobal(Core::HashedStr glyph)
      {
         auto itr = globals.find(glyph);
         return itr != globals.end() ? &itr->second : nullptr;
      }

      //
      // Loader::findLabel
      //
      Word *Loader::findLabel(Core::HashedStr glyph)
      {
         auto itr = labels.find(glyph);
         return itr != labels.end() ? &itr->second : nullptr;
      }

      //
      // Loader::gen
      //
      void Loader::gen(Program *prog)
      {
         prog->codes = GDCC::Core::Array<OpCode>{codeCount + 1};
         genCodes(prog);

         prog->funcs.alloc(funcs.size());
         genFuncs(prog);
         prog->funcs.build();

         genGlobals(prog);
      }

      //
      // Loader::genCodes
      //
      void Loader::genCodes(Program *prog)
      {
         auto codeItr = prog->codes.begin();

         GenCode_Kill(codeItr++, 0, 0);

         for(auto &code : codes)
         {
            auto const &trans = CodeTransTab[code.first];
            codeItr->op = trans.op;
            trans.gen(this, codeItr, code.second);
            codeItr += trans.count;
         }

         GenCode_Kill(codeItr++, 0, 1);
      }

      //
      // Loader::genFuncs
      //
      void Loader::genFuncs(Program *prog)
      {
         auto funcItr = prog->funcs.begin();

         for(auto &func : funcs)
         {
            OpCode *entry = &prog->codes[getLabel(func.label)];

            funcItr->key = {func.glyph.str, func.glyph.len, func.glyph.hash};
            funcItr->val = {entry, func.local, func.param};

            std::cerr << "Function: " << funcItr->key << '('
               << funcItr->val.param << ") @"
               << funcItr->val.entry - prog->codes.data() << " {"
               << funcItr->val.local << "}\n";

            ++funcItr;
         }
      }

      //
      // Loader::genGlobals
      //
      void Loader::genGlobals(Program *prog)
      {
         MemBlock **block    = prog->memory.blocks;
         MemBlock **blockMid = block + Memory::BlockC / 2;
         MemBlock **blockEnd = block + Memory::BlockC;

         // Block 0 is special.
         *block++ = MemBlock::Create_Nul();

         // Allocate memory for statics.
         Word globalWords = globalCount - MemBlock::SizeW;
         for(; globalWords >= MemBlock::SizeW; globalWords -= MemBlock::SizeW)
            *block++ = MemBlock::Create_Sta(MemBlock::SizeW);

         if(globalWords)
            *block++ = MemBlock::Create_Sta(globalWords);

         // Fill remaining first half of memory with dynamic blocks.
         prog->freestoreBegin = (block - prog->memory.blocks) * MemBlock::SizeW;
         while(block != blockMid)
            *block++ = MemBlock::Create_Dyn();

         // Upper half is reserved for future use.
         prog->freestoreEnd = (block - prog->memory.blocks) * MemBlock::SizeW;
         while(block != blockEnd)
            *block++ = MemBlock::Create_Nul();

         // Apply initializers.
         for(auto const &init : inits)
         {
            Word idx = init.first;
            for(auto const &exp : init.second)
               prog->memory.setW(idx++, evalExp(exp));
         }
      }

      //
      // Loader::getLabel
      //
      Word Loader::getLabel(Core::HashedStr glyph)
      {
         if(auto label = findLabel(glyph))
            return *label;

         // TODO: Throw parse exception.
         return 0;
      }

      //
      // Loader::loadCodedefs
      //
      void Loader::loadCodedefs(FS::File *file)
      {
         if(file->format != FS::Format::DGE_NTS)
            return;

         Core::NTSArray  arr{file->data, file->size};
         Core::NTSStream in {arr};

         try
         {
            while(in) loadKeyword(in);
            ++loadPASS;
         }
         catch(GDCC::Core::ParseException const &e)
         {
            std::cerr << "parse error in '" << file->name << "': " << e.what() << '\n';
            ++loadFAIL;
         }
      }

      //
      // Loader::loadExp
      //
      Loader::RawExp Loader::loadExp(Core::NTSStream &in)
      {
         std::vector<char const *> argv;

         while(in)
         {
            auto val = in.peek();

            if(val[0] && !val[1] && (*val == ',' || *val == ')' || *val == ';')) break;

            argv.push_back(val); in.drop();
         }

         return {argv.begin(), argv.end()};
      }

      //
      // Loader::loadKeyword
      //
      void Loader::loadKeyword(Core::NTSStream &in)
      {
         GDCC::Core::String tlk = in.get();

         switch(tlk)
         {
         case GDCC::Core::STR_:                                  break;
         case GDCC::Core::STR_block:    loadKeywordBlock(in);    break;
         case GDCC::Core::STR_define:   loadKeywordDefine(in);   break;
         case GDCC::Core::STR_function: loadKeywordFunction(in); break;
         case GDCC::Core::STR_global:   loadKeywordGlobal(in);   break;

         default:
            throw GDCC::Core::ParseExceptExpect({}, "top-level keyword", tlk, false);
         }
      }

      //
      // Loader::loadKeywordBlock
      //
      void Loader::loadKeywordBlock(Core::NTSStream &in)
      {
         char const *code = in.get();

         in.expect("{");

         if(!*code || addLabel(code)) while(!in.drop("}"))
         {
            code = in.get();

            in.expect("(");

            if(std::strcmp(code, "label") == 0)
               addLabel(in.get());

            else if(in.peek(")"))
               addCode(code, {});

            else
            {
               std::vector<RawExp> argv;

               do {argv.emplace_back(loadExp(in));} while(in.drop(","));

               addCode(code, {GDCC::Core::Move, argv.begin(), argv.end()});
            }

            in.expect(")");
         }
         else
            while(in && !in.peek("}")) in.drop();
      }

      //
      // Loader::loadKeywordDefine
      //
      void Loader::loadKeywordDefine(Core::NTSStream &in)
      {
         char const *key = in.get();    in.expect("=");
         RawExp      val = loadExp(in); in.expect(";");

         defines[key] = std::move(val);
      }

      //
      // Loader::loadKeywordFunction
      //
      void Loader::loadKeywordFunction(Core::NTSStream &in)
      {
         char const *glyph,
            *valLabel = nullptr,
            *valLocal = nullptr,
            *valParam = nullptr;

         glyph = in.get();

         in.expect("{");

         while(!in.drop("}"))
         {
            char const *key = in.get();

            switch(GDCC::Core::String::Find(key))
            {
            case GDCC::Core::STR_label: valLabel = loadVal(in); break;
            case GDCC::Core::STR_local: valLocal = loadVal(in); break;
            case GDCC::Core::STR_param: valParam = loadVal(in); break;
            case GDCC::Core::STR_retrn:            loadVal(in); break;

            default:
               throw GDCC::Core::ParseExceptExpect({}, "function keyword", key, false);
            }
         }

         if(!valLabel) return;

         Word param = valParam ? evalVal(valParam) : 0;
         Word local = valLocal ? evalVal(valLocal) : 0;

         addFunction(glyph, valLabel, param, local);
      }

      //
      // Loader::loadKeywordGlobal
      //
      void Loader::loadKeywordGlobal(Core::NTSStream &in)
      {
         char const *glyph = in.get();
         std::size_t words = evalVal(in.get());

         if(in.drop(";"))
            addGlobal(glyph, words, {});
         else
         {
            in.expect("(");

            if(in.peek(")"))
               addGlobal(glyph, words, {});
            else
            {
               std::vector<RawExp> initi; initi.reserve(words);

               do initi.emplace_back(loadExp(in)); while(in.drop(","));

               addGlobal(glyph, words, {GDCC::Core::Move, initi.begin(), initi.end()});
            }

            in.expect(")");
         }
      }

      //
      // Loader::loadVal
      //
      char const *Loader::loadVal(Core::NTSStream &in)
      {
         in.expect("=");
         auto val = in.get();
         in.expect(";");
         return val;
      }
   }
}

// EOF

