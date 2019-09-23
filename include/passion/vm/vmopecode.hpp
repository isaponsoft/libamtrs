#ifndef	__passion__cpu__opecode__hpp
#define	__passion__cpu__opecode__hpp
#include "def.hpp"
PASSION_NAMESPACE_BEGIN

enum	opecode_id
{
#define	DEFOP(_op)	_op,
#include "vmopcode_table.hpp"
#undef	DEFOP
	None
};


inline const char* get_opecodename(opecode_id _id)
{
	switch (_id)
	{
		#define	DEFOP(_op)	case opecode_id::_op : return #_op;
		#include "vmopcode_table.hpp"
		#undef	DEFOP
		default: return nullptr;
	}
}

PASSION_NAMESPACE_END
#endif
