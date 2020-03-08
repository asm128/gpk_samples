#include "Squad.h"
#include "Character.h"
#include "CharacterTile.h"
#include "PlayerControl.h"
#include "gpk_ptr.h"
#include "projects.h"


#ifndef __PLAYER_H__98213640926348273649827364987234698234__
#define __PLAYER_H__98213640926348273649827364987234698234__

namespace klib
{

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
		::klib::SPlayerControl									Control						= {};
		::klib::SPlayerSelection								Selection					= {0, 0, -1, -1, -1};
		::klib::SSquad											Squad						= {};
		::klib::SCharacterResearch								Research					= {};
		::klib::SCharacterScore									Score						= {};
		::gpk::array_pod<char_t>								Name						= ::gpk::view_const_string{"Kasparov"};
		::gpk::array_obj<::gpk::ptr_obj<::klib::CCharacter>>	Army						= {};
	};

	struct SPlayer { // can be AI or human.
		::klib::STacticalPlayer									Tactical					= {};
		::klib::SCharacterInventory								Inventory					= {};
		::klib::SPlayerProjects									Projects					= {};

		::gpk::array_obj<::klib::SEntityResearch>				ResearchablesValue			= {};
		::gpk::array_obj<::gpk::array_pod<char_t>>				ResearchablesText			= {};
		::gpk::array_obj<::klib::SEntityResearch>				ResearchedValue				= {};
		::gpk::array_obj<::gpk::array_pod<char_t>>				ResearchedText				= {};

		::gpk::array_obj<::klib::CDeadCharacter>				Memorial					= {};

		bool													IsAlive						()											const	{
			for(uint32_t iAgent = 0; iAgent < Tactical.Squad.Size; iAgent++)
				if(Tactical.Squad.Agents[iAgent] != -1 && Tactical.Army[Tactical.Squad.Agents[iAgent]]->IsAlive())
					return true;

			return false;
		}

		bool													CanMove						()											const	{
			for(uint32_t iAgent = 0; iAgent < Tactical.Squad.Size; iAgent++)
				if(Tactical.Squad.Agents[iAgent] != -1 && Tactical.Army[Tactical.Squad.Agents[iAgent]]->CanMove())
					return true;

			return false;
		}

		bool													SelectNextAgent				()													{
			uint32_t													count						= 0;
			const uint32_t												maxCount					= Tactical.Squad.Size;
			int32_t														agent						= -1;
			do {
				Tactical.Selection.PlayerUnit							= (Tactical.Selection.PlayerUnit + 1) % maxCount;
				agent													= Tactical.Squad.Agents[Tactical.Selection.PlayerUnit];
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

		bool													SelectPreviousAgent			()													{
			uint32_t													count						= 0;
			const uint32_t												maxCount					= Tactical.Squad.Size;
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

	void											playerUpdateResearchLists			(const ::klib::SEntityTables & entityTables, ::klib::SPlayer & player);

} // namespace

#endif // __PLAYER_H__98213640926348273649827364987234698234__
