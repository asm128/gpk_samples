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
		::klib::SWeapon									Weapon					= {0,0,1,-1};	// Index, ModifierIndex, Level
		::klib::SAccessory								Accessory				= {0,0,1,-1};	// Index, ModifierIndex, Level
		::klib::SProfession								Profession				= {0,0,1,-1};	// Index, ModifierIndex, Level
		::klib::SArmor									Armor					= {0,0,1,-1};	// Index, ModifierIndex, Level
		::klib::SVehicle								Vehicle					= {0,0,1,-1};	// Index, ModifierIndex, Level
		::klib::SFacility								Facility				= {0,0,1,-1};	// Index, ModifierIndex, Level
		::klib::SStageProp								StageProp				= {0,0,1,-1};	// Index, ModifierIndex, Level
	};
#pragma pack(pop)

	struct SCharacterResearch {
		::klib::SResearchGroup<::klib::SWeapon		>	Weapon					= {};
		::klib::SResearchGroup<::klib::SAccessory	>	Accessory				= {};
		::klib::SResearchGroup<::klib::SProfession	>	Profession				= {};
		::klib::SResearchGroup<::klib::SArmor		>	Armor					= {};
		::klib::SResearchGroup<::klib::SVehicle		>	Vehicle					= {};
		::klib::SResearchGroup<::klib::SFacility	>	Facility				= {};
		::klib::SResearchGroup<::klib::SStageProp	>	StageProp				= {};
	};

	struct SCharacterInventory {
		::klib::SEntityContainer<::klib::SProfession>	Profession;
		::klib::SEntityContainer<::klib::SWeapon	>	Weapon;
		::klib::SEntityContainer<::klib::SArmor		>	Armor;
		::klib::SEntityContainer<::klib::SAccessory	>	Accessory;
		::klib::SEntityContainer<::klib::SVehicle	>	Vehicle;
		::klib::SEntityContainer<::klib::SFacility	>	Facility;
		::klib::SEntityContainer<::klib::SStageProp	>	StageProp;
		::klib::SEntityContainer<::klib::SItem		>	Items;

		int32_t											GetCount				(ENTITY_TYPE entityType) const	{
			int32_t result = 0;
			switch(entityType) {
			//case ::klib::ENTITY_TYPE_CHARACTER	:	return
			case ::klib::ENTITY_TYPE_PROFESSION	: result = Profession	.Slots.size(); break;
			case ::klib::ENTITY_TYPE_WEAPON		: result = Weapon		.Slots.size(); break;
			case ::klib::ENTITY_TYPE_ARMOR		: result = Armor		.Slots.size(); break;
			case ::klib::ENTITY_TYPE_ACCESSORY	: result = Accessory	.Slots.size(); break;
			case ::klib::ENTITY_TYPE_VEHICLE	: result = Vehicle		.Slots.size(); break;
			case ::klib::ENTITY_TYPE_FACILITY	: result = Facility		.Slots.size(); break;
			case ::klib::ENTITY_TYPE_STAGE_PROP	: result = StageProp	.Slots.size(); break;
			case ::klib::ENTITY_TYPE_ITEM		: result = Items		.Slots.size(); break;
			default:
				break;
			}
			return result;
		}
	};

	struct SCharacterGoods {
		SCharacterResearch								CompletedResearch		= {};
		SCharacterInventory								Inventory				= {};
	};

	struct SCharacter {
							SEntityPoints				Points					= {};
							SEntityFlags				Flags					= {};

							SEntityPoints				FinalPoints				= {};
							SEntityFlags				FinalFlags				= {};
							SEntityGauges				Gauges					= {};

							SCharacterTurnBonus			ActiveBonus				= {};
							SCharacterScore				Score					= {};
							SCharacterEquip				CurrentEquip			= {};
							SCharacterGoods				Goods					= {};

														SCharacter				()													= default;
														SCharacter				(int maxHP, int hitChance, int attack, int coins, SFitnessPoints speed, SEntityEffect characterEffect, SEntityStatus characterStatus )
			: Points	({{maxHP}, {maxHP}, {hitChance, attack, {}, 0, 0}, speed, coins, coins, coins / 10})
			, Flags		({characterEffect, characterStatus})
			, Gauges	({{maxHP, maxHP}, {0, 0}, {0, 0}})
		{}

							void						RecalculateFinalPoints	(const ::klib::SEntityTables & tables);
							void						RecalculateFinalFlags	(const ::klib::SEntityTables & tables);
							void						Recalculate				(const ::klib::SEntityTables & tables)				{ RecalculateFinalPoints(tables); RecalculateFinalFlags(tables);	}

		inline				bool						CanMove					()								const	noexcept	{ return IsAlive() && !DidLoseTurn();					}
		inline				bool						IsAlive					()								const	noexcept	{ return Points.LifeCurrent.Health > 0;					}
							bool						DidLoseTurn				()								const	noexcept	{
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
	static inline		void						rest					(const ::klib::SEntityTables & tables, SCharacter& character)		{
		character.Recalculate(tables);
		character.Points.LifeCurrent					= character.FinalPoints.LifeMax;
	}
	class CCharacter : public SCharacter {
	public:
						::gpk::n3<int32_t>			Position				= {};
						::gpk::apod<char>		Name					= ::gpk::view_const_string{"Unnamed"};

														CCharacter				()						= default;
														CCharacter				(int maxHP, int hitChance, int attack, int coins, SFitnessPoints speed, SEntityEffect characterEffect, SEntityStatus characterStatus, const ::gpk::view_const_char& name)
		: SCharacter	(maxHP, hitChance, attack, coins, speed, characterEffect, characterStatus)
		, Position		({})
		, Name			(name)
		{}
	};
	class CDeadCharacter {
	public:
						::klib::SCharacterScore			Score					= {};
						::gpk::apod<char>		Name					= "Unnamed";

		inline											CDeadCharacter			()									= default;
		inline											CDeadCharacter			(const CCharacter& deadCharacter)
			:	Score	(deadCharacter.Score)
			,	Name	(deadCharacter.Name)
		{}
	};
	template<typename _TEntity>
	void											completeAgentResearch	(klib::SResearchGroup<_TEntity>& agentCompletedResearch, const _TEntity& itemToResearch)	{
		agentCompletedResearch.Definitions	.AddElement(itemToResearch.Definition	);
		agentCompletedResearch.Modifiers	.AddElement(itemToResearch.Modifier		);
		agentCompletedResearch.MaxResearch.Definition	= (agentCompletedResearch.MaxResearch.Definition	> itemToResearch.Definition	) ? agentCompletedResearch.MaxResearch.Definition	: itemToResearch.Definition	;
		agentCompletedResearch.MaxResearch.Modifier		= (agentCompletedResearch.MaxResearch.Modifier		> itemToResearch.Modifier	) ? agentCompletedResearch.MaxResearch.Modifier		: itemToResearch.Modifier	;
	}
	template <typename _EntityType>
	int32_t											equipRestriction
		( const _EntityType								& entity
		, const ::klib::SResearchGroup<_EntityType>		& playerCompletedResearch
		, const ::klib::SResearchGroup<_EntityType>		& agentCompletedResearch
		) {
		if (entity.Definition && playerCompletedResearch.Definitions.FindElement(entity.Definition) == -1 && agentCompletedResearch.Definitions.FindElement(entity.Definition) == -1)
			return 1;
		else if (entity.Modifier && playerCompletedResearch.Modifiers.FindElement(entity.Modifier) == -1 && agentCompletedResearch.Modifiers.FindElement(entity.Modifier) == -1)
			return 2;
		return 0;
	}
	template <typename _EntityType>
	int32_t											equipRestriction
		( ::klib::SGameMessages							& messages
		, ::klib::SEntityContainer<_EntityType>			& playerInventory
		, int32_t										selectedChoice
		, const ::klib::SEntityTable<_EntityType>		& table
		, const ::klib::SResearchGroup<_EntityType>		& playerCompletedResearch
		, const ::klib::SResearchGroup<_EntityType>		& agentCompletedResearch
		, const ::gpk::view_const_char					& modifierTypeName
		) {
		const _EntityType									& selectedItem	= playerInventory[selectedChoice].Entity;
		::gpk::error_t										restriction		= ::klib::equipRestriction(selectedItem, playerCompletedResearch, agentCompletedResearch);
		if(restriction) {
			messages.UserError								= ::gpk::view_const_string{"You can't use "};
			switch(restriction) {
			case 1:
				messages.UserError.append(table.Definitions[selectedItem.Definition].Name);
				break;
			case 2:
				char												modifierName[72]	= {};
				sprintf_s(modifierName, table.Modifiers[selectedItem.Modifier].Name.begin(), modifierTypeName.begin());
				messages.UserError.append_string(modifierName);
				break;
			}
			messages.UserError.append_string(" without researching it first!");
			messages.LogError();
		}
		return restriction;
	}
	template <typename _EntityType>
	::gpk::error_t									equipForced
		( const ::klib::SEntityTables					& tables
		, int32_t										selectedChoice
		, ::klib::SEntityContainer<_EntityType>			& playerInventory
		, SEntity										& agentEquippedEntity
		, ::klib::CCharacter							& playerAgent
		, const ::klib::SEntityTable<_EntityType>		& table
		, ::klib::SGameMessages							& messages
		) {
		_EntityType											selectedItem	= playerInventory[selectedChoice].Entity;
		playerInventory.AddElement(agentEquippedEntity);
		agentEquippedEntity								= selectedItem;
		playerInventory.DecreaseEntity(selectedChoice);
		messages.UserSuccess		= ::gpk::view_const_string{"You equipped "};
		messages.UserSuccess.append(::klib::getEntityName(table, selectedItem));
		messages.UserSuccess.append_string(" Lv. ");
		char												level	[32]	= {};
		sprintf_s(level, "%i", (int32_t)selectedItem.Level);
		messages.UserSuccess.append_string(level);
		messages.UserSuccess.append_string(" to ");
		messages.UserSuccess.append(playerAgent.Name);
		messages.LogSuccess();
		playerAgent.Recalculate(tables);
		return true;
	}

	template <typename _EntityType>
	bool														equipIfResearched
		( const ::klib::SEntityTables					& tables
		, int32_t										selectedChoice
		, CCharacter									& playerAgent
		, ::klib::SEntityContainer<_EntityType>			& playerInventory
		, _EntityType									& agentEquippedEntity
		, const ::klib::SResearchGroup<_EntityType>		& playerCompletedResearch
		, const ::klib::SResearchGroup<_EntityType>		& agentCompletedResearch
		, const ::klib::SEntityTable<_EntityType>		& table
		, const ::gpk::view_const_char					& modifierTypeName
		, ::klib::SGameMessages							& messages
		) {
		if(::klib::equipRestriction(messages, playerInventory, selectedChoice, table, playerCompletedResearch, agentCompletedResearch, modifierTypeName))
			return false;
		return ::klib::equipForced(tables, selectedChoice, playerInventory, agentEquippedEntity, playerAgent, table, messages);
	}

} // namespace

#endif // __GAMEBASE_H__98236498027346892734689273__
