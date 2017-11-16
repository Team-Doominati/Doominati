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

#include "Game/Inventory.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include "Core/Alloc.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Code_NativeLoaderDefn(Inventory);
   DGE_Game_ObjectImplement(Inventory);
   DGE_Game_ObjectImplement(ItemData);
   DGE_Game_ObjectImplement(ItemType);

   //
   // Inventory constructor
   //
   Inventory::Inventory(std::size_t ic_, ItemData::Ptr *idv_, ItemType::Ptr *itv_, Code::Word *isv_) :
      ic{ic_}, idv{idv_}, itv{itv_}, isv{isv_}
   {
      std::uninitialized_value_construct_n(idv, ic);
      std::uninitialized_value_construct_n(itv, ic);
      std::uninitialized_value_construct_n(isv, ic);
   }

   //
   // Inventory destructor
   //
   Inventory::~Inventory()
   {
      std::destroy_n(idv, ic);
      std::destroy_n(itv, ic);
      std::destroy_n(isv, ic);
   }

   //
   // Inventory::Create
   //
   Inventory *Inventory::Create(std::size_t ic, std::size_t ext)
   {
      std::size_t emc = ExtMem.max() + ext;

      // Calculate storage offsets.
      // Memory layout: [Inventory] [extension members] [size] [data] [type]
      std::size_t emo = sizeof(Inventory); // TODO: Alignment.
      std::size_t iso = Core::AlignOffset<Code::Word>(emo + emc * sizeof(Code::Word));
      std::size_t ido = Core::AlignOffset<ItemData>(iso + ic * sizeof(Code::Word));
      std::size_t ito = Core::AlignOffset<ItemType>(ido + ic * sizeof(ItemData::Ptr));

      // Allocate storage.
      auto buf = static_cast<char *>(::operator new(ito + ic * sizeof(ItemType::Ptr)));

      auto emv = reinterpret_cast<Code::Word *>(buf + emo);
      auto isv = reinterpret_cast<Code::Word *>(buf + iso);
      auto idv = reinterpret_cast<ItemData::Ptr *>(buf + ido);
      auto itv = reinterpret_cast<ItemType::Ptr *>(buf + ito);

      std::uninitialized_value_construct_n(emv, emc);

      return new(buf) Inventory{ic, idv, itv, isv};
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_Inventory_Create(unsigned ic, unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_Inventory_Create)
   {
      std::size_t ic  = argv[0];
      std::size_t ext = argv[1];

      task->dataStk.push(Inventory::Create(ic, ext)->id);
      return false;
   }

   //
   // DGE_Item DGE_Inventory_ItemGet(unsigned id, unsigned i)
   //
   DGE_Code_NativeDefn(DGE_Inventory_ItemGet)
   {
      auto inv = Inventory::Get(argv[0]);
      auto i   = argv[1];

      task->dataStk.push(inv->itv[i]->id);
      task->dataStk.push(inv->idv[i]->id);
      task->dataStk.push(inv->isv[i]);
      return false;
   }

   //
   // void DGE_Inventory_ItemSet(unsigned id, unsigned i, DGE_Item val)
   //
   DGE_Code_NativeDefn(DGE_Inventory_ItemSet)
   {
      auto inv = Inventory::Get(argv[0]);
      auto i   = argv[1];

      inv->itv[i] = ItemType::Get(argv[2]);
      inv->idv[i] = ItemData::Get(argv[3]);
      inv->isv[i] = argv[4];
      return false;
   }

   //
   // unsigned DGE_ItemData_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_ItemData_Create)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) ItemData)->id);
      return false;
   }

   //
   // unsigned DGE_ItemType_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_ItemType_Create)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) ItemType)->id);
      return false;
   }
}

// EOF

