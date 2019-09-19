#ifndef	__passion__vm__types__hpp
#define	__passion__vm__types__hpp
#include <amtrs/string.hpp>
#include <amtrs/shared_buffer.hpp>
#include "../def.hpp"
PASSION_NAMESPACE_BEGIN
using namespace amtrs;

const static std::size_t	npos			= static_cast<std::size_t>(-1);




class	vmclass_manager;
struct	vmclass;
struct	vmcontext;
struct	vmobject;
struct	vmarray;
struct	vmfunction;
struct	vmbytecode;
class	vmshared_data;


using	attribute_flag	= passion_function_attributes;



struct	vmvariable_entry
{
	vmclass*	clazz		= nullptr;	//!< クラス名
	vmstring	name;					//!< 変数名
	vmstring	thiz;					//!< 参照元のクラス名
	int			position	= 0;		//!< 位置

	bool operator == (const vmvariable_entry& _b) const noexcept { return clazz == _b.clazz && name == _b.name && thiz == _b.thiz; }
	bool operator != (const vmvariable_entry& _b) const noexcept { return !(*this == _b); }
};



// ============================================================================
//!	VMで扱う動的変数値
// ----------------------------------------------------------------------------
struct	vmvariant
		: std::variant<ref<vmobj>, ref<vmfunction>, vmvalue>
{
	using	_base_type	= std::variant<ref<vmobj>, ref<vmfunction>, vmvalue>;
	using	_base_type::_base_type;
	using	_base_type::operator = ;


	vmvariant() {}
	vmvariant(vmint_t _v) : _base_type(vmvalue(_v)) {}
};

PASSION_NAMESPACE_END
#endif
