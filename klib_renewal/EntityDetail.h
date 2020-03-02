#include "Game.h"

namespace klib {
	template <typename _TEntity>
	void								drawEntityDetail
		( ::gpk::view_grid<char>									display
		, ::gpk::view_grid<uint16_t>								textAttributes
		, int32_t													offsetY
		, int32_t													offsetX
		, const SEntity												& entity
		, const SEntityTable<_TEntity>								& table
		, const ::gpk::view_const_char								& entityTypeName
		) {
		::gpk::array_pod<char_t>					nameAndLevelText					= ::gpk::view_const_string{"-- "};
		nameAndLevelText.append(entityTypeName);
		nameAndLevelText.append_string(": ");
		lineToGridColored(display, textAttributes, COLOR_GREEN , offsetY++, offsetX, ::klib::SCREEN_LEFT, nameAndLevelText.begin(), (uint32_t)nameAndLevelText.size());
		nameAndLevelText					= ::klib::getEntityName(table, entity);
		nameAndLevelText.append_string(" Lv. ");
		char										level [32];
		sprintf_s(level, "%i", entity.Level);
		nameAndLevelText.append_string(level);

		lineToGridColored(display, textAttributes, COLOR_YELLOW, ++offsetY, offsetX, ::klib::SCREEN_LEFT, nameAndLevelText.begin(), (uint32_t)nameAndLevelText.size());

		const SEntityPoints						entityPoints						= ::klib::getEntityPoints(table, entity);
		static const ::gpk::view_const_string	formatPoints						= "%-21.21s: %-10.10s";
		static const ::gpk::view_const_string	formatCoins							= "%-21.21s: %-11.11s";
		char									formattedGauge[32]					= {};
		offsetY								+= 2;
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Health				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Max Health"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Shield				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Max Shield"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Mana				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Max Mana"				, formattedGauge);
		offsetY								+= 1;	//
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Health			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Health per turn"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Shield			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Shield per turn"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Mana			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Mana per turn"			, formattedGauge);
		offsetY								+= 1;	//
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Hit					);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Hit Chance"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Damage				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Damage"					, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Health	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Direct Damage Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Shield	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Direct Damage Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Mana	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Direct Damage Mana"		, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Absorption			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Absorption"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Range				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Range"					, formattedGauge);
		offsetY								+= 1;	//
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Attack				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Attack Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Movement			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Movement Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Reflexes			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Reflexes"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Sight				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints.begin(), "Sight"					, formattedGauge);
		uint16_t								color;//
		sprintf_s(formattedGauge, "%lli", entityPoints.Coins					);	printfToGrid(display, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins.begin()	, "Coins per turn"			, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (entityPoints.Coins >= 0) ? COLOR_ORANGE : COLOR_RED), 1, 11);
		sprintf_s(formattedGauge, "%lli", entityPoints.PriceBuy / 2				);	printfToGrid(display, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins.begin()	, "Sell Price"				, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
		sprintf_s(formattedGauge, "%lli", entityPoints.CostMaintenance			);	printfToGrid(display, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins.begin()	, "Maintenance Cost"		, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
	}
}
