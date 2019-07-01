#include "EntityPoints.h"

#include "klib_draw_misc.h"
#include "gpk_label.h"

#ifndef __ENTITY_H__38924092634721346098170219783096__
#define __ENTITY_H__38924092634721346098170219783096__

namespace klib
{
#pragma pack(push, 1)

	struct SEntity {
							int16_t							Definition			=  0;
							int16_t							Modifier			=  0;
							int16_t							Level				=  1;
							int16_t							Owner				= -1;

		inline constexpr									SEntity				(const SEntity& other)									= default;
		inline constexpr									SEntity				
			(	int16_t index		= 0
			,	int16_t modifier	= 0
			,	int16_t level		= 1
			,	int16_t owner		=-1
			)
			: Definition	(index		)
			, Modifier		(modifier	)
			, Level			(level		)
			, Owner			(owner		)
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
		}
	};

	template<typename _EntityType>
	struct SEntitySlot {
							uint32_t						Count				= 0;
							_EntityType						Entity				= {};
	};

	template<typename _EntityType, size_t _Size>
	struct SEntityContainer {
		static				const uint32_t					Size				= _Size;

							uint32_t						Count				= 0;
							SEntitySlot<_EntityType>		Slots[_Size]		= {};

		inline constexpr									SEntityContainer	()														= default;
		inline constexpr									SEntityContainer	(const SEntityContainer& other)							= default;

		template<size_t _Size>
		inline												SEntityContainer	
			(	size_t slotCount
			,	const _EntityType (&entitySlots)[_Size]
			)
			: Count(0)
		{
			for(size_t slotCount = (_Size < slotCount) ? _Size : slotCount; Count < slotCount; Count++) 
				Slots[Count]										= entitySlots[Count];
		}

		inline constexpr	int32_t							GetMaxSize			()									const	noexcept	{ return _Size; }

		inline				const SEntitySlot<_EntityType>&	operator[]			(uint32_t index)					const				{ if(index >= Count) { error_printf("Invalid index! Index: %i.", (int32_t)index); }	return Slots[index]; }
		inline				SEntitySlot<_EntityType>&		operator[]			(uint32_t index)										{ if(index >= Count) { error_printf("Invalid index! Index: %i.", (int32_t)index); }	return Slots[index]; }

		// Returns false if the container is full
		inline				bool							AddElement			(const _EntityType& element)							{ 
			// look in the inventory for the name so we just increment the counter instead of adding the item
			for(uint32_t i=0, count = (_Size < Count) ? _Size : Count; i<count; i++) {
				if(element == Slots[i].Entity) {
					++Slots[i].Count;
					return true;
				}
			}

			if(Count >= ::gpk::size(Slots))
				return false;
			else {	// If we didn't return yet it means that the item was not found and we need to add it to the inventory.
				Slots[Count].Entity									= element;
				Slots[Count].Count									= 1;
				++Count;
				return true;
			}
		}

		// Returns the amount of remaining items of the type pointed by the provided index
		inline				uint32_t						DecreaseEntity		(uint32_t index)										{ 
			if(index < 0 || index >= Count) {	
				error_printf("Invalid index! Index: %i.", (int32_t)index); 
				return (uint32_t)-1;
			}	
			if( --Slots[index].Count )
				return Slots[index].Count;

			Slots[index]										= Slots[--Count];
			return 0; 
		}
		// Returns the index of the provided type if found or -1 if not found.
		inline				int32_t							FindElement			(const _EntityType& element)		const	noexcept	{ 
			for(uint32_t i=0, count=(_Size < Count) ? _Size : Count; i<count; ++i)
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
							::gpk::label					Name				;
	};

#pragma pack(pop)
};

#endif //__ENTITY_H__38924092634721346098170219783096__
