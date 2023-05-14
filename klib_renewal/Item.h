#include "EntityImpl.h"
#include "klib_draw_misc.h"

#include "gpk_array_static.h"

#ifndef __ITEM_H__98214809271346928734293846__
#define __ITEM_H__98214809271346928734293846__

namespace klib
{
#pragma pack(push, 1)
	enum ITEM_TYPE : uint16_t
		{	ITEM_TYPE_UNKNOWN				= 0x00
		,	ITEM_TYPE_POTION				= 0x01
		,	ITEM_TYPE_GRENADE				= 0x02
		,	ITEM_TYPE_BOX					= 0x04
		};

	enum PROPERTY_TYPE : uint32_t
		{	PROPERTY_TYPE_NONE				= 0x00000000
		,	PROPERTY_TYPE_HEALTH			= 0x00000001
		,	PROPERTY_TYPE_MANA				= 0x00000002
		,	PROPERTY_TYPE_SHIELD			= 0x00000004
		,	PROPERTY_TYPE_DAMAGE			= 0x00000008
		,	PROPERTY_TYPE_HIT				= 0x00000010
		,	PROPERTY_TYPE_ATTACK_SPEED		= 0x00000020
		,	PROPERTY_TYPE_MOVEMENT_SPEED	= 0x00000040
		,	PROPERTY_TYPE_REFLEXES			= 0x00000080
		,	PROPERTY_TYPE_ABSORPTION		= 0x00000100
		,	PROPERTY_TYPE_HEALTH_DAMAGE		= 0x00000200
		,	PROPERTY_TYPE_MANA_DAMAGE		= 0x00000400
		,	PROPERTY_TYPE_SHIELD_DAMAGE		= 0x00000800
		,	PROPERTY_TYPE_BLAST				= 0x00001000
		,	PROPERTY_TYPE_PIERCING			= 0x00002000
		,	PROPERTY_TYPE_FRAG				= 0x00004000
		,	PROPERTY_TYPE_STUN				= 0x00008000
		,	PROPERTY_TYPE_SMOKE				= 0x00010000
		,	PROPERTY_TYPE_FIRE				= 0x00020000
		,	PROPERTY_TYPE_POISON			= 0x00040000
		,	PROPERTY_TYPE_COLD				= 0x00080000
		,	PROPERTY_TYPE_FREEZE			= 0x00100000
		,	PROPERTY_TYPE_SHOCK				= 0x00200000
		,	PROPERTY_TYPE_EMP				= 0x00400000
		,	PROPERTY_TYPE_SLEEP				= 0x00800000
		,	PROPERTY_TYPE_FLASHBANG			= 0x01000000
		,	PROPERTY_TYPE_ROCK				= 0x02000000
		,	PROPERTY_TYPE_RANGE				= 0x04000000
		,	PROPERTY_TYPE_SIGHT				= 0x08000000
		,	PROPERTY_TYPE_REVIVE			= 0x10000000
		};

	struct CItem {
		ITEM_TYPE											Type;
		PROPERTY_TYPE										Property;
		int64_t												Price;
		::gpk::view_const_char								Name;
	};

	static	const CItem									itemDescriptions[]			=
		// Type					// Property														// Price// Name
	{	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_NONE											,	 0, ::gpk::view_const_string{"Rock"						}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_HEALTH										,	10, ::gpk::view_const_string{"Potion of Healing"		}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_MANA											,	10, ::gpk::view_const_string{"Potion of Mana"			}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_SHIELD										,	10, ::gpk::view_const_string{"Potion of Shield"			}}
	,	CItem{ITEM_TYPE_POTION	,	(PROPERTY_TYPE)(PROPERTY_TYPE_MANA|PROPERTY_TYPE_HEALTH)	,	10, ::gpk::view_const_string{"Potion of Rejuvenation"	}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_HIT											,	10, ::gpk::view_const_string{"Potion of Focus"			}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_DAMAGE										,	10, ::gpk::view_const_string{"Potion of Strength"		}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_ATTACK_SPEED									,	10, ::gpk::view_const_string{"Potion of Speed"			}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_MOVEMENT_SPEED								,	10, ::gpk::view_const_string{"Potion of Sprint"			}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_REFLEXES										,	10, ::gpk::view_const_string{"Potion of Reflexes"		}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_ABSORPTION									,	10, ::gpk::view_const_string{"Potion of Shielding"		}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_HEALTH_DAMAGE									,	10, ::gpk::view_const_string{"Epidemic Potion"			}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_MANA_DAMAGE									,	10, ::gpk::view_const_string{"Potion of Damnation"		}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_SHIELD_DAMAGE									,	10, ::gpk::view_const_string{"Potion of Corrosion"		}}
	,	CItem{ITEM_TYPE_POTION	,	PROPERTY_TYPE_REVIVE										,	10, ::gpk::view_const_string{"Potion of Resurrection"	}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_SMOKE											,	10, ::gpk::view_const_string{"Smoke Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_SLEEP											,	10, ::gpk::view_const_string{"Sleep Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_STUN											,	10, ::gpk::view_const_string{"Stun Grenade"				}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_BLAST											,	10, ::gpk::view_const_string{"Blast Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_FIRE											,	10, ::gpk::view_const_string{"Fire Grenade"				}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_PIERCING										,	10, ::gpk::view_const_string{"Needle Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_FRAG											,	10, ::gpk::view_const_string{"Fragmentary Grenade"		}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_POISON										,	10, ::gpk::view_const_string{"Poison Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_SHOCK											,	10, ::gpk::view_const_string{"Shock Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_COLD											,	10, ::gpk::view_const_string{"Cold Grenade"				}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_FREEZE										,	10, ::gpk::view_const_string{"Freeze Grenade"			}}
	,	CItem{ITEM_TYPE_GRENADE	,	PROPERTY_TYPE_EMP											,	10, ::gpk::view_const_string{"EMP Grenade"				}}
	};
	struct CItemGrade {
		::gpk::view_const_char								Name;
	};

	static	const ::gpk::array_static<CItemGrade, 5>	itemGrades				=
		{ CItemGrade{::gpk::view_const_string{"%s (Prop)"	}}
		, CItemGrade{::gpk::view_const_string{"%s (Small)"	}}
		, CItemGrade{::gpk::view_const_string{"%s (Regular)"}}
		, CItemGrade{::gpk::view_const_string{"%s (Large)"	}}
		, CItemGrade{::gpk::view_const_string{"%s (Huge)"	}}
		};

	static inline constexpr	int32_t						getFinalItemCount					()													{ return (int32_t)((::gpk::size(itemDescriptions) - 1) * itemGrades.size()); }
	static inline			int32_t						getItemPrice						(const SItem& item, bool bSellPrice=false)			{ return (int32_t)( bSellPrice ? (itemDescriptions[item.Definition].Price*item.Level)*.5 : itemDescriptions[item.Definition].Price*item.Level ); }
							::gpk::array_pod<char>	getItemName							(const SItem& item);
	struct CItemV2 {
		ITEM_TYPE											Type;
		int64_t												Price;
		::gpk::view_const_char								Name;
	};


	static const SEntityRecord<SItem>					itemModifiers[]						=
		{ {{{},{},{},{},0,	 0, 0},{}, ::gpk::view_const_string{"%s"				}}
		, {{{},{},{},{},0,	 0, 0},{}, ::gpk::view_const_string{"%s of Healing"		}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"%s of Mana"		}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"%s of Shield"		}}
		, {{{},{},{},{},0,	15, 0},{}, ::gpk::view_const_string{"%s of Rejuvenation"}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"%s of Focus"		}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"Strength %s"		}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"Speed %s"			}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"Sprint %s"			}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"%s of Reflexes"	}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"%s of Shielding"	}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"Epidemic %s"		}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"%s of Damnation"	}}
		, {{{},{},{},{},0,	10, 0},{}, ::gpk::view_const_string{"%s of Corrosion"	}}
		, {{{},{},{},{},0,	 0, 0},{}, ::gpk::view_const_string{"Smoke %s"			}}
		, {{{},{},{},{},0,	 0, 0},{}, ::gpk::view_const_string{"Sleep %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Stun %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Blast %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Fire %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Needle %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Poison %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Cold %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Freeze %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"Shock %s"			}}
		, {{{},{},{},{},0,	 5, 0},{}, ::gpk::view_const_string{"EMP %s"			}}
		};

	//static const CItemV2 itemDefinitions[] =
	//		// Type										// Price	// Name
	//	{	{ITEM_TYPE_UNKNOWN							,	0	,	"Advice"			}
	//	,	{ITEM_TYPE_POTION							,	10	,	"Potion"			}
	//	,	{ITEM_TYPE_GRENADE							,	20	,	"Grenade"			}
	//	//,	{(ITEM_TYPE)(ITEM_TYPE_BOX|ITEM_TYPE_POTION	),	100	,	"Potion Box"		}
	//	//,	{(ITEM_TYPE)(ITEM_TYPE_BOX|ITEM_TYPE_GRENADE),	200	,	"Grenade Box"		}
	//	};
#pragma pack(pop)
} // namespace

#endif // __ITEM_H__98214809271346928734293846__
