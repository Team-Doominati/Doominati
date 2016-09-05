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

#include "Code/Memory.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // MemPage_Nul
      //
      class MemPage_Nul : public MemPage
      {
      public:
         virtual void free() {}

         virtual Byte  getB(Word) {return 0;}
         virtual HWord getH(Word) {return 0;}
         virtual Word  getW(Word) {return 0;}

         virtual void setB(Word, Byte)  {}
         virtual void setH(Word, HWord) {}
         virtual void setW(Word, Word)  {}
      };

      //
      // MemPage_Sta
      //
      class MemPage_Sta : public MemPage
      {
      public:
         MemPage_Sta() : data{0} {}

         virtual Word getW(Word idx) {return data[idx];}

         virtual void setW(Word idx, Word val) {data[idx] = val;}

         Word data[SizeW];
      };

      //
      // MemPage_Sub
      //
      class MemPage_Sub : public MemPage
      {
      public:
         virtual Word getW(Word idx) {return data[idx];}

         virtual void setW(Word idx, Word val) {data[idx] = val;}

         Word *data;
      };

      //
      // MemBlock_Dyn
      //
      class MemBlock_Dyn : public MemBlock
      {
      public:
         //
         // destructor
         //
         ~MemBlock_Dyn()
         {
            for(MemPage *page : pages)
               if(page) page->free();
         }

         //
         // allocPage
         //
         virtual MemPage *allocPage(Word idx)
         {
            return pages[idx] = MemPage::Create_Sta();
         }
      };

      //
      // MemBlock_Nul
      //
      class MemBlock_Nul : public MemBlock
      {
      public:
         //
         // constructor
         //
         MemBlock_Nul()
         {
            for(MemPage *&page : pages)
               page = MemPage::Create_Nul();
         }

         virtual void free() {}
      };

      //
      // MemBlock_Sta
      //
      class MemBlock_Sta : public MemBlock
      {
      public:
         //
         // constructor
         //
         MemBlock_Sta() : data{0}
         {
            Word        *dataItr = data;
            MemPage_Sub *pageItr = pageSub;

            for(auto &page : pages)
            {
               page = pageItr;
               pageItr++->data = dataItr;
               dataItr += MemPage::SizeW;
            }
         }

         MemPage_Sub pageSub[PageC];
         Word        data[SizeW];
      };

      //
      // MemBlock_StaPart
      //
      class MemBlock_StaPart : public MemBlock
      {
      public:
         //
         // constructor
         //
         MemBlock_StaPart(Word pageC)
         {
            MemPage **page    = pages;
            MemPage **pageMid = pages + pageC;
            MemPage **pageEnd = pages + PageC;

            while(page != pageMid)
               *page++ = MemPage::Create_Sta();

            while(page != pageEnd)
               *page++ = MemPage::Create_Nul();
         }

         //
         // destructor
         //
         ~MemBlock_StaPart()
         {
            for(MemPage *page : pages)
               page->free();
         }
      };
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
      // MemPage::getB
      //
      Byte MemPage::getB(Word idx)
      {
         return (getW(idx / 4) >> (idx % 4 * 8)) & 0xFF;
      }

      //
      // MemPage::getH
      //
      HWord MemPage::getH(Word idx)
      {
         return (getW(idx / 2) >> (idx % 2 * 16)) & 0xFFFF;
      }

      //
      // MemPage::setB
      //
      void MemPage::setB(Word idx, Byte val)
      {
         Word idxW = idx / 4;
         Word idxB = idx % 4 * 8;

         setW(idxW, (getW(idxW) & ~(Word(0xFF) << idxB)) | (Word(val) << idxB));
      }

      //
      // MemPage::setH
      //
      void MemPage::setH(Word idx, HWord val)
      {
         Word idxW = idx / 2;
         Word idxH = idx % 2 * 16;

         setW(idxW, (getW(idxW) & ~(Word(0xFFFF) << idxH)) | (Word(val) << idxH));
      }

      //
      // MemPage::Create_Nul
      //
      MemPage *MemPage::Create_Nul()
      {
         static MemPage_Nul page;
         return &page;
      }

      //
      // MemPage::Create_Sta
      //
      MemPage *MemPage::Create_Sta()
      {
         return new MemPage_Sta;
      }

      //
      // MemBlock::allocPage
      //
      MemPage *MemBlock::allocPage(Word)
      {
         return MemPage::Create_Nul();
      }

      //
      // MemBlock::Create_Dyn
      //
      MemBlock *MemBlock::Create_Dyn()
      {
         return new MemBlock_Dyn;
      }

      //
      // MemBlock::Create_Nul
      //
      MemBlock *MemBlock::Create_Nul()
      {
         static MemBlock_Nul block;
         return &block;
      }

      //
      // MemBlock::Create_Sta
      //
      MemBlock *MemBlock::Create_Sta(Word size)
      {
         Word pageC = (size + (MemPage::SizeW - 1)) / MemPage::SizeW;

         if(pageC < PageC)
            return new MemBlock_StaPart(pageC);
         else
            return new MemBlock_Sta;
      }

      //
      // Memory constructor
      //
      Memory::Memory() :
         blocks{nullptr}
      {
      }

      //
      // Memory destructor
      //
      Memory::~Memory()
      {
         for(MemBlock *block : blocks)
            if(block) block->free();
      }
   }
}

// EOF

