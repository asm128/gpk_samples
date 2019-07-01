//#define NOMINMAX

#include "Game.h"

#include "gpk_noise.h"

#include "gpk_view_array.h"

#include <algorithm>

#ifndef __DRAW_H_2394623987462983746823749623__
#define __DRAW_H_2394623987462983746823749623__

namespace klib
{

	template<size_t _Width, size_t _Height>
	void													drawSnowBackground							( SWeightedDisplay<_Width, _Height>& display, double lastTimeSeconds, uint32_t disturbance = 1 )																							{
		int32_t														displayWidth								= (int32_t)display.Width;
		//int32_t														displayDepth								= (int32_t)display.Depth;

		for(int32_t x=0; x<displayWidth; ++x) 
			if(display.DisplayWeights[0][x] == 0)  {
				if( 0 == (rand()%200) && x % 2) {
					display.Screen			.Cells[0][x]					= (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000+x), disturbance) > 0.0) ? '.' : (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000-x*x), disturbance) > 0.0) ? 15 : ',';	
					display.DisplayWeights	.Cells[0][x]					= .00001f;
					display.Speed			.Cells[0][x]					= rand()*.001f;
					display.SpeedTarget		.Cells[0][x]					= rand()*.001f;
					display.TextAttributes	.Cells[0][x]					= (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000-x), disturbance) > 0.0) ? COLOR_CYAN:COLOR_WHITE;
				}
			}
	
		for(uint32_t z=0; z<display.Depth-2; ++z) 
			for(uint32_t x=0; x<display.Width; ++x) {
				if(display.DisplayWeights.Cells[z][x] == 0)
					continue;

				display.DisplayWeights.Cells[z][x]						+= (float)(lastTimeSeconds*display.Speed.Cells[z][x]);

				if(display.Speed.Cells[z][x] < display.SpeedTarget.Cells[z][x])
					display.Speed.Cells	[z][x]								+= (float)((display.Speed.Cells[z][x]*lastTimeSeconds*lastTimeSeconds));//*.1f;
				else
					display.Speed.Cells	[z][x]								-= (float)((display.Speed.Cells[z][x]*lastTimeSeconds*lastTimeSeconds));//*.1f;
			}

		for(uint32_t z=0; z<display.Depth-2; ++z) 
			for(uint32_t x=0; x<display.Width; ++x) {
				if(display.DisplayWeights.Cells[z][x] == 0)
					continue;

				if(display.DisplayWeights.Cells[z][x] > 1.0) {
					int															randX										= (rand()%2) ? rand()%(1+disturbance*2)-disturbance : 0;
					int32_t														xpos										= ::gpk::max(::gpk::min((int)x+randX, displayWidth-1), 0);
					display.Screen			.Cells[z+1][xpos]				= display.Screen.Cells			[z][x];
					display.Speed			.Cells[z+1][xpos]				= display.Speed.Cells			[z][x];
					display.TextAttributes	.Cells[z+1][xpos]				= (::gpk::noise1D((uint32_t)(lastTimeSeconds*10000+x), disturbance) > 0.0) ? COLOR_CYAN:COLOR_WHITE;
					//display.TextAttributes	.Cells[z+1][xpos]			= display.TextAttributes.Cells	[z][x];
					display.DisplayWeights	.Cells[z+1][xpos]				= 0.0001f;
					display.SpeedTarget		.Cells[z+1][xpos]				= (float)((rand()%5000))*0.001f+0.001f;

					display.Screen			.Cells[z][x]					= ' ';
					display.DisplayWeights	.Cells[z][x]					= 0;
					display.Speed			.Cells[z][x]					= 0; 
					display.SpeedTarget		.Cells[z][x]					= 0;
					display.TextAttributes	.Cells[z][x]					= COLOR_WHITE;
				}
			}

	}

	template<size_t _Width, size_t _Height>
	void													drawFireBackground							( SWeightedDisplay<_Width, _Height>& display, double lastTimeSeconds, uint32_t disturbance = 2, uint32_t disappearChanceDivisor=10, bool bReverse=false, bool bDontSlowdown=true )			{
		int32_t														displayWidth								= (int32_t)display.Width;
		int32_t														displayDepth								= (int32_t)display.Depth;

		uint32_t													firstRow									= bReverse ? 0 : displayDepth - 1;
		uint32_t													lastRow										= bReverse ? displayDepth - 1 : 0;
		uint64_t													seed										= (uint64_t)(disturbance+lastTimeSeconds*100000*(1+(rand()%100)));
		uint32_t													randBase									= (uint32_t)(lastTimeSeconds*(disturbance+654)*100000			);
		for(int32_t x=0; x<displayWidth; ++x) 
			if(display.DisplayWeights.Cells[firstRow][x] == 0) {
				if( 0 == (rand()%4) ) {
					display.Screen			.Cells[firstRow][x]				=  (::gpk::noise1D(randBase+x, seed+1203) > 0.0) ? '.' :  (::gpk::noise1D(randBase+1+x*x, seed+1235) > 0.0) ? '|' : ',';
					display.DisplayWeights	.Cells[firstRow][x]				= .00001f;
					display.Speed			.Cells[firstRow][x]				= rand()*.001f+0.001f;
					display.SpeedTarget		.Cells[firstRow][x]				= rand()*.0009f+0.001f;
					display.TextAttributes	.Cells[firstRow][x]				= bReverse ? ((::gpk::noise1D(randBase+321+x, seed+91423) > 0.0)?COLOR_CYAN:COLOR_BLUE) :  (::gpk::noise1D(randBase+32+x, seed<<1) > 0.0) ? COLOR_RED : (::gpk::noise1D(randBase+987429654+x, seed+98234) > 0.0) ? COLOR_ORANGE : COLOR_DARKYELLOW;
				}
			}

		for(uint32_t z = 0, maxZ = display.Depth; z < maxZ; z ++) 
			for(uint32_t x=0; x<display.Width; ++x) {
				if(lastRow == z) {
					display.Screen			.Cells[lastRow][x]				= ' ';
					display.DisplayWeights	.Cells[lastRow][x]				= 0;
					display.Speed			.Cells[lastRow][x]				= 0; 
					display.SpeedTarget		.Cells[lastRow][x]				= 0;
				}
				if(display.Screen.Cells[z][x] == ' ')
					continue;

				display.DisplayWeights.Cells[z][x]						+= (float)(lastTimeSeconds*display.Speed.Cells[z][x]);

				if(display.Speed.Cells[z][x] < display.SpeedTarget.Cells[z][x])
					display.Speed.Cells[z][x] += (float)(display.Speed.Cells[z][x]*lastTimeSeconds);
				else
					display.Speed.Cells[z][x] -= (float)(display.Speed.Cells[z][x]*lastTimeSeconds);
			}

		for(uint32_t z = 0, maxZ = display.Depth; z < maxZ; z ++) 
			for(uint32_t x=0; x<display.Width; ++x) {
				if(display.Screen.Cells[z][x] == ' ')
					continue;
			
				if(display.DisplayWeights.Cells[z][x] > 1.0) {
					int randX = ((::gpk::noise1D(randBase+x+z*display.Width), seed+544) > 0.0) ? rand()%(1+disturbance*2)-disturbance : 0;
					int32_t xpos = ::gpk::min(::gpk::max(0, (int)x+randX), displayWidth-1);
					int32_t zpos = bReverse ? z+1 : z-1;
	
					if((rand()%disappearChanceDivisor) == 0) {
						display.Screen			.Cells[zpos][xpos] = ' ';
						display.DisplayWeights	.Cells[zpos][xpos] = 0;
					}
					else { 
						if(('|' == display.Screen	.Cells[z][x]) && z < (display.Depth/5*4)) {
							display.Screen			.Cells[zpos][xpos] = '.';
							display.TextAttributes	.Cells[zpos][xpos] = ((bReverse) || (::gpk::noiseNormal1D(x, seed<<2) < 0.0)) ? COLOR_DARKGREY : COLOR_YELLOW; 
						}
						else if( bReverse && z > (display.Depth/5)) {
							display.Screen			.Cells[zpos][xpos] = '|';
							display.TextAttributes	.Cells[zpos][xpos] = COLOR_CYAN;
						}
						else {
							display.Screen			.Cells[zpos][xpos]	= display.Screen.Cells[z][x];
							display.TextAttributes	.Cells[zpos][xpos]	= display.TextAttributes.Cells[z][x];
						}

						display.DisplayWeights	.Cells[zpos][xpos]	= 0.00001f;
						display.Speed			.Cells[zpos][xpos]	= display.Speed.Cells[z][x];
						display.SpeedTarget		.Cells[zpos][xpos]	= (float)::gpk::noiseNormal1D(x, seed)*50.0f;
						if(bDontSlowdown)
							display.SpeedTarget	.Cells[zpos][xpos]	*= ((bReverse ? display.Depth-z : z )*2/(float)display.Depth);
						display.SpeedTarget		.Cells[zpos][xpos]	+= 0.001f;
					}

					display.Screen				.Cells[z][x]	= ' ';
					display.DisplayWeights		.Cells[z][x]	= 0;
					display.Speed				.Cells[z][x]	= 0; 
					display.SpeedTarget			.Cells[z][x]	= 0;
					display.TextAttributes		.Cells[z][x]	= COLOR_WHITE;
				}
			}
	}

	template<size_t _Width, size_t _Height>
	void drawBubblesBackground( SWeightedDisplay<_Width, _Height>& display, double lastTimeSeconds, uint32_t disturbance=2 ) {
		int32_t displayWidth	= (int32_t)display.Width;
		int32_t displayDepth	= (int32_t)display.Depth;

		uint64_t seed		= (uint64_t)(disturbance+lastTimeSeconds*100000*(1+(rand()%100)));
		uint32_t randBase	= (uint32_t)(lastTimeSeconds*(disturbance+654)*100000			);
		for(int32_t x=0; x<displayWidth; ++x) 
			//if(display.DisplayWeights[displayDepth-1][x] == 0) 
			if(	display.Screen.Cells[displayDepth-1][x] != '0' &&
				display.Screen.Cells[displayDepth-1][x] != 'o' &&
				display.Screen.Cells[displayDepth-1][x] != '.' &&
				display.Screen.Cells[displayDepth-1][x] != 'O'
			) 
			{
				if( rand()%2 ) {
					display.Screen			.Cells[displayDepth-1][x] = (::gpk::noise1D(randBase+x, seed+1203) > 0.0) ? 'o' : (::gpk::noise1D(randBase+561+x, seed+2135) > 0.0) ? '0' : (::gpk::noise1D(randBase+x+6, seed+103) > 0.0) ? '.' : 'O';
					display.DisplayWeights	.Cells[displayDepth-1][x] = .000001f;
					display.Speed			.Cells[displayDepth-1][x] = rand()*.001f+0.001f;
					display.SpeedTarget		.Cells[displayDepth-1][x] = rand()*.0025f+0.001f;
					display.TextAttributes	.Cells[displayDepth-1][x] = (rand() % 2)?COLOR_GREEN:COLOR_DARKGREEN;
				}
			}
	
		for(uint32_t z=1; z<display.Depth; ++z) 
			for(uint32_t x=0; x<display.Width; ++x) {
				if(display.Screen.Cells[z][x] == ' ')
					continue;

				display.DisplayWeights.Cells[z][x] += (float)(lastTimeSeconds*display.Speed.Cells[z][x]);

				if(display.Speed.Cells[z][x] < display.SpeedTarget.Cells[z][x])
					display.Speed.Cells	[z][x] += (float)((display.Speed.Cells[z][x]*lastTimeSeconds));	
				else																					
					display.Speed.Cells	[z][x] -= (float)((display.Speed.Cells[z][x]*lastTimeSeconds));	

				display.Speed.Cells[z][x] *= .999f;
			}

		for(uint32_t z=1; z<display.Depth; ++z) 
			for(uint32_t x=0; x<display.Width; ++x) {
				if(display.Screen.Cells[z][x] == ' ')
					continue;

				if(display.DisplayWeights.Cells[z][x] > 1.0) {
					int randX = (rand()%2) ? rand()%(1+disturbance*2)-disturbance : 0;
					if(1 == z) {
						display.Screen			.Cells[0][x]	= ' ';
						display.DisplayWeights	.Cells[0][x]	= 0;
						display.Speed			.Cells[0][x]	= 0; 
						display.SpeedTarget		.Cells[0][x]	= 0;
						display.TextAttributes	.Cells[0][x]	= COLOR_WHITE;
					}
					else {
						int32_t xpos = ::gpk::max(::gpk::min((int)x+randX, displayWidth-1), 0);
					
						if((rand()%10) == 0)  {
							display.Screen.Cells[z-1][xpos]			= ' ';
							display.DisplayWeights.Cells[z-1][xpos]	= 0;
						}
						else {
								 if( '0' == display.Screen.Cells[z][x] && z < (display.Depth/5*4))
									display.Screen.Cells[z-1][xpos] = 'O';
							else if( 'O' == display.Screen.Cells[z][x] && z < (display.Depth/3*2))
									display.Screen.Cells[z-1][xpos] = (::gpk::noise1D(randBase+x, seed+12345) > 0.0) ? 'o' : '\'';
							else if( 'o' == display.Screen.Cells[z][x] && z < (display.Depth>>1))
									display.Screen.Cells[z-1][xpos] = '.';
							else
									display.Screen.Cells			[z-1][xpos]	= display.Screen.Cells[z][x];

							display.TextAttributes	.Cells[z-1][xpos]	= (::gpk::noise1D(randBase+x+x, seed+41203) > 0.0) ? COLOR_DARKGREEN : COLOR_GREEN;
							display.DisplayWeights	.Cells[z-1][xpos]	= 0.00001f;
							display.Speed			.Cells[z-1][xpos]	= display.Speed.Cells[z][x];
							display.SpeedTarget		.Cells[z-1][xpos]	= (float)::gpk::noiseNormal1D(x, seed) * 20.0f * (z*1.0f/display.Depth)+0.001f;
						}
					}

					display.Screen			.Cells[z][x]		= ' ';
					display.DisplayWeights	.Cells[z][x]		= 0;
					display.Speed			.Cells[z][x]		= 0; 
					display.SpeedTarget		.Cells[z][x]		= 0;
					display.TextAttributes	.Cells[z][x]		= COLOR_WHITE;
				}
			}
	}

	template<size_t _Width, size_t _Height>
	void drawRainBackground( SWeightedDisplay<_Width, _Height>& display, double lastTimeSeconds ) {
		for(uint32_t i=0; i<_Width; ++i)
			if(rand()%2)
				display.Screen.Cells[_Height-1][i] = (rand()%2) ? '.' : '|';
		return drawFireBackground( display, lastTimeSeconds*1.5, 0, 20, true, false );
	}

	template<typename _TCell, size_t _LineCount>
	SGameState drawCredits(_TCell* display, uint32_t width, uint32_t depth, double lastFrameTime, const ::gpk::view_const_string (&namesCredits)[_LineCount], const SGameState& returnValue) {
		static double	offset			= (double)depth;
		int32_t			curLine			= (int32_t)offset;
		static int32_t	maxDifference	= curLine;
		int32_t			curDifference	= curLine;
		double			bbHeight		= (double)depth;

		for(uint32_t i=0, count = (uint32_t)::gpk::size(namesCredits); i < count && curLine < bbHeight; ++i)
			if((curLine+=2) >= 0)
				printfToRect((char_t*)display, width, depth, curLine, 0, ::klib::SCREEN_CENTER, "%s", namesCredits[i].begin());

		maxDifference = std::max(curLine - curDifference, maxDifference);

		offset -= lastFrameTime*6.0;

		if( offset <= -maxDifference )
			offset += depth+maxDifference;

		return returnValue;
	}

	template<typename _TCell, size_t _Width, size_t _Depth, size_t _LineCount>
	SGameState drawCredits(::klib::SGrid<_TCell, _Width, _Depth>& display, double lastFrameTime, const ::gpk::view_const_string (&namesCredits)[_LineCount], const SGameState& returnValue) {
		return ::klib::drawCredits(&display.Cells[0][0], (uint32_t)_Width, (uint32_t)_Depth, lastFrameTime, namesCredits, returnValue);
	}

	template<typename _TCell>
	SGameState drawMemorial(_TCell* display, uint32_t width, uint32_t depth, uint16_t* textAttributes, double lastFrameTime, const ::gpk::array_obj<CDeadCharacter>& namesMemorial, const SGameState& returnValue) {
		static double	offset			= (double)depth;
		int32_t			curLine			= (int32_t)offset;
		static int32_t	maxDifference	= curLine;
		const int32_t	curDifference	= curLine;
		const double	bbHeight		= (double)depth;

		for(uint32_t i=0, memorialLines = namesMemorial.size(); i < memorialLines && curLine < bbHeight; ++i) {
			static const char format1[]		= 
				"Damage Dealt        : %-8.8s - "
				"Damage Taken        : %-8.8s - "
				"Turns Played        : %-6.6s - "
				"Battles Won         : %-6.6s - "
				"Battles Lost        : %-6.6s - "
				"Escapes Succeeded   : %-6.6s"
				//"Escapes Failed      : %-6.6s - "
				;

			static const char format2[]		= 
				"Enemies Killed      : %-8.8s - "
				"Attacks Hit         : %-8.8s - "
				"Attacks Missed      : %-6.6s - "
				"Attacks Received    : %-6.6s - "
				"Attacks Avoided     : %-6.6s - "
				"Potions Used        : %-6.6s - "
				"Grenades Used       : %-6.6s"
				;

			static const char format3[]		= 
				"Money Earned        : %-8.8s - "
				"Money Spent         : %-8.8s"
				;
			char bufferMoneyEarned		[32]	= {};
			char bufferMoneySpent		[32]	= {};
			char bufferDamageDealt		[32]	= {};
			char bufferDamageTaken		[32]	= {};
			char bufferTurnsPlayed		[32]	= {};
			char bufferBattlesWon		[32]	= {};
			char bufferBattlesLost		[32]	= {};
			char bufferEscapesSucceeded	[32]	= {};
			//char bufferEscapesFailed	[32]	= {};
			char bufferEnemiesKilled	[32]	= {};
			char bufferAttacksHit		[32]	= {};
			char bufferAttacksMissed	[32]	= {};
			char bufferAttacksReceived	[32]	= {};
			char bufferAttacksAvoided	[32]	= {};
			char bufferPotionsUsed		[32]	= {};
			char bufferGrenadesUsed		[32]	= {};

			const CDeadCharacter					& deadCharacter									= namesMemorial[i];
			sprintf_s(bufferMoneyEarned			, "%lli", (int64_t)deadCharacter.Score.MoneyEarned		);
			sprintf_s(bufferMoneySpent			, "%lli", (int64_t)deadCharacter.Score.MoneySpent		);
			sprintf_s(bufferDamageDealt			, "%lli", (int64_t)deadCharacter.Score.DamageDealt		);
			sprintf_s(bufferDamageTaken			, "%lli", (int64_t)deadCharacter.Score.DamageTaken		);
			sprintf_s(bufferTurnsPlayed			, "%lli", (int64_t)deadCharacter.Score.TurnsPlayed		);
			sprintf_s(bufferBattlesWon			, "%lli", (int64_t)deadCharacter.Score.BattlesWon		);
			sprintf_s(bufferBattlesLost			, "%lli", (int64_t)deadCharacter.Score.BattlesLost		);
			sprintf_s(bufferEscapesSucceeded	, "%lli", (int64_t)deadCharacter.Score.EscapesSucceeded	);
			//sprintf_s(bufferEscapesFailed		, "%lli", (int64_t)deadCharacter.Score.EscapesFailed	);
			sprintf_s(bufferEnemiesKilled		, "%lli", (int64_t)deadCharacter.Score.EnemiesKilled	);
			sprintf_s(bufferAttacksHit			, "%lli", (int64_t)deadCharacter.Score.AttacksHit		);
			sprintf_s(bufferAttacksMissed		, "%lli", (int64_t)deadCharacter.Score.AttacksMissed	);
			sprintf_s(bufferAttacksReceived		, "%lli", (int64_t)deadCharacter.Score.AttacksReceived	);
			sprintf_s(bufferAttacksAvoided		, "%lli", (int64_t)deadCharacter.Score.AttacksAvoided	);
			sprintf_s(bufferPotionsUsed			, "%lli", (int64_t)deadCharacter.Score.PotionsUsed		);
			sprintf_s(bufferGrenadesUsed		, "%lli", (int64_t)deadCharacter.Score.GrenadesUsed		);

			int32_t messageColor = COLOR_GREEN;
			int32_t offsetX = 4;
			if((curLine+=2) >= 0 && (curLine < bbHeight))
				::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, messageColor, curLine, 0, ::klib::SCREEN_CENTER, "-- %s --", deadCharacter.Name.begin());

			messageColor = COLOR_DARKGREY;
			if((curLine+=2) >= 0 && curLine < bbHeight)
				offsetX = printfToRectColored((char_t*)display, width, depth, textAttributes, messageColor, curLine, 3, ::klib::SCREEN_LEFT, format1
					, bufferDamageDealt			
					, bufferDamageTaken			
					, bufferTurnsPlayed			
					, bufferBattlesWon			
					, bufferBattlesLost			
					, bufferEscapesSucceeded	
					//, bufferEscapesFailed		
					);
			if((curLine+=1) >= 0 && curLine < bbHeight)
				::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format2
					, bufferEnemiesKilled		
					, bufferAttacksHit			
					, bufferAttacksMissed		
					, bufferAttacksReceived		
					, bufferAttacksAvoided		
					, bufferPotionsUsed			
					, bufferGrenadesUsed		
					);
			if((curLine+=1) >= 0 && curLine < bbHeight)
				::klib::printfToRectColored((char_t*)display, width, depth, textAttributes, messageColor, curLine, offsetX, ::klib::SCREEN_LEFT, format3
					, bufferMoneyEarned			
					, bufferMoneySpent			
					);
		}

		maxDifference	= ::gpk::max(curLine - curDifference, maxDifference);
		offset			-= lastFrameTime * 6.0;

		if( offset <= -maxDifference )
			offset			+= depth + maxDifference;

		return returnValue;
	}

	void drawAndPresentGame( SGame& instanceGame, ::klib::SASCIITarget& target );

} // namespace

#define TACTICAL_DISPLAY_POSY 6

#endif // __DRAW_H_2394623987462983746823749623__
