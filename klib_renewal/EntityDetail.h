#include "Game.h"

#ifndef ENTITYDETAIL_H_23627
#define ENTITYDETAIL_H_23627

//
namespace klib {
	template <typename _TEntity>
	void									drawEntityDetail
		( ::gpk::gchar									display
		, ::gpk::gu16								textAttributes
		, int32_t													offsetY
		, int32_t													offsetX
		, const SEntity												& entity
		, const SEntityTable<_TEntity>								& table
		, const ::gpk::vcc								& entityTypeName
		) {
		::gpk::apod<char>					nameAndLevelText					= ::gpk::view_const_string{"-- "};
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
		stacxpr	const char				formatPoints	[]					= "%-21.21s: %-10.10s";
		stacxpr	const char				formatCoins		[]					= "%-21.21s: %-11.11s";
		char										formattedGauge	[32]				= {};
		offsetY									+= 2;
		bool										blankLine							= false;
		if(entityPoints.LifeMax.Health				) { sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Health	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Max Health"	, formattedGauge); blankLine = true; }
		if(entityPoints.LifeMax.Shield				) { sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Shield	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Max Shield"	, formattedGauge); blankLine = true; }
		if(entityPoints.LifeMax.Mana				) { sprintf_s(formattedGauge, "%i", entityPoints.LifeMax.Mana	);	printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Max Mana"	, formattedGauge); blankLine = true; }
		if(blankLine)
			offsetY									+= 1;
		blankLine								= false;
		if(entityPoints.LifeCurrent.Health			)	{ sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Health	);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Health per turn"	, formattedGauge); blankLine = true; }
		if(entityPoints.LifeCurrent.Shield			)	{ sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Shield	);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Shield per turn"	, formattedGauge); blankLine = true; }
		if(entityPoints.LifeCurrent.Mana			)	{ sprintf_s(formattedGauge, "%i", entityPoints.LifeCurrent.Mana		);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Mana per turn"	, formattedGauge); blankLine = true; }
		if(blankLine)
			offsetY									+= 1;
		blankLine								= false;
		if(entityPoints.Attack.Hit					)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.Hit					);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Hit Chance"			, formattedGauge); blankLine = true; }
		if(entityPoints.Attack.Damage				)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.Damage				);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Damage"				, formattedGauge); blankLine = true; }
		if(entityPoints.Attack.DirectDamage.Health	)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Health	);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Direct Damage Health"	, formattedGauge); blankLine = true; }
		if(entityPoints.Attack.DirectDamage.Shield	)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Shield	);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Direct Damage Shield"	, formattedGauge); blankLine = true; }
		if(entityPoints.Attack.DirectDamage.Mana	)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.DirectDamage.Mana		);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Direct Damage Mana"	, formattedGauge); blankLine = true; }
		if(entityPoints.Attack.Absorption			)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.Absorption			);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Absorption"			, formattedGauge); blankLine = true; }
		if(entityPoints.Attack.Range				)	{ sprintf_s(formattedGauge, "%i", entityPoints.Attack.Range					);	printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Range"				, formattedGauge); blankLine = true; }
		if(blankLine)
			offsetY									+= 1;
		blankLine								= false;
		if(entityPoints.Fitness.Attack		)	{ sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Attack	);	::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Attack Speed"		, formattedGauge); blankLine = true; }
		if(entityPoints.Fitness.Movement	)	{ sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Movement	);	::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Movement Speed"	, formattedGauge); blankLine = true; }
		if(entityPoints.Fitness.Reflexes	)	{ sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Reflexes	);	::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Reflexes"			, formattedGauge); blankLine = true; }
		if(entityPoints.Fitness.Sight		)	{ sprintf_s(formattedGauge, "%i", entityPoints.Fitness.Sight	);	::klib::printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatPoints, "Sight"			, formattedGauge); blankLine = true; }
		if(blankLine)
			offsetY									+= 1;
		blankLine								= false;
		uint16_t									color;//
		if(entityPoints.Coins				)	{ sprintf_s(formattedGauge, "%lli", entityPoints.Coins				);	::klib::printfToGrid(display, offsetY, offsetX, ::klib::SCREEN_LEFT, formatCoins	, "Coins per turn"		, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (entityPoints.Coins >= 0) ? ::klib::ASCII_COLOR_INDEX_ORANGE : ::klib::ASCII_COLOR_INDEX_RED), 1, 11);	blankLine = true; }
		if(blankLine)
			offsetY									+= 1;
		blankLine								= false;
		/*if(entityPoints.PriceBuy / 2	) */	{ sprintf_s(formattedGauge, "%lli", entityPoints.PriceBuy / 2		);	::klib::printfToGrid(display,   offsetY, offsetX, ::klib::SCREEN_LEFT, formatCoins	, "Sell Price"			, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);																blankLine = true; }
		if(entityPoints.CostMaintenance		)	{ sprintf_s(formattedGauge, "%lli", entityPoints.CostMaintenance	);	::klib::printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatCoins	, "Maintenance Cost"	, formattedGauge);	valueToGrid(textAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = ::klib::ASCII_COLOR_INDEX_ORANGE), 1, 11);																blankLine = true; }
	}
}

#endif // ENTITYDETAIL_H_23627
