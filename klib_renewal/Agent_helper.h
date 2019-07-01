#include "Game.h"
#include "klib_draw_misc.h"

#include "Profession.h"
#include "Weapon.h"
#include "Armor.h"
#include "Accessory.h"
#include "Facility.h"
#include "Vehicle.h"
#include "StageProp.h"

#include "gpk_view_grid.h"

#ifndef __AGENT_HELPER_H__91238648097234698723649872364923874__
#define __AGENT_HELPER_H__91238648097234698723649872364923874__

namespace klib
{
	template <typename _BitfieldType, size_t _Width, size_t _Depth>
	static int32_t							displayFlag						(SWeightedDisplay<_Width, _Depth>& display, ::gpk::SCoord2<int32_t> offset, _BitfieldType characterBits, uint32_t statusCount, uint16_t colorTitle, uint16_t colorField, const std::string& textFormat)	{
		uint16_t									color							= COLOR_GREEN;
		int32_t										iLine							= 0;
		for(uint32_t iStatus=0; iStatus<statusCount; ++iStatus) {
			_BitfieldType								statusBit						= (_BitfieldType)(1 << iStatus);
			if(0 == (statusBit & characterBits))
				continue;

			::gpk::label								valueLabel						= ::gpk::get_value_label(statusBit);
			int32_t										actualX							= printfToGridColored(display.Screen, display.TextAttributes, (color = colorTitle), offset.y+iLine, offset.x, ::klib::SCREEN_LEFT, textFormat.c_str(), valueLabel.begin());
			valueToGrid(display.TextAttributes, offset.y+iLine, (int32_t)(actualX+textFormat.size()-8), ::klib::SCREEN_LEFT, &(color = colorField), 1, (int32_t)(textFormat.size()+valueLabel.size()-8));
			++iLine;
		}

		return iLine;
	}

	template <size_t _Width, size_t _Depth>
	static int32_t							displayEntityStatus				(SWeightedDisplay<_Width, _Depth>& display, int32_t offsetY, int32_t offsetX, const SEntityStatus& entityStatus)										{
		int32_t										iLine							= 0;
		if(entityStatus.Inflict		)	iLine += displayFlag(display, {offsetX, offsetY+iLine}, entityStatus.Inflict	, MAX_COMBAT_STATUS_COUNT, COLOR_YELLOW, COLOR_RED	, "- Inflicts: %-14.14s");
		if(entityStatus.Immunity	)	iLine += displayFlag(display, {offsetX, offsetY+iLine}, entityStatus.Immunity	, MAX_COMBAT_STATUS_COUNT, COLOR_YELLOW, COLOR_CYAN	, "- Immunity: %-14.14s");

		return iLine;
	}

	template <size_t _Width, size_t _Depth>
	static int32_t							displayEntityEffect				(SWeightedDisplay<_Width, _Depth>& display, int32_t offsetY, int32_t offsetX, const SEntityEffect& entityEffect)										{
		int32_t										iLine							= 0;
		if(entityEffect.Attack	)	iLine += displayFlag(display, {offsetX, offsetY+iLine}, entityEffect.Attack	, MAX_ATTACK_EFFECT_COUNT, COLOR_YELLOW, COLOR_RED	, "- Attack effect: %-14.14s"	);
		if(entityEffect.Defend	)	iLine += displayFlag(display, {offsetX, offsetY+iLine}, entityEffect.Defend	, MAX_DEFEND_EFFECT_COUNT, COLOR_YELLOW, COLOR_CYAN	, "- Defend effect: %-14.14s"	);
		if(entityEffect.Passive	)	iLine += displayFlag(display, {offsetX, offsetY+iLine}, entityEffect.Passive	, MAX_PASSIVE_EFFECT_COUNT, COLOR_YELLOW, COLOR_GREEN	, "- Passive effect: %-14.14s"	);

		return iLine;
	}

	template <size_t _Width, size_t _Depth>
	static int32_t							displayEntityTechnology			(SWeightedDisplay<_Width, _Depth>& display, int32_t offsetY, int32_t offsetX, const SEntityGrade& entityTech)											{
		int32_t										iLine							= 0;
		if(entityTech.Tech				) iLine	+= displayFlag(display, {offsetX, offsetY+iLine}, entityTech.Tech				, MAX_ENTITY_TECHNOLOGY_COUNT, COLOR_YELLOW, COLOR_GREEN	, "- Technology: %-14.14s"	);
		if(entityTech.Grade				) iLine	+= displayFlag(display, {offsetX, offsetY+iLine}, entityTech.Grade				, 3, COLOR_YELLOW, COLOR_GREEN	, "- Grade: %-14.14s"		);
		if(entityTech.AttackType		) iLine	+= displayFlag(display, {offsetX, offsetY+iLine}, entityTech.AttackType			, MAX_ATTACK_TYPE_COUNT, COLOR_YELLOW, COLOR_RED		, "- Attack type: %-14.14s"	);
		if(entityTech.ProjectileClass	) iLine	+= displayFlag(display, {offsetX, offsetY+iLine}, entityTech.ProjectileClass	, MAX_PROJECTILE_CLASS_COUNT, COLOR_YELLOW, COLOR_RED	, "- Projectile Class: %-14.14s" );
		if(entityTech.AmmoEffect		) iLine	+= displayFlag(display, {offsetX, offsetY+iLine}, entityTech.AmmoEffect			, MAX_AMMO_EFFECT_COUNT, COLOR_YELLOW, COLOR_RED		, "- Ammo Effect: %-14.14s"	);

		return iLine;
	}

	template <size_t _Width, size_t _Depth>
	static void								displayStatusEffectsAndTechs	(SWeightedDisplay<_Width, _Depth>& display, int32_t offsetY, int32_t offsetX, CCharacter& character)													{
		int32_t										iLine							= 0;
		if(character.Flags.Tech.Gender)	
			iLine									+= displayFlag(display, {offsetX, offsetY+iLine}, character.Flags.Tech.Gender, 2	, COLOR_YELLOW, COLOR_GREEN	, "- Gender: %-13.13s");
		iLine									+= displayEntityTechnology	(display, offsetY+iLine, offsetX, character.FinalFlags.Tech		);
		iLine									+= displayEntityStatus		(display, offsetY+iLine, offsetX, character.FinalFlags.Status	);
		iLine									+= displayEntityEffect		(display, offsetY+iLine, offsetX, character.FinalFlags.Effect	);
	}


	template <size_t _Width, size_t _Depth>
	static void								displayEmptySlot				(SWeightedDisplay<_Width, _Depth>& display, int32_t offsetY, int32_t offsetX, int32_t agentIndex)																			{
		static const size_t							LINE_SIZE						= 30;
		uint16_t									color							= COLOR_GREEN;
		printfToGridColored(display.Screen, display.TextAttributes, color, offsetY, offsetX, ::klib::SCREEN_LEFT, "-- Agent #%i: %-14.14s --", agentIndex, "Open position");
		valueToGrid(display.TextAttributes, offsetY, offsetX+13, ::klib::SCREEN_LEFT, &(color = COLOR_DARKCYAN), 1, LINE_SIZE-14);
	}

	template <size_t _Width, size_t _Depth>
	static void								displayResumedAgentSlot			(SWeightedDisplay<_Width, _Depth>& display_, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character)													{
		::klib::SGrid<char, _Width, _Depth>			& display						= display_.Screen;

		static const char							formatAgentTitle	[]			= "-- Agent #%i:" " %-34.34s --"	;
		static const char							formatAgentCoins	[]			= "%-21.21s: %-11.11s"				;
		static const char							formatAgentPoints	[]			= "%-21.21s: %-10.10s"				;
		static const char							formatAgentEquip	[]			= "%-10.10s: %-36.36s Lv. %i"		;

		static const size_t							LINE_SIZE						= 56;
		uint16_t									color							= COLOR_GREEN;
		::klib::printfToGridColored(display, display_.TextAttributes, color, offsetY, offsetX, ::klib::SCREEN_LEFT,  formatAgentTitle, agentIndex, character.Name.c_str());
		::klib::valueToGrid(display_.TextAttributes, offsetY, offsetX+13, ::klib::SCREEN_LEFT, &color, 1, LINE_SIZE-14);
		offsetY									+= 2;

		std::string									equipName;	
		equipName	 = getProfessionName	(character.CurrentEquip.Profession	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Class"		, equipName.c_str(), character.CurrentEquip.Profession	.Level);
		equipName	 = getWeaponName		(character.CurrentEquip.Weapon		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Weapon"		, equipName.c_str(), character.CurrentEquip.Weapon		.Level);
		equipName	 = getArmorName			(character.CurrentEquip.Armor		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Armor"		, equipName.c_str(), character.CurrentEquip.Armor		.Level);
		equipName	 = getAccessoryName		(character.CurrentEquip.Accessory	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, "Accessory"	, equipName.c_str(), character.CurrentEquip.Accessory	.Level);
			
		::klib::lineToGridColored(display, display_.TextAttributes, COLOR_RED, ++offsetY, offsetX, ::klib::SCREEN_LEFT, "- Final Points:");
		offsetY									+=2;
		
		const SEntityPoints							& agentFinalPoints				= character.FinalPoints;
		char										formattedGauge[32];

		sprintf_s(formattedGauge, "%i/%i"	, std::max(0, character.Points.LifeCurrent.Health	), agentFinalPoints.LifeMax.Health	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i/%i"	, std::max(0, character.Points.LifeCurrent.Shield	), agentFinalPoints.LifeMax.Shield	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i/%i"	, std::max(0, character.Points.LifeCurrent.Mana		), agentFinalPoints.LifeMax.Mana	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana"	, formattedGauge);

		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Hit						); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Hit Chance"				, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Damage					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Damage"					, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Health		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Shield		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Mana			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Mana"		, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Absorption				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Absorption"				, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Range						); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Range"					, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Attack					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Attack Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Movement					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Movement Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Reflexes					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Reflexes"				, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Sight					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Sight"					, formattedGauge);

		const SEntityPoints							& agentBasePoints				= character.Points;
		sprintf_s(formattedGauge, "%i"		, agentBasePoints.Coins								); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins in wallet"		, formattedGauge);
		valueToGrid(display_.TextAttributes	, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Coins							); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins per turn"		, formattedGauge);
		valueToGrid(display_.TextAttributes	, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (agentFinalPoints.Coins >= 0) ? COLOR_ORANGE : COLOR_RED), 1, 11);
		sprintf_s(formattedGauge, "%i"		, agentBasePoints.CostMaintenance					); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Salary"				, formattedGauge);
		valueToGrid(display_.TextAttributes	, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.CostMaintenance					); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Total Cost"			, formattedGauge);
		valueToGrid(display_.TextAttributes	, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
	
	}

	template <size_t _Width, size_t _Depth>
	static void								displayDetailedAgentSlot		(SWeightedDisplay<_Width, _Depth>& display_, int32_t offsetY, int32_t offsetX, const CCharacter& character, uint16_t color=COLOR_GREEN)										{
		::klib::SGrid<char, _Width, _Depth>			& display						= display_.Screen;
		static const char							formatAgentTitle	[]			= " - %-34.34s"			;
		static const char							formatAgentEquip	[]			= "%-36.36s Lv. %i"		;
		static const char							formatAgentPoints	[]			= "%-21.21s: %-10.10s"	;
		static const char							formatAgentCoins	[]			= "%-21.21s: %-11.11s"	;

		printfToGridColored(display, display_.TextAttributes, color, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentTitle, character.Name.c_str());
		offsetY									+=1;

		std::string									equipName;	
		equipName	= getProfessionName	(character.CurrentEquip.Profession		); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Profession	.Level);
		equipName	= getWeaponName		(character.CurrentEquip.Weapon			); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Weapon		.Level);
		equipName	= getArmorName		(character.CurrentEquip.Armor			); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Armor		.Level);
		equipName	= getAccessoryName	(character.CurrentEquip.Accessory		); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentEquip, equipName.c_str(), character.CurrentEquip.Accessory	.Level);

		lineToGridColored(display, display_.TextAttributes, COLOR_RED, ++offsetY, offsetX, ::klib::SCREEN_LEFT, "- Final Points:");
		offsetY									+=2;

		const SEntityPoints							& agentFinalPoints				= character.FinalPoints;
		char										formattedGauge[32];
		sprintf_s(formattedGauge, "%i/%i"	, std::max(0, character.Points.LifeCurrent.Health	), agentFinalPoints.LifeMax.Health	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i/%i"	, std::max(0, character.Points.LifeCurrent.Shield	), agentFinalPoints.LifeMax.Shield	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i/%i"	, std::max(0, character.Points.LifeCurrent.Mana		), agentFinalPoints.LifeMax.Mana	); printfToGrid(display, offsetY++	, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana"		, formattedGauge);
		++offsetY;	//

		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Health			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Health per turn"		, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Shield			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Shield per turn"		, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.LifeCurrent.Mana				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Mana per turn"			, formattedGauge);
		++offsetY;	//																		   
																							   
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Hit					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Hit Chance"				, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Damage				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Damage"					, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Health	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Health"	, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Shield	); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Shield"	, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.DirectDamage.Mana		); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Direct Damage Mana"		, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Absorption			); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Absorption"				, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Attack.Range					); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Range"					, formattedGauge);
		++offsetY;	//																		   
																							   
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Attack				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Attack Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Movement				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Movement Speed"			, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Reflexes				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Reflexes"				, formattedGauge);
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Fitness.Sight				); printfToGrid(display, offsetY++, offsetX, ::klib::SCREEN_LEFT, formatAgentPoints, "Sight"					, formattedGauge);

		const SEntityPoints							& agentBasePoints				= character.Points;
		sprintf_s(formattedGauge, "%i"		, agentBasePoints.Coins							); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins in wallet"	, formattedGauge); valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);	//																				  
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.Coins						); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Coins per turn"	, formattedGauge); valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = (agentFinalPoints.Coins >= 0) ? COLOR_ORANGE : COLOR_RED), 1, 11);	//									  
		sprintf_s(formattedGauge, "%i"		, agentBasePoints.CostMaintenance				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Salary"			, formattedGauge); valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);	//																				  
		sprintf_s(formattedGauge, "%i"		, agentFinalPoints.CostMaintenance				); printfToGrid(display, ++offsetY, offsetX, ::klib::SCREEN_LEFT, formatAgentCoins, "- Total Cost"		, formattedGauge); valueToGrid(display_.TextAttributes, offsetY, offsetX+23, ::klib::SCREEN_LEFT, &(color = COLOR_ORANGE), 1, 11);
	}

	template <size_t _Width, size_t _Depth>
	static void								displayAgentSlot				(SWeightedDisplay<_Width, _Depth>& display_, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character, bool bShort=false, uint16_t color=COLOR_GREEN)	{
		if( bShort )
			displayResumedAgentSlot		(display_, offsetY, offsetX, agentIndex, character);
		else 
			displayDetailedAgentSlot	(display_, offsetY, offsetX, character, color);
	}

	#define MAX_ENTITY_COLUMNS	4
	template <size_t _Width, size_t _Depth> 
	static void								drawScore						(SWeightedDisplay<_Width, _Depth>& display_, int32_t offsetY, int32_t offsetX, const SCharacterScore& score)																{
		//char										buffer[128]						= {};
		char										bufferValues[64]				= {};
		const char									format[]						= "- %-21.21s: %-12.12s";
	
		sprintf_s(bufferValues, "%lli", (int64_t)score.MoneyEarned			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Money Earned"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.MoneySpent			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Money Spent"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.DamageDealt			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Damage Dealt"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.DamageTaken			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Damage Taken"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.TurnsPlayed			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Turns Played"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.BattlesWon			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Battles Won"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.BattlesLost			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Battles Lost"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.EscapesSucceeded		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Escapes Succeeded"		, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.EscapesFailed		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Escapes Failed"		, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.EnemiesKilled		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Enemies Killed"		, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksHit			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Hit"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksMissed		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Missed"		, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksReceived		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Received"		, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.AttacksAvoided		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Attacks Avoided"		, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.PotionsUsed			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Potions Used"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.GrenadesUsed			); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Grenades Used"			, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedResearch	); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Research"	, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedProduction	); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Productions"	, bufferValues);
		sprintf_s(bufferValues, "%lli", (int64_t)score.CompletedUpgrade		); printfToGrid(display_.Screen, offsetY++, offsetX, ::klib::SCREEN_LEFT, format, "Completed Upgrades"	, bufferValues);
	}

	#define MAX_AGENT_ROWS		2
	#define MAX_AGENT_COLUMNS	3

	void									drawSquadSlots					(SGame& instanceGame);
}

#endif // __AGENT_HELPER_H__91238648097234698723649872364923874__
