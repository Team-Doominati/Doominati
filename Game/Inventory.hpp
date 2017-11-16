//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Inventory handling
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Inventory_H__
#define DGE__Game__Inventory_H__

#include "../Game/Object.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Inventory_GetMemberCases
//
#define DGE_Game_Inventory_GetMemberCases() \
   DGE_Game_Object_GetMemberCases(); \
   case ObjectMember::ic: return ic

//
// DGE_Game_Inventory_SetMemberCases
//
#define DGE_Game_Inventory_SetMemberCases() \
   DGE_Game_Object_SetMemberCases(); \
   case ObjectMember::ic: (void)val; break

//
// DGE_Game_ItemData_GetMemberCases
//
#define DGE_Game_ItemData_GetMemberCases() \
   DGE_Game_Object_GetMemberCases()

//
// DGE_Game_ItemData_SetMemberCases
//
#define DGE_Game_ItemData_SetMemberCases() \
   DGE_Game_Object_SetMemberCases()

//
// DGE_Game_ItemType_GetMemberCases
//
#define DGE_Game_ItemType_GetMemberCases() \
   DGE_Game_Object_GetMemberCases()

//
// DGE_Game_ItemType_SetMemberCases
//
#define DGE_Game_ItemType_SetMemberCases() \
   DGE_Game_Object_SetMemberCases()


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // ItemData
   //
   class ItemData : public Object
   {
      DGE_Game_ObjectPreamble(ItemData, Object);

   public:
      ItemData(Code::Word *emv_, std::size_t emc_) : Object{emv_, emc_} {}


      static This *Create(Code::Word ext) {return CreateT<This>(ext);}
   };

   //
   // ItemType
   //
   class ItemType : public Object
   {
      DGE_Game_ObjectPreamble(ItemType, Object);

   public:
      ItemType(Code::Word *emv_, std::size_t emc_) : Object{emv_, emc_} {}


      static This *Create(Code::Word ext) {return CreateT<This>(ext);}
   };

   //
   // Inventory
   //
   class Inventory : public Object
   {
      DGE_Game_ObjectPreamble(Inventory, Object);

   public:
      std::size_t    const ic;
      ItemData::Ptr *const idv;
      ItemType::Ptr *const itv;
      Code::Word    *const isv;


      static Inventory *Create(std::size_t ic, std::size_t ext);

   private:
      Inventory(Code::Word *emv, std::size_t emc, std::size_t ic,
         ItemData::Ptr *idv, ItemType::Ptr *itv, Code::Word *isv);
      virtual ~Inventory();
   };
}
#endif//DGE__Game__Inventory_H__

