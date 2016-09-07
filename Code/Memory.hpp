//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM memory.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Code__Memory_H__
#define Doom__Code__Memory_H__

#include "Code/Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // MemPage
      //
      class MemPage
      {
      public:
         virtual ~MemPage() {}

         virtual void free() {delete this;}

         virtual Byte  getB(Word idx);
         virtual HWord getH(Word idx);
         virtual Word  getW(Word idx) = 0;

         virtual void setB(Word idx, Byte  val);
         virtual void setH(Word idx, HWord val);
         virtual void setW(Word idx, Word  val) = 0;


         static MemPage *Create_Nul();
         static MemPage *Create_Sta();

         static constexpr Word SizeB = 4096;
         static constexpr Word SizeH = 2048;
         static constexpr Word SizeW = 1024;
      };

      //
      // MemBlock
      //
      class MemBlock
      {
      public:
         MemBlock() : pages{nullptr} {}
         virtual ~MemBlock() {}

         virtual MemPage *allocPage(Word idx);

         virtual void free() {delete this;}

         static constexpr Word PageC = 1024;
         MemPage *pages[PageC];


         static MemBlock *Create_Dyn();
         static MemBlock *Create_Nul();
         static MemBlock *Create_Sta(Word size);

         static constexpr Word SizeB = MemPage::SizeB * PageC;
         static constexpr Word SizeH = MemPage::SizeH * PageC;
         static constexpr Word SizeW = MemPage::SizeW * PageC;
      };

      //
      // Memory
      //
      class Memory
      {
      public:
         Memory();
         ~Memory();

         Byte getB(Word idx)
            {return getPage(idx / MemPage::SizeB)->getB(idx % MemPage::SizeB);}
         HWord getH(Word idx)
            {return getPage(idx / MemPage::SizeH)->getH(idx % MemPage::SizeH);}
         Word getW(Word idx)
            {return getPage(idx / MemPage::SizeW)->getW(idx % MemPage::SizeW);}

         void setB(Word idx, Byte val)
            {getPage(idx / MemPage::SizeB)->setB(idx % MemPage::SizeB, val);}
         void setH(Word idx, HWord val)
            {getPage(idx / MemPage::SizeH)->setH(idx % MemPage::SizeH, val);}
         void setW(Word idx, Word val)
            {getPage(idx / MemPage::SizeW)->setW(idx % MemPage::SizeW, val);}

         static constexpr Word BlockC = 1024;
         MemBlock *blocks[BlockC];

      private:
         MemPage *getPage(Word idx)
         {
            MemBlock *block = blocks[idx / MemBlock::PageC];
            MemPage *page = block->pages[idx % MemBlock::PageC];
            if(!page)
               page = block->allocPage(idx % MemBlock::PageC);

            return page;
         }
      };
   }
}

#endif//Doom__Code__Memory_H__
