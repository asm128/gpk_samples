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
	template <typename _BitfieldType>
	int32_t									displayFlag						(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, ::gpk::SCoord2<int32_t> offset, _BitfieldType characterBits, uint32_t statusCount, uint16_t colorTitle, uint16_t colorField, const std::string& textFormat)	{
		uint16_t									color							= COLOR_GREEN;
		int32_t										iLine							= 0;
		for(uint32_t iStatus=0; iStatus<statusCount; ++iStatus) {
			_BitfieldType								statusBit						= (_BitfieldType)(1 << iStatus);
			if(0 == (statusBit & characterBits))
				continue;

			::gpk::label								valueLabel						= ::gpk::get_value_label(statusBit);
			int32_t										actualX							= printfToGridColored(display, textAttributes, (color = colorTitle), offset.y+iLine, offset.x, ::klib::SCREEN_LEFT, textFormat.c_str(), valueLabel.begin());
			valueToGrid(textAttributes, offset.y+iLine, (int32_t)(actualX+textFormat.size()-8), ::klib::SCREEN_LEFT, &(color = colorField), 1, (int32_t)(textFormat.size()+valueLabel.size()-8));
			++iLine;
		}

		return iLine;
	}


	static constexpr	const uint32_t		MAX_AGENT_ROWS					= 2;
	static constexpr	const uint32_t		MAX_AGENT_COLUMNS				= 3;
	void									displayStatusEffectsAndTechs	(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, CCharacter& character);
	void									displayDetailedAgentSlot		(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, const CCharacter& character, uint16_t color=COLOR_GREEN);
	void									displayAgentSlot				(::gpk::view_grid<char> display, ::gpk::view_grid<uint16_t> textAttributes, int32_t offsetY, int32_t offsetX, int32_t agentIndex, CCharacter& character, bool bShort=false, uint16_t color=COLOR_GREEN);
	void									drawSquadSlots					(::klib::SGame& instanceGame);
}

#endif // __AGENT_HELPER_H__91238648097234698723649872364923874__
