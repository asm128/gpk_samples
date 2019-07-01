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
		SWeapon													Weapon					= {0,0,1,-1};	// Index, ModifierIndex, Level
		SAccessory												Accessory				= {0,0,1,-1};	// Index, ModifierIndex, Level
		SProfession												Profession				= {0,0,1,-1};	// Index, ModifierIndex, Level
		SArmor													Armor					= {0,0,1,-1};	// Index, ModifierIndex, Level
		SVehicle												Vehicle					= {0,0,1,-1};	// Index, ModifierIndex, Level
		SFacility												Facility				= {0,0,1,-1};	// Index, ModifierIndex, Level
		SStageProp												StageProp				= {0,0,1,-1};	// Index, ModifierIndex, Level
	};																					

	struct SCharacterResearch {																					
		SResearchGroup<SWeapon		>							Weapon					= {};
		SResearchGroup<SAccessory	>							Accessory				= {};
		SResearchGroup<SProfession	>							Profession				= {};
		SResearchGroup<SArmor		>							Armor					= {};
		SResearchGroup<SVehicle		>							Vehicle					= {};
		SResearchGroup<SFacility	>							Facility				= {};
		SResearchGroup<SStageProp	>							StageProp				= {};
	};

#define MAX_INVENTORY_SLOTS 256
	struct SCharacterInventory {
		SEntityContainer<SItem		, MAX_INVENTORY_SLOTS>		Items;
		SEntityContainer<SProfession, MAX_INVENTORY_SLOTS>		Profession;
		SEntityContainer<SWeapon	, MAX_INVENTORY_SLOTS>		Weapon;
		SEntityContainer<SArmor		, MAX_INVENTORY_SLOTS>		Armor;
		SEntityContainer<SAccessory	, MAX_INVENTORY_SLOTS>		Accessory;
		SEntityContainer<SVehicle	, MAX_INVENTORY_SLOTS>		Vehicle;
		SEntityContainer<SStageProp	, MAX_INVENTORY_SLOTS>		StageProp;
		SEntityContainer<SFacility	, MAX_INVENTORY_SLOTS>		Facility;

		int32_t													GetCount				(ENTITY_TYPE entityType) const	{
			int32_t result = 0;
			switch(entityType) {
			//case ENTITY_TYPE_CHARACTER	:	return 
			case ENTITY_TYPE_PROFESSION	: result = Profession	.Count; break;
			case ENTITY_TYPE_WEAPON		: result = Weapon		.Count; break;
			case ENTITY_TYPE_ARMOR		: result = Armor		.Count; break;
			case ENTITY_TYPE_ACCESSORY	: result = Accessory	.Count; break;
			case ENTITY_TYPE_VEHICLE	: result = Vehicle		.Count; break;
			case ENTITY_TYPE_FACILITY	: result = Facility		.Count; break;
			case ENTITY_TYPE_STAGE_PROP	: result = StageProp	.Count; break;
			case ENTITY_TYPE_ITEM		: result = Items		.Count; break;
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

							void								RecalculateFinalPoints	()										noexcept	;
							void								RecalculateFinalFlags	()										noexcept	;
							void								Recalculate				()										noexcept	{ RecalculateFinalPoints(); RecalculateFinalFlags();	}

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

	static inline		void								rest(SCharacter& character) {
		character.Recalculate();
		character.Points.LifeCurrent = character.FinalPoints.LifeMax;
	}
#pragma pack(pop)



	class CCharacter : public SCharacter {
	public:
						::gpk::SCoord3<int32_t>					Position				= {};
						::std::string							Name					= "Unnamed";

																CCharacter				()						= default;
																CCharacter				(int maxHP, int hitChance, int attack, int coins, SFitnessPoints speed, SEntityEffect characterEffect, SEntityStatus characterStatus, const ::std::string& name)
		: SCharacter	(maxHP, hitChance, attack, coins, speed, characterEffect, characterStatus)
		, Position		({0,0})
		, Name			(name)
		{}
	};

	class CDeadCharacter {
	public:
						SCharacterScore							Score					= {};	
						::gpk::label							Name					= "Unnamed";

		inline													CDeadCharacter			()									= default;
		inline													CDeadCharacter			(const CCharacter& deadCharacter)
			:	Score	(deadCharacter.Score)
			,	Name	(deadCharacter.Name.c_str(), (uint32_t)deadCharacter.Name.size())
		{}
	};
	
	template<typename _TEntity>
	void														completeAgentResearch	(klib::SResearchGroup<_TEntity>& agentCompletedResearch, const _TEntity& itemToResearch)	{
		agentCompletedResearch.Definitions	.AddElement(itemToResearch.Definition	);
		agentCompletedResearch.Modifiers	.AddElement(itemToResearch.Modifier		);
		agentCompletedResearch.MaxResearch.Definition	= (agentCompletedResearch.MaxResearch.Definition	> itemToResearch.Definition	) ? agentCompletedResearch.MaxResearch.Definition	: itemToResearch.Definition	;
		agentCompletedResearch.MaxResearch.Modifier		= (agentCompletedResearch.MaxResearch.Modifier		> itemToResearch.Modifier	) ? agentCompletedResearch.MaxResearch.Modifier		: itemToResearch.Modifier	;
	}

	template <typename _EntityType, size_t _MaxSlots, size_t _SizeDefinitions, size_t _SizeModifiers>
	static bool													equipIfResearched
		( int16_t selectedChoice
		, CCharacter&								playerAgent
		, SEntityContainer<_EntityType, _MaxSlots>& playerInventory
		, _EntityType&								agentEquippedEntity
		, const SResearchGroup<_EntityType>&		playerCompletedResearch
		, const SResearchGroup<_EntityType>&		agentCompletedResearch
		, const SEntityRecord<_EntityType>			(&entityDefinitions)	[_SizeDefinitions]
		, const SEntityRecord<_EntityType>			(&entityModifiers)		[_SizeModifiers]
		, const ::gpk::label&						modifierTypeName
		, ::std::string&							messageSuccess
		, ::std::string&							messageError
		)
	{
		_EntityType			selectedItem	= playerInventory[selectedChoice].Entity;
		if (selectedItem.Definition	&&	playerCompletedResearch.Definitions.FindElement(selectedItem.Definition) == -1 && agentCompletedResearch.Definitions.FindElement(selectedItem.Definition) == -1) {
			messageError = "You can't use " + std::string(entityDefinitions[selectedItem.Definition].Name.begin()) + " without researching it first!";
			return false;
		}
		else if (selectedItem.Modifier && playerCompletedResearch.Modifiers.FindElement(selectedItem.Modifier) == -1 && agentCompletedResearch.Modifiers.FindElement(selectedItem.Modifier) == -1) {
			char				modifierName[72]	= {};
			sprintf_s(modifierName, entityModifiers[selectedItem.Modifier].Name.begin(), modifierTypeName.begin());
			char				message		[256]	= {};
			sprintf_s(message, "You can't use %s without researching them first!", modifierName);
			messageError	= message;
			return false;
		}
		else {
			playerInventory.AddElement(agentEquippedEntity);
			agentEquippedEntity	= selectedItem;
			playerInventory.DecreaseEntity(selectedChoice);
			messageSuccess		= "You equipped " + getEntityName(selectedItem, entityDefinitions, entityModifiers) + " Lv. " + std::to_string(selectedItem.Level) + " to " + playerAgent.Name;
			playerAgent.Recalculate();
			return true;
		}
	}

} // namespace

#endif // __GAMEBASE_H__98236498027346892734689273__
