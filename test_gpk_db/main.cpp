#include "gpk_datatype.h"
#include "gpk_array.h"
#include "gpk_label.h"

struct STable {
	::gpk::label							Name		;
	::gpk::array_obj<::gpk::label>			RowNames	;
	::gpk::array_pod<::gpk::DATA_TYPE>		RowTypes	;
};

#pragma pack(push, 1)
enum GPK_SQL_OP 
	{	GPK_SQL_OP_NOP		= 0
	,	GPK_SQL_OP_SELECT	
	,	GPK_SQL_OP_DELETE	
	,	GPK_SQL_OP_INSERT	
	,	GPK_SQL_OP_UPDATE	
	,	GPK_SQL_OP_WHERE	
	,	GPK_SQL_OP_JOIN		
	,	GPK_SQL_OP_FROM		
	,	GPK_SQL_OP_VALUES	
	,	GPK_SQL_OP_INNER	
	,	GPK_SQL_OP_OUTER	
	,	GPK_SQL_OP_RIGHT	
	,	GPK_SQL_OP_LEFT		
	,	GPK_SQL_OP_COUNT	
	};

struct SSQLOperator {
	::GPK_SQL_OP					OpCode;
	::gpk::view_const_string		Name;
};

static const ::SSQLOperator		table_operators	[]	= 
	{ {GPK_SQL_OP_NOP		, "NOP"		}
	, {GPK_SQL_OP_SELECT	, "SELECT"	}
	, {GPK_SQL_OP_DELETE	, "DELETE"	}
	, {GPK_SQL_OP_INSERT	, "INSERT"	}
	, {GPK_SQL_OP_UPDATE	, "UPDATE"	}
	, {GPK_SQL_OP_WHERE		, "WHERE"	}
	, {GPK_SQL_OP_JOIN		, "JOIN"	}
	, {GPK_SQL_OP_FROM		, "FROM"	}
	, {GPK_SQL_OP_VALUES	, "VALUES"	}
	, {GPK_SQL_OP_INNER		, "INNER"	}
	, {GPK_SQL_OP_OUTER		, "OUTER"	}
	, {GPK_SQL_OP_RIGHT		, "RIGHT"	}
	, {GPK_SQL_OP_LEFT		, "LEFT"	}
	, {GPK_SQL_OP_COUNT		, "COUNT"	}
	};

struct SUserAccess {
	int32_t									User;
	::gpk::array_obj<int32_t>				Tables;
	::GPK_SQL_OP							AccessOperation;

};

struct SUser {
	::gpk::array_obj<int32_t>				Tables;
};
#pragma pack(pop)

struct SDatabase {
	::gpk::array_obj<::STable>				Tables		;
	::gpk::array_obj<::SUser>				Users		;
	::gpk::array_obj<::SUserAccess>			UserAccess	;
};

int										main					() {
	::gpk::view_const_string					query					= "SELECT * from databases;";
	::SDatabase									database				= {};
	return 0;
}
