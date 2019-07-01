#include "Game.h"

namespace klib {


	template <typename _TEntity, size_t _Width, size_t _Depth, size_t _SizeDefinitions, size_t _SizeModifiers> 
	void								drawEntityDetail					(SWeightedDisplay<_Width, _Depth>& display_, int32_t offsetY, int32_t offsetX, const SEntity& entity, const SEntityRecord<_TEntity> (&definitions)[_SizeDefinitions], const SEntityRecord<_TEntity> (&modifiers)[_SizeModifiers], const ::gpk::label& entityTypeName)	{
		::std::string							nameAndLevelText					= "-- " + std::string(entityTypeName.begin()) + ": ";	
		lineToGridColored(display_.Screen, display_.TextAttributes, COLOR_GREEN , offsetY++, offsetX, ::klib::SCREEN_LEFT, nameAndLevelText.c_str(), (uint32_t)nameAndLevelText.size());
		nameAndLevelText					= getEntityName(entity, definitions, modifiers) + " Lv. " + std::to_string(entity.Level);	
		lineToGridColored(display_.Screen, display_.TextAttributes, COLOR_YELLOW, ++offsetY, offsetX, ::klib::SCREEN_LEFT, nameAndLevelText.c_str(), (uint32_t)nameAndLevelText.size());

		const SEntityPoints						entityPoints						= getEntityPoints(entity, definitions, modifiers);
		static const ::gpk::label				formatPoints						= "%-21.21s: %-10.10s";
		static const ::gpk::label				formatCoins							= "%-21.21s: %-11.11s";
		char									formattedGauge[32]					= {};
		offsetY								+= 2;
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Health				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Max Health"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Shield				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Max Shield"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Mana				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Max Mana"				, formattedGauge);
		offsetY								+= 1;	//																																												
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Health			);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Health per turn"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Shield			);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Shield per turn"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Mana			);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Mana per turn"			, formattedGauge);
		offsetY								+= 1;	//																																												
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Hit					);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Hit Chance"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Damage				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Damage"					, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Health	);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Direct Damage Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Shield	);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Direct Damage Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Mana	);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Direct Damage Mana"		, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Absorption			);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Absorption"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Range				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Range"					, formattedGauge);
		offsetY								+= 1;	//																																												
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Attack				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Attack Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Movement			);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Movement Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Reflexes			);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Reflexes"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Sight				);	printfToGrid(display_.Screen, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin()	, "Sight"					, formattedGauge);

		uint16_t								color;
		sprintf_s(formattedGauge, "%i", entityPoints.Coins						);	printfToGrid(display_.Screen, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins.begin()	, "Coins per turn"			, formattedGauge);	valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (entityPoints.Coins >= 0) ? COLOR_ORANGE : COLOR_RED), 1, 11);												
		sprintf_s(formattedGauge, "%i", entityPoints.PriceBuy/2					);	printfToGrid(display_.Screen, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins.begin()	, "Sell Price"				, formattedGauge);	valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);																						
		sprintf_s(formattedGauge, "%i", entityPoints.CostMaintenance			);	printfToGrid(display_.Screen, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins.begin()	, "Maintenance Cost"		, formattedGauge);	valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
	}
}
