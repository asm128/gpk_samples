#include <cstdint>
#include <cstdio>

#ifndef __COMBATPOINTS_H__928374092873409238650160213__
#define __COMBATPOINTS_H__928374092873409238650160213__

namespace klib
{
#pragma pack(push, 1)

	struct SLifePointsMultiplier {
							double						Health	;
							double						Mana	;
							double						Shield	;

		inline constexpr	SLifePointsMultiplier		operator *				(const int32_t level)									const	noexcept	{ return { Health*level , Mana*level , Shield*level }; }
							void						Print					()														const	noexcept	{
			::printf("Health .....: %f.\n",	Health	);
			::printf("Mana -------: %f.\n",	Mana	);
			::printf("Shield .....: %f.\n",	Shield	);
		}
	};

	struct SLifePoints {
							int32_t						Health	;
							int32_t						Mana	;
							int32_t						Shield	;

		inline constexpr	bool						operator ==				(const SLifePoints& other)								const	noexcept	{ return Health == other.Health && Mana == other.Mana && Shield == other.Shield; }
		inline constexpr	SLifePoints					operator +				(const SLifePoints& other)								const	noexcept	{ return { Health+other.Health, Mana+other.Mana, Shield+other.Shield }; }
		inline constexpr	SLifePoints					operator -				(const SLifePoints& other)								const	noexcept	{ return { Health-other.Health, Mana-other.Mana, Shield-other.Shield }; }
		inline				SLifePoints					operator *				(const SLifePointsMultiplier& other)					const	noexcept	{ 
			double												health					= Health	*	other.Health	;
			double												mana					= Mana		*	other.Mana		;
			double												shield					= Shield	*	other.Shield	;

			health											= (	health	< 0 &&	health	> -1.0) ? -1.000000001 : (	health	> 0 &&	health	< 1.0) ?	health	= 1.000000001 : health	;
			mana											= (	mana	< 0 &&	mana	> -1.0) ? -1.000000001 : (	mana	> 0 &&	mana	< 1.0) ?	mana	= 1.000000001 : mana	;
			shield											= (	shield	< 0 &&	shield	> -1.0) ? -1.000000001 : (	shield	> 0 &&	shield	< 1.0) ?	shield	= 1.000000001 : shield	;

			return	{ (int32_t)health	
					, (int32_t)mana	
					, (int32_t)shield	
					}; 
		}

							SLifePoints&				operator +=				(const SLifePoints& other)										noexcept	{ 
			Health											+= other.Health; 
			Mana											+= other.Mana; 
			Shield											+= other.Shield; 
			return *this; 
		}
			
							void						Print					()														const				{
			::printf("Health .....: %i.\n",	Health	);
			::printf("Mana -------: %i.\n",	Mana	);
			::printf("Shield .....: %i.\n",	Shield	);
		}
	};

	struct SFitnessPointsMultiplier {
		double											Attack					;
		double											Movement				;
		double											Reflexes				;
		double											Sight					;

		constexpr			SFitnessPointsMultiplier	operator *				(const int32_t level)									const	noexcept	{ return { Attack*level, Movement*level, Reflexes*level, Sight*level }; }

		void											Print					()														const				{
			::printf("Attack .....: %f.\n",	Attack		);
			::printf("Movement ---: %f.\n",	Movement	);
			::printf("Reflexes ...: %f.\n",	Reflexes	);
		}
	};

	struct SFitnessPoints {
		int32_t											Attack		;
		int32_t											Movement	;
		int32_t											Reflexes	;
		int32_t											Sight		;

		constexpr			SFitnessPoints				operator +				(const SFitnessPoints& other)							const	noexcept	{ return { Attack+other.Attack, Movement+other.Movement, Reflexes+other.Reflexes , Sight+other.Sight }; }
		constexpr			SFitnessPoints				operator -				(const SFitnessPoints& other)							const	noexcept	{ return { Attack-other.Attack, Movement-other.Movement, Reflexes-other.Reflexes , Sight-other.Sight }; }
							SFitnessPoints&				operator+=				(const SFitnessPoints& other)									noexcept	{ Attack += other.Attack; Movement += other.Movement; Reflexes += other.Reflexes; Sight += other.Sight; return *this; }
							SFitnessPoints				operator *				(const SFitnessPointsMultiplier& other)					const				{ 
			double												attack					= Attack	*	other.Attack	;
			double												movement				= Movement	*	other.Movement	;
			double												reflexes				= Reflexes	*	other.Reflexes	;
			double												sight					= Sight		*	other.Sight		;

			attack											= (	attack		< 0 &&	attack		> -1.0) ? -1.000000001 : (	attack		> 0 &&	attack		< 1.0) ?	attack		= 1.000000001 : attack		;
			movement										= (	movement	< 0 &&	movement	> -1.0) ? -1.000000001 : (	movement	> 0 &&	movement	< 1.0) ?	movement	= 1.000000001 : movement	;
			reflexes										= (	reflexes	< 0 &&	reflexes	> -1.0) ? -1.000000001 : (	reflexes	> 0 &&	reflexes	< 1.0) ?	reflexes	= 1.000000001 : reflexes	;
			sight											= (	sight		< 0 &&	sight		> -1.0) ? -1.000000001 : (	sight		> 0 &&	sight		< 1.0) ?	sight		= 1.000000001 : sight		;

			return	{ (int32_t)attack		
					, (int32_t)movement	
					, (int32_t)reflexes	
					, (int32_t)sight		
					}; 
		}

							void						Print					()														const				{
			::printf("Attack .....: %i.\n",	Attack		);
			::printf("Movement ---: %i.\n",	Movement	);
			::printf("Reflexes ...: %i.\n",	Reflexes	);
			::printf("Sight ------: %i.\n",	Sight		);
		}
	};

	struct SAttackPointsMultiplier {
							double						Hit						;
							double						Damage					;
							SLifePointsMultiplier		DirectDamage			;
							double						Absorption				;
							double						Range					;

		constexpr			SAttackPointsMultiplier		operator *				(const int32_t level)									const	noexcept	{ return { Hit*level, Damage*level, DirectDamage*level, Absorption*level, Range*level }; }

							void						Print					()														const				{
			::printf("Hit ........: %f.\n",	Hit			);
			::printf("Damage .....: %f.\n",	Damage		);
			::printf("- Extra Point Damage:\n");
			DirectDamage.Print();
			::printf("Absorption -: %f.\n",	Absorption	);
			::printf("Range ......: %f.\n",	Range		);
		}
	};

	struct SAttackPoints {
							int32_t						Hit						;
							int32_t						Damage					;
							SLifePoints					DirectDamage			;
							int32_t						Absorption				;
							int32_t						Range					;

		constexpr			SAttackPoints				operator +				(const SAttackPoints& other)							const	noexcept	{ return { Hit+other.Hit, Damage+other.Damage, DirectDamage+other.DirectDamage, Absorption+other.Absorption, Range+other.Range }; }
		constexpr			SAttackPoints				operator -				(const SAttackPoints& other)							const	noexcept	{ return { Hit-other.Hit, Damage-other.Damage, DirectDamage-other.DirectDamage, Absorption-other.Absorption, Range-other.Range }; }
							SAttackPoints				operator *				(const SAttackPointsMultiplier& other)					const				{ 
			double												hit						= Hit			*	other.Hit			;
			double												damage					= Damage		*	other.Damage		;
			double												absorption				= Absorption	*	other.Absorption	;
			double												range					= Range			*	other.Range			;

			hit												= (	hit			< 0 &&	hit			> -1.0) ? -1.000000001 : (	hit			> 0 &&	hit			< 1.0) ?	hit			= 1.000000001 : hit			;
			damage											= (	damage		< 0 &&	damage		> -1.0) ? -1.000000001 : (	damage		> 0 &&	damage		< 1.0) ?	damage		= 1.000000001 : damage		;
			absorption										= (	absorption	< 0 &&	absorption	> -1.0) ? -1.000000001 : (	absorption	> 0 &&	absorption	< 1.0) ?	absorption	= 1.000000001 : absorption	;
			range											= (	range		< 0 &&	range		> -1.0) ? -1.000000001 : (	range		> 0 &&	range		< 1.0) ?	range		= 1.000000001 : range		;

			return	{ (int32_t)hit			
					, (int32_t)damage		
					, DirectDamage * other.DirectDamage
					, (int32_t)absorption	
					, (int32_t)range		
				}; 
		}

							SAttackPoints&				operator +=				(const SAttackPoints& other)									noexcept	{ 
			Hit												+= other.Hit; 
			Damage											+= other.Damage; 
			DirectDamage									+= other.DirectDamage; 
			Absorption										+= other.Absorption; 
			Range											+= other.Range; 
			return *this; 
		}

							void						Print					()														const				{
			::printf("Hit ........: %i.\n",	Hit			);
			::printf("Damage .....: %i.\n",	Damage		);
			::printf("- Extra Point Damage:\n");
			DirectDamage.Print();
			::printf("Absorption -: %%%i.\n",	Absorption	);
			::printf("Range ......: %%%i.\n",	Range		);
		}
	};

#pragma pack(pop)
} //namespace

#endif // __COMBATPOINTS_H__928374092873409238650160213__
