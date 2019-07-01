#include "CombatPoints.h"
#include "CombatStatus.h"

#ifndef KLIB_ENTITYPOINTS_H__9238409723640921876409234__
#define KLIB_ENTITYPOINTS_H__9238409723640921876409234__

namespace klib
{
#pragma pack(push, 1)

	struct SEntityPointsMultiplier {
							SLifePointsMultiplier		LifeMax										= {};
							SLifePointsMultiplier		LifeCurrent									= {};
							SAttackPointsMultiplier		Attack										= {};
							SFitnessPointsMultiplier	Fitness										= {};
							double						Coins										= 1.0;
							double						PriceBuy									= 1.0;
							double						CostMaintenance								= 1.0;

		constexpr										SEntityPointsMultiplier						()															= default;
		constexpr										SEntityPointsMultiplier
			(	const SLifePointsMultiplier			& lifeMax				
			,	const SLifePointsMultiplier			& lifeCurrent			
			,	const SAttackPointsMultiplier		& attack				
			,	const SFitnessPointsMultiplier		& fitness				
			,	double								coins				
			,	double								priceBuy			
			,	double								costMaintenance		
			) 
				: LifeMax				(lifeMax				)
				, LifeCurrent			(lifeCurrent			)
				, Attack				(attack					)
				, Fitness				(fitness				)
				, Coins					(coins					)
				, PriceBuy				(priceBuy				)
				, CostMaintenance		(costMaintenance		)
		{}

		inline constexpr	SEntityPointsMultiplier		operator *									(const int32_t level)					const				{ return { LifeMax*level, LifeCurrent*level, Attack*level, Fitness*level, Coins*level, PriceBuy*level, CostMaintenance*level }; }
							void						Print										()										const				{
			printf("- Max Life:\n");		LifeMax		.Print();
			printf("- Current Life:\n");	LifeCurrent	.Print();
			printf("- Attack Points:\n");	Attack		.Print();
			printf("- Fitness Points:\n");	Fitness		.Print();
			printf("- Coins...........: %f.\n", Coins);
			printf("- Buy Price.......: %f.\n", PriceBuy);
			printf("- Maintenace Cost.: %f.\n", CostMaintenance);
		}
	};

	// 
	struct SEntityPoints {
							SLifePoints					LifeMax			;
							SLifePoints					LifeCurrent		;
							SAttackPoints				Attack			;
							SFitnessPoints				Fitness			;
							int32_t						Coins			;
							int32_t						PriceBuy		;
							int32_t						CostMaintenance	;
	
							void						Print										()										const				{
			printf("- Max Life:\n");			LifeMax		.Print();
			printf("- Current Life:\n");		LifeCurrent	.Print();
			//printf("- Life Gauges:\n");		LifeGauges	.Print();
			printf("- Attack Points:\n");		Attack		.Print();
			printf("- Fitness Points:\n");		Fitness		.Print();

			printf("- Coins.............: %i.\n", (int32_t)Coins);
			printf("- Buy Price.........: %i.\n", (int32_t)PriceBuy);
			printf("- Maintenance Cost..: %i.\n", (int32_t)CostMaintenance);
		}

		inline constexpr	SEntityPoints				operator+									(const SEntityPoints& other)			const	noexcept	{ 
			return { LifeMax+other.LifeMax, LifeCurrent+other.LifeCurrent, Attack+other.Attack, Fitness+other.Fitness, Coins+other.Coins, PriceBuy+other.PriceBuy, CostMaintenance+other.CostMaintenance };
		}

		inline constexpr	SEntityPoints				operator-									(const SEntityPoints& other)			const	noexcept	{ 
			return { LifeMax-other.LifeMax, LifeCurrent-other.LifeCurrent, Attack-other.Attack, Fitness-other.Fitness, Coins-other.Coins, PriceBuy-other.PriceBuy, CostMaintenance-other.CostMaintenance };
		}

		inline				SEntityPoints				operator*									(const SEntityPointsMultiplier& other)	const	noexcept	{ 
			double												coins										= Coins				*	other.Coins				;
			double												priceBuy									= PriceBuy			*	other.PriceBuy			;
			double												costMaintenance								= CostMaintenance	*	other.CostMaintenance	;

			coins											= (	coins			< 0 &&	coins			> -1.0) ? -1.000000001 : (	coins			> 0 &&	coins			< 1.0) ?	coins			= 1.000000001 : coins			;
			priceBuy										= (	priceBuy		< 0 &&	priceBuy		> -1.0) ? -1.000000001 : (	priceBuy		> 0 &&	priceBuy		< 1.0) ?	priceBuy		= 1.000000001 : priceBuy		;
			costMaintenance									= (	costMaintenance	< 0 &&	costMaintenance	> -1.0) ? -1.000000001 : (	costMaintenance	> 0 &&	costMaintenance	< 1.0) ?	costMaintenance	= 1.000000001 : costMaintenance	;

			return	{ LifeMax					*	other.LifeMax		
					, LifeCurrent				*	other.LifeCurrent	
					, Attack					*	other.Attack	
					, Fitness					*	other.Fitness
					, (int32_t)(coins			)	
					, (int32_t)(priceBuy		)
					, (int32_t)(costMaintenance	)
					};
		}
	};
	
	// Can be status immunity, status inflict, attack effect, defend effect or passive effect and the allowance of tech, attack types and entity grades.
	struct SEntityFlags {
							SEntityEffect				Effect			;
							SEntityStatus				Status			;
							SEntityGrade				Tech			;

		inline constexpr	bool						operator==		(const SEntityFlags& other)											const	noexcept	{ return Effect == other.Effect && Status == other.Status && Tech == other.Tech;	}
		inline constexpr	SEntityFlags				operator|		(const SEntityFlags& other)											const	noexcept	{ return { Effect | other.Effect, Status | other.Status, Tech | other.Tech };		}

							void						Print			()																	const				{
			Effect	.Print();
			Status	.Print();
			Tech	.Print();
		}
	};

	struct SEntityCombo {
							SEntityPoints				Points	;
							SEntityFlags				Flags	;

		inline				SEntityCombo				operator*		(const SEntityPointsMultiplier& other)								const	noexcept	{ return { Points * other, Flags };													}
		inline constexpr	SEntityCombo				operator+		(const SEntityCombo& other)											const	noexcept	{ return { Points + other.Points,  Flags | other.Flags };							}

							void						Print			()																	const				{
			printf("- Status Points:\n"	); Points	.Print();
			printf("- Status Flags:\n"	); Flags	.Print();
		}
	};

#pragma pack(pop)
} // klib

#endif // KLIB_ENTITYPOINTS_H__9238409723640921876409234__

