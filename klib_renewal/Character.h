#include "Entity.h"
#include "EntityPoints.h"

#include "CharacterPoints.h"

#include "gpk_coord.h"

#ifndef __GAMEBASE_H__98236498027346892734689273__
#define __GAMEBASE_H__98236498027346892734689273__

namespace klib
{
#pragma pack(push, 1)
	struct SCharacterEquip {
		SWeapon								Weapon					= {0,0,1,-1};	// Index, ModifierIndex, Level
		SAccessory							Accessory				= {0,0,1,-1};	// Index, ModifierIndex, Level
		SProfession							Profession				= {0,0,1,-1};	// Index, ModifierIndex, Level
		SArmor								Armor					= {0,0,1,-1};	// Index, ModifierIndex, Level
		SVehicle							Vehicle					= {0,0,1,-1};	// Index, ModifierIndex, Level
		SFacility							Facility				= {0,0,1,-1};	// Index, ModifierIndex, Level
		SStageProp							StageProp				= {0,0,1,-1};	// Index, ModifierIndex, Level
	};

	struct SCharacterResearch {
		SResearchGroup<SWeapon		>		Weapon					= {};
		SResearchGroup<SAccessory	>		Accessory				= {};
		SResearchGroup<SProfession	>		Profession				= {};
		SResearchGroup<SArmor		>		Armor					= {};
		SResearchGroup<SVehicle		>		Vehicle					= {};
		SResearchGroup<SFacility	>		Facility				= {};
		SResearchGroup<SStageProp	>		StageProp				= {};
	};

	struct SCharacterInventory {
		SEntityContainer<SItem		>		Items;
		SEntityContainer<SProfession>		Profession;
		SEntityContainer<SWeapon	>		Weapon;
		SEntityContainer<SArmor		>		Armor;
		SEntityContainer<SAccessory	>		Accessory;
		SEntityContainer<SVehicle	>		Vehicle;
		SEntityContainer<SStageProp	>		StageProp;
		SEntityContainer<SFacility	>		Facility;

		int32_t								GetCount				(ENTITY_TYPE entityType) const	{
			int32_t result = 0;
			switch(entityType) {
			//case ENTITY_TYPE_CHARACTER	:	return
			case ENTITY_TYPE_PROFESSION	: result = Profession	.Slots.size(); break;
			case ENTITY_TYPE_WEAPON		: result = Weapon		.Slots.size(); break;
			case ENTITY_TYPE_ARMOR		: result = Armor		.Slots.size(); break;
			case ENTITY_TYPE_ACCESSORY	: result = Accessory	.Slots.size(); break;
			case ENTITY_TYPE_VEHICLE	: result = Vehicle		.Slots.size(); break;
			case ENTITY_TYPE_FACILITY	: result = Facility		.Slots.size(); break;
			case ENTITY_TYPE_STAGE_PROP	: result = StageProp	.Slots.size(); break;
			case ENTITY_TYPE_ITEM		: result = Items		.Slots.size(); break;
			default:
				break;
			}

			return result;
		}
	};

	struct SCharacterGoods {
		SCharacterResearch										CompletedResearch		= {};
		SCharacterInventory										Inventory				= {};
	};


	struct SCharacter {
							SEntityPoints						Points					= {};
							SEntityFlags						Flags					= {};

							SEntityPoints						FinalPoints				= {};
							SEntityFlags						FinalFlags				= {};
							SEntityGauges						Gauges					= {};

							SCharacterTurnBonus					ActiveBonus				= {};
							SCharacterScore						Score					= {};
							SCharacterEquip						CurrentEquip			= {};
							SCharacterGoods						Goods					= {};

																SCharacter				()													= default;
																SCharacter				(int maxHP, int hitChance, int attack, int coins, SFitnessPoints speed, SEntityEffect characterEffect, SEntityStatus characterStatus )
			:Points				({{maxHP}, {maxHP}, {hitChance, attack, {}, 0, 0}, speed, coins, coins, coins/10})
			,Flags				({characterEffect, characterStatus})
			,Gauges				({{maxHP, maxHP}, {0, 0}, {0, 0}})
		{}

							void								RecalculateFinalPoints	(const ::klib::SEntityTables & tables);
							void								RecalculateFinalFlags	(const ::klib::SEntityTables & tables);
							void								Recalculate				(const ::klib::SEntityTables & tables)				{ RecalculateFinalPoints(tables); RecalculateFinalFlags(tables);	}

		inline				bool								CanMove					()								const	noexcept	{ return IsAlive() && !DidLoseTurn();					}
		inline				bool								IsAlive					()								const	noexcept	{ return Points.LifeCurrent.Health > 0;					}
							bool								DidLoseTurn				()								const	noexcept	{
			return ::gpk::bit_true(ActiveBonus.Status.Status, COMBAT_STATUS_SLEEP		)
				|| ::gpk::bit_true(ActiveBonus.Status.Status, COMBAT_STATUS_STUN		)
				|| ::gpk::bit_true(ActiveBonus.Status.Status, COMBAT_STATUS_SHOCK		)
				|| ::gpk::bit_true(ActiveBonus.Status.Status, COMBAT_STATUS_PANIC		)
				|| ::gpk::bit_true(ActiveBonus.Status.Status, COMBAT_STATUS_PETRIFY	)
				|| ::gpk::bit_true(ActiveBonus.Status.Status, COMBAT_STATUS_FROZEN		)
				;
		}
	};

	//SEntityPoints				calculateFinalPoints			(const SCharacter& character);
	//SEntityFlags				calculateFinalFlags				(const SCharacter& character);

	static inline		void								rest					(const ::klib::SEntityTables & tables, SCharacter& character)		{
		character.Recalculate(tables);
		character.Points.LifeCurrent							= character.FinalPoints.LifeMax;
	}
#pragma pack(pop)



	class CCharacter : public SCharacter {
	public:
						::gpk::SCoord3<int32_t>					Position				= {};
						::gpk::array_pod<char_t>				Name					= ::gpk::view_const_string{"Unnamed"};

																CCharacter				()						= default;
																CCharacter				(int maxHP, int hitChance, int attack, int coins, SFitnessPoints speed, SEntityEffect characterEffect, SEntityStatus characterStatus, const ::gpk::view_const_char& name)
		: SCharacter	(maxHP, hitChance, attack, coins, speed, characterEffect, characterStatus)
		, Position		({0,0})
		, Name			(name)
		{}
	};

	class CDeadCharacter {
	public:
						SCharacterScore							Score					= {};
						::gpk::array_pod<char_t>				Name					= "Unnamed";

		inline													CDeadCharacter			()									= default;
		inline													CDeadCharacter			(const CCharacter& deadCharacter)
			:	Score	(deadCharacter.Score)
			,	Name	(deadCharacter.Name)
		{}
	};

	template<typename _TEntity>
	void														completeAgentResearch	(klib::SResearchGroup<_TEntity>& agentCompletedResearch, const _TEntity& itemToResearch)	{
		agentCompletedResearch.Definitions	.AddElement(itemToResearch.Definition	);
		agentCompletedResearch.Modifiers	.AddElement(itemToResearch.Modifier		);
		agentCompletedResearch.MaxResearch.Definition	= (agentCompletedResearch.MaxResearch.Definition	> itemToResearch.Definition	) ? agentCompletedResearch.MaxResearch.Definition	: itemToResearch.Definition	;
		agentCompletedResearch.MaxResearch.Modifier		= (agentCompletedResearch.MaxResearch.Modifier		> itemToResearch.Modifier	) ? agentCompletedResearch.MaxResearch.Modifier		: itemToResearch.Modifier	;
	}

	template <typename _EntityType>
	static bool													equipIfResearched
		( const ::klib::SEntityTables								& tables
		, int16_t													selectedChoice
		, CCharacter												& playerAgent
		, SEntityContainer<_EntityType>								& playerInventory
		, _EntityType												& agentEquippedEntity
		, const SResearchGroup<_EntityType>							& playerCompletedResearch
		, const SResearchGroup<_EntityType>							& agentCompletedResearch
		, const ::klib::SEntityTable<_EntityType>					& table
		, const ::gpk::view_const_char								& modifierTypeName
		, ::gpk::array_pod<char_t>									& messageSuccess
		, ::gpk::array_pod<char_t>									& messageError
		)
	{
		_EntityType			selectedItem	= playerInventory[selectedChoice].Entity;
		if (selectedItem.Definition	&&	playerCompletedResearch.Definitions.FindElement(selectedItem.Definition) == -1 && agentCompletedResearch.Definitions.FindElement(selectedItem.Definition) == -1) {
			messageError = ::gpk::view_const_string{"You can't use "};
			messageError.append(table.Definitions[selectedItem.Definition].Name);
			messageError.append_string(" without researching it first!");
			return false;
		}
		else if (selectedItem.Modifier && playerCompletedResearch.Modifiers.FindElement(selectedItem.Modifier) == -1 && agentCompletedResearch.Modifiers.FindElement(selectedItem.Modifier) == -1) {
			char				modifierName[72]	= {};
			sprintf_s(modifierName, table.Modifiers[selectedItem.Modifier].Name.begin(), modifierTypeName.begin());
			char				message		[256]	= {};
			sprintf_s(message, "You can't use %s without researching them first!", modifierName);
			messageError	= ::gpk::view_const_string{message};
			return false;
		}
		else {
			playerInventory.AddElement(agentEquippedEntity);
			agentEquippedEntity	= selectedItem;
			playerInventory.DecreaseEntity(selectedChoice);
			messageSuccess		= ::gpk::view_const_string{"You equipped "};
			messageSuccess.append(::klib::getEntityName(table, selectedItem));
			messageSuccess.append_string(" Lv. ");
			char					level [32];
			sprintf_s(level, "%i", (int32_t)selectedItem.Level);
			messageSuccess.append_string(level);
			messageSuccess.append_string(" to ");
			messageSuccess.append(playerAgent.Name);
			playerAgent.Recalculate(tables);
			return true;
		}
	}

} // namespace

#endif // __GAMEBASE_H__98236498027346892734689273__
