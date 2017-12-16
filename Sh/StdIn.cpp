//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Non-blocking stdin.
//
//-----------------------------------------------------------------------------

#include "Sh/StdIn.hpp"

#include <mutex>
#include <thread>
#include <vector>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Sh
{
   // BufI is for reading by main thread, BufO is for writing by worker thread.
   static std::vector<char> Buf[2], *BufI = &Buf[0], *BufO = &Buf[1];
   static auto BufIPtr = BufI->end();

   static std::mutex BufMut;
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Sh
{
   //
   // BufSwap
   //
   static bool BufSwap()
   {
      std::lock_guard<std::mutex> lock{BufMut};

      // Only swap buffers if BufI is empty and BufO has a full line.
      if(BufIPtr != BufI->end() || BufO->empty() || BufO->back() != '\n')
         return false;

      BufI->clear();

      std::swap(BufI, BufO);

      BufIPtr = BufI->begin();

      return true;
   }

   //
   // BufPut
   //
   static bool BufPut(char c)
   {
      std::lock_guard<std::mutex> lock{BufMut};

      // Do not read past end of line.
      if(!BufO->empty() && BufO->back() == '\n')
         return false;

      // Special handling for backspace.
      if(c == '\b')
      {
         if(!BufO->empty())
            BufO->pop_back();
      }
      else
         BufO->push_back(c);

      return true;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Sh
{
   //
   // StdIn_Init
   //
   void StdIn_Init()
   {
      std::thread{[]()
      {
         for(int c; (c = std::getchar()) != EOF;)
            while(!BufPut(c))
               std::this_thread::sleep_for(std::chrono::milliseconds{1});
      }}.detach();
   }

   //
   // StdIn_Read
   //
   std::size_t StdIn_Read(char *buf, std::size_t len)
   {
      if(BufIPtr == BufI->end() && !BufSwap())
         return 0;

      std::size_t avail = BufI->end() - BufIPtr;

      if(len > avail)
         len = avail;

      std::copy(BufIPtr, BufIPtr + len, buf);
      BufIPtr += len;

      return len;
   }

   //
   // StdIn_UnGet
   //
   int StdIn_UnGet(int c)
   {
      if(c == EOF) return c;

      if(BufIPtr == BufI->begin()) return EOF;

      return *--BufIPtr = c;
   }
}

// EOF

