/// Copyright 2016-2017 - asm128
#include "gpk_coord.h"

#ifndef KLIB_MISC_H_982143092634987160987328097642398
#define KLIB_MISC_H_982143092634987160987328097642398

namespace klib
{
#pragma pack(push, 1)
	enum AXIS_3D : uint8_t
		{	XPOSITIVE				= 0x1
		,	XNEGATIVE				= 0x2
		,	YPOSITIVE				= 0x4
		,	YNEGATIVE				= 0x8
		,	ZPOSITIVE				= 0x10
		,	ZNEGATIVE				= 0x20
		};

	enum ALIGN_SCREEN : uint8_t
		{	SCREEN_RIGHT			= ((uint8_t)XPOSITIVE)
		,	SCREEN_LEFT				= ((uint8_t)XNEGATIVE)
		,	SCREEN_BOTTOM			= ((uint8_t)YPOSITIVE)
		,	SCREEN_TOP				= ((uint8_t)YNEGATIVE)
		,	SCREEN_FAR				= ((uint8_t)ZPOSITIVE)
		,	SCREEN_NEAR				= ((uint8_t)ZNEGATIVE)
		,	SCREEN_CENTER			= SCREEN_LEFT		| SCREEN_RIGHT
		,	SCREEN_VCENTER			= SCREEN_TOP		| SCREEN_BOTTOM
		,	SCREEN_ZCENTER			= SCREEN_FAR		| SCREEN_NEAR
		,	SCREEN_TOP_LEFT			= SCREEN_TOP		| SCREEN_LEFT  
		,	SCREEN_TOP_RIGHT		= SCREEN_TOP		| SCREEN_RIGHT 
		,	SCREEN_BOTTOM_LEFT		= SCREEN_BOTTOM		| SCREEN_LEFT  
		,	SCREEN_BOTTOM_RIGHT		= SCREEN_BOTTOM		| SCREEN_RIGHT 
		,	SCREEN_CENTER_TOP		= SCREEN_CENTER		| SCREEN_TOP	
		,	SCREEN_CENTER_BOTTOM	= SCREEN_CENTER		| SCREEN_BOTTOM	
		,	SCREEN_VCENTER_LEFT		= SCREEN_VCENTER	| SCREEN_LEFT		
		,	SCREEN_VCENTER_RIGHT	= SCREEN_VCENTER	| SCREEN_RIGHT	
		};

	// This function copies an amount of _tCell values into a rectangle of _tCells 
	// with rectangleTopLeft specifying the origin of the rectangle 
	// and width and height specifying the sizes from there (with Y axis being up to down most likely)
	template<typename _tCell>	int32_t		valueToRect				( _tCell* rectangleTopLeft, uint32_t width, uint32_t height, int32_t offsetLine, int32_t offsetColumn, ::klib::ALIGN_SCREEN align, const _tCell* values, int32_t valueCount, int32_t repeat=0 )		{
		if(align & SCREEN_RIGHT) {	// testing for RIGHT bit would tell us that this might be aligned to either RIGHT or CENTER.
			int32_t										offsetX					= (int32_t) ((align == SCREEN_CENTER) ? (width>>1)-(valueCount>>1)+offsetColumn : (int32_t)(width)-(valueCount)-offsetColumn); 
			return valueToRect(rectangleTopLeft, width, height, offsetLine, offsetX, SCREEN_LEFT, values, valueCount, repeat);
		}
		else if(offsetLine < (int32_t)height) {
			int32_t										linearCellOffset		= (int32_t)(offsetLine * width + offsetColumn);
			int32_t										remainingColumns		= (int32_t)width - offsetColumn;
			if(remainingColumns <= 0)
				return offsetColumn;
			const int32_t								maxPrintSize			= (valueCount < remainingColumns) ? valueCount : remainingColumns;
			memcpy(&rectangleTopLeft[linearCellOffset], values, maxPrintSize * sizeof(_tCell));
			if(repeat > 0)
				valueToRect( rectangleTopLeft, width, height, offsetLine, offsetColumn+valueCount, align, values, valueCount, --repeat );
		}

		return offsetColumn;
	}

#pragma pack(pop)
}

#endif // KLIB_MISC_H_982143092634987160987328097642398
