#include "Game.h"

namespace klib {
	template <typename _TEntity>
	void									drawEntityDetail
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
		lineToGridColored(display, textAttributes, ::klib::ASCII_COLOR_INDEX_GREEN , offsetY++, offsetX, ::klib::SCREEN_LEFT, nameAndLevelText.begin(), (uint32_t)nameAndLevelText.size());
		nameAndLevelText						= ::klib::getEntityName(table, entity);
		nameAndLevelText.append_string(" Lv. ");
		char										level [32];
		sprintf_s(level, "%i", entity.Level);
		nameAndLevelText.append_string(level);

		lineToGridColored(display, textAttributes, ::klib::ASCII_COLOR_INDEX_YELLOW, ++offsetY, offsetX, ::klib::SCREEN_LEFT, nameAndLevelText.begin(), (uint32_t)nameAndLevelText.size());

		const SEntityPoints							entityPoints						= ::klib::getEntityPoints(table, entity);
		static constexpr	const char				formatPoints	[]					= "%-21.21s: %-10.10s";
		static constexpr	const char				formatCoins		[]					= "%-21.21s: %-11.11s";
		char										formattedGauge	[32]				= {};
		offsetY									+= 2;
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Health				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Max Health"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Shield				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Max Shield"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Mana				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Max Mana"				, formattedGauge);
		offsetY									+= 1;	//
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Health			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Health per turn"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Shield			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Shield per turn"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Mana			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Mana per turn"			, formattedGauge);
		offsetY									+= 1;	//
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Hit					);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Hit Chance"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Damage				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Damage"					, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Health	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Direct Damage Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Shield	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Direct Damage Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Mana	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Direct Damage Mana"		, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Absorption			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Absorption"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Attack.Range				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Range"					, formattedGauge);
		offsetY									+= 1;	//
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Attack				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Attack Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Movement			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Movement Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Reflexes			);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Reflexes"				, formattedGauge);
		sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Sight				);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Sight"					, formattedGauge);
		uint16_t									color;//
		sprintf_s(formattedGauge, "%lli", entityPoints.Coins					);	printfToGrid(display, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins	, "Coins per turn"			, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (entityPoints.Coins >= 0) ? ::klib::ASCII_COLOR_INDEX_ORANGE : ::klib::ASCII_COLOR_INDEX_RED), 1, 11);
		sprintf_s(formattedGauge, "%lli", entityPoints.PriceBuy / 2				);	printfToGrid(display, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins	, "Sell Price"				, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);
		sprintf_s(formattedGauge, "%lli", entityPoints.CostMaintenance			);	printfToGrid(display, ++offsetY	, offsetX, ::klib::SCREEN_LEFT, formatCoins	, "Maintenance Cost"		, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);
	}
}
