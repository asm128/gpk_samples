#include "EntityPoints.h"

#include "klib_draw_misc.h"
#include <cstring>

#ifndef KLIB_ENTITY_H__38924092634721346098170219783096__
#define KLIB_ENTITY_H__38924092634721346098170219783096__

namespace klib
{
#pragma pack(push, 1)
	struct SEntity {
							int16_t							Definition			=  0;
							int16_t							Modifier			=  0;
							int16_t							Level				=  1;
							int8_t							Owner				= -1;
							//ENTITY_TYPE						Type				= ENTITY_TYPE_INVALID;

		inline constexpr									SEntity				(const SEntity& other)									= default;
		inline constexpr									SEntity
			(	int16_t		index		=  0
			,	int16_t		modifier	=  0
			,	int16_t		level		=  1
			,	int8_t		owner		= -1
			//,	ENTITY_TYPE type		= ENTITY_TYPE_INVALID
			)
			: Definition	(index		)
			, Modifier		(modifier	)
			, Level			(level		)
			, Owner			(owner		)
			//, Type			(type		)
		{}

		inline				operator						SEntity				()														{ return *this; }

		inline				SEntity&						operator=			(const SEntity& other)						noexcept	= default;
		inline constexpr	bool							operator==			(const SEntity& other)				const	noexcept	{ return Definition == other.Definition && Modifier == other.Modifier && Level == other.Level && Owner == other.Owner;  }

							void							Print				() const
		{
			printf("Index ......: %i.\n",	Definition	);
			printf("Modifier ---: %i.\n",	Modifier	);
			printf("Level ......: %i.\n",	Level		);
			printf("Owner ------: %i.\n",	Owner		);
			//printf("Type ------: %i.\n",	::gpk::get_value_label(Type).begin());
		}
	};

#pragma pack(pop)
	template<typename _EntityType>
	struct SEntitySlot {
							uint32_t									Count				= 0;
							_EntityType									Entity				= {};
	};

	template<typename _EntityType>
	struct SEntityContainer {
							::gpk::array_pod<SEntitySlot<_EntityType>>	Slots;

		inline constexpr												SEntityContainer	()														= default;
		inline constexpr												SEntityContainer	(const SEntityContainer& other)							= default;

		template<size_t _tSize>
		inline															SEntityContainer
			(	uint32_t			slotCount
			,	const _EntityType	(&entitySlots)[_tSize]
			) {
			slotCount														= (_tSize < slotCount) ? _tSize : slotCount;
			Slots.resize(slotCount);
			for(uint32_t iSlot = 0; iSlot < slotCount; ++iSlot)
				Slots[iSlot]													= entitySlots[iSlot];
		}

							const uint32_t					size				()									const	noexcept	{ return Slots.size(); }

		inline				const SEntitySlot<_EntityType>&	operator[]			(uint32_t index)					const				{ return Slots[index]; }
		inline				SEntitySlot<_EntityType>&		operator[]			(uint32_t index)										{ return Slots[index]; }

		// Returns false if the container is full
		inline				bool							AddElement			(const _EntityType& element)							{
			// look in the inventory for the name so we just increment the counter instead of adding the item
			for(uint32_t iSlot = 0, count = Slots.size(); iSlot < count; ++iSlot) {
				if(element == Slots[iSlot].Entity) {
					++Slots[iSlot].Count;
					return true;
				}
			}
			return Slots.push_back({1, element}) >= 0;
		}

		// Returns the amount of remaining items of the type pointed by the provided index
		inline				uint32_t						DecreaseEntity		(uint32_t index)										{
			if(index < 0 || index >= Slots.size()) {
				error_printf("Invalid index! Index: %i.", (int32_t)index);
				return (uint32_t)-1;
			}
			if( --Slots[index].Count )
				return Slots[index].Count;

			Slots.remove_unordered(index);
			return 0;
		}
		// Returns the index of the provided type if found or -1 if not found.
		inline				int32_t							FindElement			(const _EntityType& element)		const	noexcept	{
			for(uint32_t i=0, count= Slots.size(); i < count; ++i)
				if(Slots[i].Entity == element)
					return i;
			return -1;
		}
	};


	template<typename _TEntity>
	struct SEntityRecord {
		typedef				_TEntity						TEntity				;

							SEntityPoints					Points				;
							SEntityFlags					Flags				;
							::gpk::view_const_string		Name				;
	};

	template <typename _tEntity>
	struct SEntityTable {
		::gpk::view_array<const SEntityRecord<_tEntity>>	Definitions	;
		::gpk::view_array<const SEntityRecord<_tEntity>>	Modifiers	;
	};

	template<typename _EntityType>
	SEntityPoints						getEntityPoints					(const SEntityTable<_EntityType> & table, const SEntity & entity) { return (table.Definitions[entity.Definition].Points + table.Modifiers[entity.Modifier].Points) * (_EntityType::getMultipliers() * entity.Level); }
	template<typename _EntityType>
	SEntityFlags						getEntityFlags					(const SEntityTable<_EntityType> & table, const SEntity & entity) { return (table.Definitions[entity.Definition].Flags | table.Modifiers[entity.Modifier].Flags); }
	// Combines two record tables to get the names and combine them as one for display purposes.
	template<typename _EntityType>
	::gpk::array_pod<char_t>			getEntityName					(const SEntityTable<_EntityType> & table, const SEntity & entity) {
		char									formattedName	[128]			= {};
		sprintf_s(formattedName, table.Modifiers[entity.Modifier].Name.begin(), table.Definitions[entity.Definition].Name.begin());
		return ::gpk::view_const_string{formattedName};
	}
};

#endif // KLIB_ENTITY_H__38924092634721346098170219783096__
