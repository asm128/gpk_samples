/// Copyright 2014-2017 - asm128
#include "gpk_eval.h"
#include "gpk_coord.h"
#include "klib_draw_misc.h"

#ifndef KLIB_FILL_H_9283492364
#define KLIB_FILL_H_9283492364

namespace klib
{
	template<typename _tValue, size_t _arraySize>	
	static inline								_tValue*			fillArray				(_tValue (&targets)[_arraySize], const _tValue& value)																													{
		for( uint32_t i=0; i < _arraySize; i++ ) 
			targets[i]														= value; 

		return targets;	
	}
	template<typename _tValue>						
	static inline								_tValue*			fillArray				(_tValue* targets, const _tValue& value, uint32_t count, uint32_t maxCount)																								{
		gerror_if(count > maxCount, "%s", "Attempt to assign a count higher than the allowed size.");
		for( uint32_t i=0, actualCount = (count > maxCount) ? maxCount : count; i < actualCount; ++i ) 
			targets[i]														= value;	

		return targets; 
	}
	template<typename _tValue, size_t _arraySize>	
	static inline								_tValue*			fillArray				(_tValue (&targets)[_arraySize], const _tValue& value, uint32_t count)																									{
		gerror_if(count > _arraySize, "%s", "Attempt to assign a count higher than the allowed size.");
		for( uint32_t i=0, actualCount = (count > _arraySize) ? _arraySize : count; i < actualCount; ++i ) 
			targets[i]														= value;	

		return targets; 
	}
	template<typename _tValue, size_t _arrayWidth, size_t _arrayHeight>	
	static inline								_tValue*			fillArray2D				(_tValue (&targets)[_arrayHeight][_arrayWidth], const _tValue& value)																									{
		for( uint32_t z=0; z < _arrayHeight; z++	)											
			for( uint32_t x=0; x < _arrayWidth; x++	)											
				targets[z][x]													= value;																

		return &targets[0][0];																		
	}
	template <typename _tCell>					void				fillRectangleBorders	(_tCell* target, int32_t targetWidth, int32_t targetHeight, const _tCell& color, const ::gpk::SRectangle2D<int32_t>& rect)												{
		const int32_t														ystart					= ::gpk::max(0, ::gpk::min(targetHeight	, rect.Offset.y					));
		const int32_t														xstart					= ::gpk::max(0, ::gpk::min(targetWidth	, rect.Offset.x					));
		const int32_t														ystop					= ::gpk::max(0, ::gpk::min(targetHeight	, rect.Offset.y + rect.Size.y	));
		const int32_t														xstop					= ::gpk::max(0, ::gpk::min(targetWidth	, rect.Offset.x + rect.Size.x	));
		const int32_t														ylastIndex				= rect.Offset.y + rect.Size.y - 1;
		const int32_t														xlastIndex				= rect.Offset.x + rect.Size.x - 1;
		if(::gpk::in_range(rect.Offset.x	, 0, targetWidth	)) { for(int32_t y = rect.Offset.y, ycount = rect.Offset.y + rect.Size.y; y < ycount; ++y) { if(y >= ystop) break; else if(y >= ystart) { const int32_t linearIndex = y * targetWidth + rect.Offset.x	; target[linearIndex] = color; } } }
		if(::gpk::in_range(rect.Offset.y	, 0, targetHeight	)) { for(int32_t x = rect.Offset.x, xcount = rect.Offset.x + rect.Size.x; x < xcount; ++x) { if(x >= xstop) break; else if(x >= xstart) { const int32_t linearIndex = rect.Offset.y * targetWidth + x	; target[linearIndex] = color; } } }
		if(::gpk::in_range(ylastIndex		, 0, targetHeight	)) { for(int32_t x = rect.Offset.x, xcount = rect.Offset.x + rect.Size.x; x < xcount; ++x) { if(x >= xstop) break; else if(x >= xstart) { const int32_t linearIndex = ylastIndex * targetWidth + x		; target[linearIndex] = color; } } }
		if(::gpk::in_range(xlastIndex		, 0, targetWidth	)) { for(int32_t y = rect.Offset.y, ycount = rect.Offset.y + rect.Size.y; y < ycount; ++y) { if(y >= ystop) break; else if(y >= ystart) { const int32_t linearIndex = y * targetWidth + xlastIndex		; target[linearIndex] = color; } } }
	}
	template <typename _tCell>					void				fillCircle				(_tCell* target, int32_t targetWidth, int32_t targetHeight, const _tCell& color, const ::gpk::SCircle2D<int32_t>& circle)												{
		const int32_t														ystart					= (int32_t)::gpk::max(0.0, ::gpk::min((double)targetHeight	, circle.Center.y - circle.Radius));
		const int32_t														xstart					= (int32_t)::gpk::max(0.0, ::gpk::min((double)targetWidth	, circle.Center.x - circle.Radius));
		const int32_t														ystop					= (int32_t)::gpk::max(0.0, ::gpk::min((double)targetHeight	, circle.Center.y + circle.Radius));
		const int32_t														xstop					= (int32_t)::gpk::max(0.0, ::gpk::min((double)targetWidth	, circle.Center.x + circle.Radius));

		const double														squaredRadius			= circle.Radius*circle.Radius;
		for(int32_t y=ystart; y<ystop; ++y)
			for(int32_t x=xstart; x<xstop; ++x) {							
				const ::gpk::SCoord2<int32_t>										cellPos					= {x, y};
				const double														distanceFromCenter		= (circle.Center - cellPos).LengthSquared();
				if(distanceFromCenter > squaredRadius)
					continue;

				const int32_t														linearIndex				= y * targetWidth + x;
				target[linearIndex]												= color;
			}
	}
	template <typename _tCell, typename _tCoord>	void			fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, int32_t targetPitch, const _tCell& color, const ::gpk::SRectangle2D<_tCoord>& rect)						{
		const int32_t														ystart					= ::gpk::max(0, ::gpk::min(targetHeight	, rect.Offset.y				));
		const int32_t														xstart					= ::gpk::max(0, ::gpk::min(targetWidth	, rect.Offset.x				));
		const int32_t														ystop					= ::gpk::max(0, ::gpk::min(targetHeight	, rect.Offset.y+rect.Size.y	));
		const int32_t														xstop					= ::gpk::max(0, ::gpk::min(targetWidth	, rect.Offset.x+rect.Size.x	));

		for(int32_t y = ystart; y < ystop; ++y)
			for(int32_t x = xstart; x < xstop; ++x) {							
				const int32_t														linearIndex				= y*targetPitch+x;
				target[linearIndex]												= color;
			}
	}
	template <typename _tCell, typename _tCoord>	
	static inline								void				fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, int32_t targetPitch, _tCell color, const ::gpk::SRectangle2D<_tCoord>& rect, ::klib::ALIGN_SCREEN align)	{
		if(align == (::klib::SCREEN_TOP_LEFT))
			::klib::fillRectangle(target, targetWidth, targetHeight, targetPitch, color, rect);
		else {
			::gpk::SRectangle2D<int32_t>										rectAligned				= rect;
			::klib::realignRectangle(targetWidth, targetHeight, rect, rectAligned, align);
			::klib::fillRectangle(target, targetWidth, targetHeight, targetPitch, color, rectAligned);
		}
	}
	template <typename _tCell, typename _tCoord>	static inline	void		fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, _tCell color, const ::gpk::SRectangle2D<_tCoord>& rect)								{ ::klib::fillRectangle(target, targetWidth, targetHeight, targetWidth, color, rect);			}
	template <typename _tCell, typename _tCoord>	static inline	void		fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, _tCell color, const ::gpk::SRectangle2D<_tCoord>& rect, ::klib::ALIGN_SCREEN align)	{ ::klib::fillRectangle(target, targetWidth, targetHeight, targetWidth, color, rect, align);	}
}

#endif // KLIB_FILL_H_9283492364
