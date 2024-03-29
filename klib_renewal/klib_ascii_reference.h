﻿/// Copyright 2016-2017 - asm128
#ifndef KLIB_ASCII_REFERENCE_H_23627
#define KLIB_ASCII_REFERENCE_H_23627

namespace klib
{
#pragma pack(push, 1)
	static const unsigned char			ascii_wall_double				[] = 
		{	' '				// empty				0
		,	0xBA			// vertical				1	║
		,	0xB9			// vertical left		2	╣
		,	0xBB			// down left			3	╗
		,	0xC8			// up right				4	╚
		,	0xCA			// horizontal up		5	╩
		,	0xCB			// horizontal down		6	╦
		,	0xCC			// vertical right		7	╠
		,	0xCD			// horizontal			8	═
		,	0xCE			// cross				9	╬
		,	0xBC			// up left				10	╝
		,	0xC9			// down right			11	╔
		}; //" ╝╗╚╔║═╠╣╦╩╬";	

	static const unsigned char			ascii_wall_single				[] = 
		{	' '				// empty				0	
		,	0xB3			// vertical				1	│
		,	0xB4			// vertical left		2	┤
		,	0xBF			// down left			3	┐
		,	0xC0			// up right				4	└
		,	0xC1			// horizontal up		5	┴
		,	0xC2			// horizontal down		6	┬
		,	0xC3			// vertical right		7	├
		,	0xC4			// horizontal			8	─
		,	0xC5			// cross				9	┼
		,	0xD9			// up left				10	┘
		,	0xDA			// down right			11	┌
		}; //" ┘┐└┌│─├┤┬┴┼";	

	static const unsigned char			ascii_wall_combined_single		[] = 
		{	' '				// empty				0
		,	0xB4			// vertical left	╡	1
		,	0xBF			// down left		╕	2
		,	0xC0			// up right			╘	3
		,	0xC1			// horizontal up	╨	4
		,	0xC2			// horizontal down	╥	5
		,	0xC3			// vertical right	╞	6
		,	 197			// cross vertical	╪	7
		,	 197			// cross horizontal	╫	8
		,	0xD9			// up left			╛	9
		,	0xDA			// down right		╒	10
		}; // "   "										11

	static const unsigned char			ascii_wall_combined_reinforced	[] = 
		{	' '				// empty				0
		,	180				// vertical left	╢	1
		,	191				// down left		╖	2
		,	192				// up right			╙	3
		,	193				// horizontal up	╧	4
		,	194				// horizontal down	╤	5
		,	195				// vertical right	╟	6
		,	197				// cross vertical	╫	7
		,	197				// cross horizontal	╪	8
		,	217				// up left			╜	9
		,	218				// down right		╓	10
		}; 	//											11

	static const unsigned char			ascii_wall_solid				[] = 
		{	' '				// empty				0
		,	180				// vertical left	╢	1
		,	191				// down left		╖	2
		,	192				// up right			╙	3
		,	193				// horizontal up	╧	4
		,	194				// horizontal down	╤	5
		,	195				// vertical right	╟	6
		,	197				// cross vertical	╫	7
		,	197				// cross horizontal	╪	8
		,	217				// up left			╜	9
		,	218				// down right		╓	10
		}; 	//											11

	enum ASCII_WALL : unsigned char
		{ ASCII_WALL_NONE							= 0
		, ASCII_WALL_VERTICAL							// 1
		, ASCII_WALL_CONNECTION_VERTICAL_LEFT			// 2
		, ASCII_WALL_CORNER_DOWN_LEFT					// 3
		, ASCII_WALL_CORNER_UP_RIGHT					// 4
		, ASCII_WALL_CONNECTION_HORIZONTAL_UP			// 5
		, ASCII_WALL_CONNECTION_HORIZONTAL_DOWN			// 6
		, ASCII_WALL_CONNECTION_VERTICAL_RIGHT			// 7
		, ASCII_WALL_HORIZONTAL							// 8
		, ASCII_WALL_CONNECTION_CROSS					// 9
		, ASCII_WALL_CORNER_UP_LEFT						// 10
		, ASCII_WALL_CORNER_DOWN_RIGHT					// 11
		, ASCII_WALL_COUNT								// 12
		, ASCII_WALL_INVALID							= 0xFF// 12
		};										

	struct SASCIIWallConnection {
						bool						Bottom				: 1;
						bool						Top					: 1;
						bool						Left				: 1;
						bool						Right				: 1;
						bool						ReinforcedBottom	: 1;
						bool						ReinforcedTop		: 1;
						bool						ReinforcedLeft		: 1;
						bool						ReinforcedRight		: 1;
	};

	static inline	char						resolveASCIIConnectionCross				( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_CROSS				] : ascii_wall_single[ASCII_WALL_CONNECTION_CROSS				]; }
	static inline	char						resolveASCIIConnectionHorizontalUp		( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_HORIZONTAL_UP		] : ascii_wall_single[ASCII_WALL_CONNECTION_HORIZONTAL_UP		]; }
	static inline	char						resolveASCIIConnectionHorizontalDown	( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_HORIZONTAL_DOWN	] : ascii_wall_single[ASCII_WALL_CONNECTION_HORIZONTAL_DOWN		]; }
	static inline	char						resolveASCIIConnectionVerticalRight		( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_VERTICAL_RIGHT		] : ascii_wall_single[ASCII_WALL_CONNECTION_VERTICAL_RIGHT		]; }
	static inline	char						resolveASCIIConnectionVerticalLeft		( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CONNECTION_VERTICAL_LEFT		] : ascii_wall_single[ASCII_WALL_CONNECTION_VERTICAL_LEFT		]; }
	static inline	char						resolveASCIICornerUpRight				( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_UP_RIGHT				] : ascii_wall_single[ASCII_WALL_CORNER_UP_RIGHT				]; }
	static inline	char						resolveASCIICornerUpLeft				( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_UP_LEFT				] : ascii_wall_single[ASCII_WALL_CORNER_UP_LEFT					]; }
	static inline	char						resolveASCIICornerDownRight				( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_DOWN_RIGHT				] : ascii_wall_single[ASCII_WALL_CORNER_DOWN_RIGHT				]; }
	static inline	char						resolveASCIICornerDownLeft				( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_CORNER_DOWN_LEFT				] : ascii_wall_single[ASCII_WALL_CORNER_DOWN_LEFT				]; }
	static inline	char						resolveASCIIVertical					( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_VERTICAL						] : ascii_wall_single[ASCII_WALL_VERTICAL						]; }
	static inline	char						resolveASCIIHorizontal					( bool bIsReinforced, const SASCIIWallConnection& /*connection*/) { return (bIsReinforced) ? ascii_wall_double[ASCII_WALL_HORIZONTAL					] : ascii_wall_single[ASCII_WALL_HORIZONTAL						]; }
														 
	static			const unsigned char			ascii_fraction	[]						= {' ',	0xFF & 0xAC, 0xFF & 0xAB };	//'¼', '½'};
	static			const unsigned char			ascii_water		[]						= {' ', 0xF7, '~' };						//'≈', '~'};
	static			const unsigned char			ascii_tones		[]						= {' ', 0xB0, 0xB1, 0xB2};	//" ░▒▓";
	static			const unsigned char			ascii_arrow		[]						= {0, 0x11, 0x1F, 0x10, 0x1E};					//" ◄▲►▼";
	//static const char ascii_arrw2[] = " ←↑→↓↕↔↨";

	#define ARROW_LEFT		1
	#define ARROW_UP		2
	#define ARROW_RIGHT		3
	#define ARROW_DOWN		4
	#define ARROW_UPDOWN	5
	#define ARROW_LEFTRIGHT	6

	static const unsigned char					ascii_gender	[]						= {' ', 0x0C, 0x0B, 232};				// " ♀♂Φ";
	#define FEMALE			1
	#define MALE			2
	#define HERMAPHRODITE	3

	//static const char ascii_sound[] = "♫";

	static const char							ascii_cards		[]						= {' ', 0x03, 0x04, 0x05, 0x06};				// " ♥♦♣♠";
	#define DECK_HEARTS		1
	#define DECK_DIAMONDS	2
	#define DECK_CLUBS		3
	#define DECK_SPADES		4

	static const char							ascii_face		[]						= {' ', 0x01, 0x02};								//" ☺☻";
	#define FACE_BLACK	1
	#define FACE_WHITE	2

	static const char							ascii_button	[]						= {' ', 0x09, 0x0A};							//" ○◙";
	#define BUTTON_OFF	1
	#define BUTTON_ON	2

	static const char							ascii_buttn2	[]						= {' ', 0x07, 0x08};							//" •◘";
	#define BUTTON_OFF	1
	#define BUTTON_ON	2

	// ↑ ↓ → ← 
	// ↔ ↕ ↨ 
	// ⌂
	// ☼
	// « »
	// ª º °
	// ∙
	// ÷
	// ≡
	// ≥ ≤ ±
	// ⌠ ⌡
	// ∞
	// ö Ö
	// ü Ü
	// £ ¥ ₧ $ ¢
	// { | }
	// ƒ 
	// α ß Σ σ µ τ Φ Θ δ Ω φ ε ∩ π 
	// ‼ ¶ § 
	// √ ⁿ ² 
	// ç Ç
	// c C
	// [X] [√]
	// ^ < >
	//static const char ascii_blah0 = "'☺☻♥♦♣♠ •◘ ○◙  ♂ ♀ ↨ ♫ ☼ ►◄ ↕  ▬ ↨∟ ↔ ▲ ▼ !\"#%&'()*+,-./0123456789:;<=>?@ABDEFGHIJKLMNOPQRSTUVWXYZ[\\]_`abdefghijklmnopqrstuvwxyz~ ⌂ éâäàåêëèïîìÄÅÉæÆôòûùÿ áíóúñÑ ¿⌐¬ ½ ¼ ¡ « » ░▒▓│┤╣║╗┐└┴┬├─┼╚╔╩╠═╬┘┌ Γ ÷≈°∙· ";
#pragma pack(pop)
}

#endif // KLIB_ASCII_REFERENCE_H_23627


// ▌ █▐ ▄▌▄▌▐▀■█▄▌▐▀■█▄▌▐▀■█▄▌▐▀■▀■▄▌▐
//    █▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀█
//    ▌                            █
//    █                            █
//    █                            █
//    █                            ▐
//    █                            █
//    █                            █
//    █                            █
//    █                            █
//    █                            █
//    █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█

