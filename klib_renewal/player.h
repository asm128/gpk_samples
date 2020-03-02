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
		int64_t											Money;
	};

	struct SPlayerProjects {
		SProjectBudget									BudgetProduction			= {true, 10};
		SProjectBudget									BudgetResearch				= {true, 10};
		SProjectBudget									BudgetUpgrade				= {true, 10};

		::gpk::array_obj<SEntityResearch>				QueuedProduction			= {};
		::gpk::array_obj<SEntityResearch>				QueuedResearch				= {};
		::gpk::array_obj<SEntityResearch>				QueuedUpgrade				= {};

		int64_t											CostProduction				= 0;
		int64_t											CostResearch				= 0;
		int64_t											CostUpgrade					= 0;

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

	struct STacticalPlayer { // can be AI or human.
		int64_t													Money						= 2500000;
		SPlayerControl											Control						= {};
		SPlayerSelection										Selection					= {0, 0, -1, -1, -1};
		SSquad													Squad						= {};
		SCharacterResearch										Research					= {};
		SCharacterScore											Score						= {};
		::gpk::array_pod<char_t>								Name						= ::gpk::view_const_string{"Kasparov"};
		::gpk::array_obj<::gpk::ptr_obj<::klib::CCharacter>>	Army						= {};
	};

	struct SPlayer { // can be AI or human.
		STacticalPlayer											Tactical					= {};
		SCharacterInventory										Inventory					= {};
		SPlayerProjects											Projects					= {};
		::gpk::array_obj<CDeadCharacter>						Memorial					= {};

		bool													IsAlive						()											const	{
			for(uint32_t iAgent = 0; iAgent < Tactical.Squad.Size; iAgent++)
				if(Tactical.Squad.Agents[iAgent] != -1 && Tactical.Army[Tactical.Squad.Agents[iAgent]]->IsAlive())
					return true;

			return false;
		}

		bool											CanMove						()											const	{
			for(uint32_t iAgent = 0; iAgent < Tactical.Squad.Size; iAgent++)
				if(Tactical.Squad.Agents[iAgent] != -1 && Tactical.Army[Tactical.Squad.Agents[iAgent]]->CanMove())
					return true;

			return false;
		}

		bool											SelectNextAgent				()													{
			uint32_t											count						= 0;
			const uint32_t										maxCount					= Tactical.Squad.Size;
			int32_t												agent						= -1;
			do {
				Tactical.Selection.PlayerUnit					= (Tactical.Selection.PlayerUnit + 1) % maxCount;
				agent											= Tactical.Squad.Agents[Tactical.Selection.PlayerUnit];
			}
			while((count++) < maxCount
			  &&  (agent == -1 || 0 >= Tactical.Army[agent]->Points.LifeCurrent.Health || Tactical.Army[agent]->DidLoseTurn())
			);

			if(count >= maxCount
				&& (agent == -1 || 0 >= Tactical.Army[agent]->Points.LifeCurrent.Health || Tactical.Army[agent]->DidLoseTurn())
			)
				return false;

			Tactical.Squad.TargetPositions[Tactical.Selection.PlayerUnit] = Tactical.Army[agent]->Position;
			return true;
		}

		bool											SelectPreviousAgent			()													{
			uint32_t											count						= 0;
			const uint32_t										maxCount					= Tactical.Squad.Size;
			do {
				--Tactical.Selection.PlayerUnit;
				if(Tactical.Selection.PlayerUnit < 0)
					Tactical.Selection.PlayerUnit							= ((int16_t)maxCount-1);
			}
			while ((count++) < maxCount
				&& (Tactical.Squad.Agents[Tactical.Selection.PlayerUnit] == -1 || 0 >= Tactical.Army[Tactical.Squad.Agents[Tactical.Selection.PlayerUnit]]->Points.LifeCurrent.Health || Tactical.Army[Tactical.Squad.Agents[Tactical.Selection.PlayerUnit]]->DidLoseTurn())
			);

			if(count >= Tactical.Squad.Size
				&& (Tactical.Squad.Agents[Tactical.Selection.PlayerUnit] == -1 || 0 >= Tactical.Army[Tactical.Squad.Agents[Tactical.Selection.PlayerUnit]]->Points.LifeCurrent.Health || Tactical.Army[Tactical.Squad.Agents[Tactical.Selection.PlayerUnit]]->DidLoseTurn())
			)
				return false;

			Tactical.Squad.TargetPositions[Tactical.Selection.PlayerUnit]		= Tactical.Army[Tactical.Squad.Agents[Tactical.Selection.PlayerUnit]]->Position;
			return true;
		}
	};
} // namespace

#endif // __PLAYER_H__98213640926348273649827364987234698234__
