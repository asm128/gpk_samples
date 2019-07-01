#include "Squad.h"
#include "Character.h"
#include "CharacterTile.h"
#include "PlayerControl.h"
#include "gpk_ptr.h"


#ifndef __PLAYER_H__98213640926348273649827364987234698234__
#define __PLAYER_H__98213640926348273649827364987234698234__

namespace klib
{
	struct SProjectBudget {
		bool											bIsRatio;
		int32_t											Money;
	};

	struct SPlayerProjects {
		SProjectBudget									BudgetProduction			= {true, 10};
		SProjectBudget									BudgetResearch				= {true, 10};
		SProjectBudget									BudgetUpgrade				= {true, 10};

		::gpk::array_obj<SEntityResearch>				QueuedProduction			= {};
		::gpk::array_obj<SEntityResearch>				QueuedResearch				= {};
		::gpk::array_obj<SEntityResearch>				QueuedUpgrade				= {};

		int32_t											CostProduction				= 0;
		int32_t											CostResearch				= 0;
		int32_t											CostUpgrade					= 0;

		void											EnqueueProduction			( const SEntityResearch&	production	)			{ QueuedProduction	.push_back(	production	); CostProduction	 +=	production	.PriceUnit -	production	.PricePaid; };
		void											EnqueueResearch				( const SEntityResearch&	research	)			{ QueuedResearch	.push_back(	research	); CostResearch		 +=	research	.PriceUnit -	research	.PricePaid; };
		void											EnqueueUpgrade				( const SEntityResearch&	upgrade		)			{ QueuedUpgrade		.push_back(	upgrade		); CostUpgrade		 +=	upgrade		.PriceUnit -	upgrade		.PricePaid; };

		void											DequeueProduction			( int32_t index	)									{ const SEntityResearch & production	= QueuedProduction	[index]; CostProduction	 -=	production	.PriceUnit -	production	.PricePaid; QueuedProduction	.remove(index); }
		void											DequeueResearch				( int32_t index	)									{ const SEntityResearch & research		= QueuedResearch	[index]; CostResearch	 -=	research	.PriceUnit -	research	.PricePaid; QueuedResearch		.remove(index); }
		void											DequeueUpgrade				( int32_t index	)									{ const SEntityResearch & upgrade		= QueuedUpgrade		[index]; CostUpgrade	 -=	upgrade		.PriceUnit -	upgrade		.PricePaid; QueuedUpgrade		.remove(index); }
	};

#define DECLARE_EQUIP_TYPE(name)			\
	typedef SEquip<S##name> SEquip##name;	\
//	GDECLARE_POD(SEquip##name)

	DECLARE_EQUIP_TYPE(Profession	);
	DECLARE_EQUIP_TYPE(Weapon		);
	DECLARE_EQUIP_TYPE(Armor		);
	DECLARE_EQUIP_TYPE(Accessory	);
	DECLARE_EQUIP_TYPE(Vehicle		);
	DECLARE_EQUIP_TYPE(Facility		);
	DECLARE_EQUIP_TYPE(StageProp	);
	DECLARE_EQUIP_TYPE(Item			);
	DECLARE_EQUIP_TYPE(Tile			);

	struct SPlayerInventory {
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipProfession>>		EquipProfession				= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipWeapon	>>		EquipWeapon					= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipArmor		>>		EquipArmor					= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipAccessory	>>		EquipAccessory				= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipVehicle	>>		EquipVehicle				= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipFacility	>>		EquipFacility				= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipStageProp	>>		EquipStageProp				= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipItem		>>		EquipItem					= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::SEquipTile		>>		EquipTile					= {};
	};

	//
	//GDECLARE_OBJ(CCharacter);

	struct SPlayer { // can be AI or human.
		int32_t													Money						= 1000000;
		SPlayerControl											Control						= SPlayerControl();
		SPlayerSelection										Selection					= {0, 0, -1, -1, -1};
		SSquad													Squad						= SSquad();
		SCharacterScore											Score						= {};
		SCharacterGoods											Goods						= {};
		SPlayerProjects											Projects					= {};
		SPlayerInventory										Inventory					= {};
		::gpk::array_obj<::gpk::ptr_obj<::klib::CCharacter>>	Army						= {};
		::gpk::array_obj<CDeadCharacter>						Memorial					= {};
		::gpk::label											Name						= "Kasparov";

		bool													IsAlive						()											const	{
			for(int32_t iAgent = 0; iAgent < Squad.Size; iAgent++)
				if(Squad.Agents[iAgent] != -1 && Army[Squad.Agents[iAgent]]->IsAlive())
					return true;

			return false;
		}

		bool											CanMove						()											const	{
			for(int32_t iAgent = 0; iAgent < Squad.Size; iAgent++)
				if(Squad.Agents[iAgent] != -1 && Army[Squad.Agents[iAgent]]->CanMove())
					return true;

			return false;
		}

		bool											SelectNextAgent				()													{
			int32_t												count						= 0;
			const int32_t										maxCount					= Squad.Size;
			do
				Selection.PlayerUnit							= (Selection.PlayerUnit + 1) % maxCount;
			while((count++) < maxCount
			  &&  (Squad.Agents[Selection.PlayerUnit] == -1 || 0 >= Army[Squad.Agents[Selection.PlayerUnit]]->Points.LifeCurrent.Health || Army[Squad.Agents[Selection.PlayerUnit]]->DidLoseTurn()) 
			);

			if(count >= maxCount
				&& (Squad.Agents[Selection.PlayerUnit] == -1 || 0 >= Army[Squad.Agents[Selection.PlayerUnit]]->Points.LifeCurrent.Health || Army[Squad.Agents[Selection.PlayerUnit]]->DidLoseTurn()) 
			)
				return false;

			Squad.TargetPositions[Selection.PlayerUnit] = Army[Squad.Agents[Selection.PlayerUnit]]->Position;
			return true;
		}

		bool											SelectPreviousAgent			()													{
			int32_t												count						= 0;
			const int32_t										maxCount					= Squad.Size;
			do {
				--Selection.PlayerUnit;
				if(Selection.PlayerUnit < 0) 
					Selection.PlayerUnit							= ((int16_t)maxCount-1);
			}
			while ((count++) < maxCount
				&& (Squad.Agents[Selection.PlayerUnit] == -1 || 0 >= Army[Squad.Agents[Selection.PlayerUnit]]->Points.LifeCurrent.Health || Army[Squad.Agents[Selection.PlayerUnit]]->DidLoseTurn()) 
			);

			if(count >= Squad.Size
				&& (Squad.Agents[Selection.PlayerUnit] == -1 || 0 >= Army[Squad.Agents[Selection.PlayerUnit]]->Points.LifeCurrent.Health || Army[Squad.Agents[Selection.PlayerUnit]]->DidLoseTurn()) 
			)
				return false;

			Squad.TargetPositions[Selection.PlayerUnit]		= Army[Squad.Agents[Selection.PlayerUnit]]->Position;

			return true;
		}
	};
} // namespace

#endif // __PLAYER_H__98213640926348273649827364987234698234__
