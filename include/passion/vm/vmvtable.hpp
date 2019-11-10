#ifndef	__passion__vm__vmvtable__hpp
#define	__passion__vm__vmvtable__hpp
#include "def.hpp"
#include "vmfunction.hpp"
#include "vmclass.hpp"
PASSION_EXTERN_C_BEGIN


PASSION_EXTERN_C_END

PASSION_NAMESPACE_BEGIN

// ============================================================================
//! 仮想関数テーブル
// ----------------------------------------------------------------------------
//! 実行時型情報と仮想関数のリストを保持します。
//! クラスと 1 : 1 で作成されます。
// ----------------------------------------------------------------------------
struct	vmvtable
		: public ref_object
{
	using	function_pointer	= passion_function*;


	size_t		base_offset;



	//! 仮想関数テーブルを生成します。
	template<class It>
	static ref<vmvtable> create(It _first, It _last)
	{
		std::size_t		size		= std::distance(_first, _last);
		std::size_t		thisSize	= sizeof(vmvtable) + (sizeof(vmfunction*) * size);
		auto			mem			= ::operator new (thisSize);

		ref<vmvtable>	thiz		= new (mem) vmvtable();
		std::copy(_first, _last, thiz->table_address());

		return	thiz;
	}

	static ref<vmvtable> create(std::size_t size)
	{
		std::size_t		thisSize	= sizeof(vmvtable) + (sizeof(vmfunction*) * size);
		auto			mem			= ::operator new (thisSize);
		ref<vmvtable>	thiz		= new (mem) vmvtable();
		return	thiz;
	}

	function_pointer destractor() const noexcept { return table_address()[0]; }


	      function_pointer* table_address()       noexcept { return reinterpret_cast<function_pointer*>(this+1); }
	const function_pointer* table_address() const noexcept { return reinterpret_cast<const function_pointer*>(this+1); }


	      function_pointer& operator [] (int _i)       noexcept { return table_address()[_i]; }
	const function_pointer& operator [] (int _i) const noexcept { return table_address()[_i]; }
};


PASSION_NAMESPACE_END
#endif
