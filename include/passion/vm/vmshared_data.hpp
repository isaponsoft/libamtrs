#ifndef	__passion__vm_shared_data__hpp
#define	__passion__vm_shared_data__hpp
#include <unordered_map>
#include <amtrs/logging.hpp>
#include "def.hpp"
#include "types.hpp"
#include "vmclass.hpp"
#include "vmobject.hpp"
#include "vmstrobj.hpp"
PASSION_NAMESPACE_BEGIN

// ****************************************************************************
//!	VMで使用する共有データ。
// ----------------------------------------------------------------------------
class	vmshared_data : public ref_object
{
public:
	enum class_types
	{
		string_clz	= 0,
		array_clz	= 1,
		arraygc_clz	= 2,

		__max
	};

	static ref<vmshared_data> get_instance()
	{
		ref<vmshared_data>	retval	= mInstance;
		if (retval.empty())
		{
			mInstance	= new vmshared_data();
			retval		= mInstance;
			retval->initialize();
		}
		return	retval;
	}

	~vmshared_data();


	vmclass* clazz(class_types _t) const noexcept { return mClasses[_t]; }

	vmfunction* deallocator() const noexcept { return mDeallocator.get(); }

private:
	void initialize();


	vmshared_data()
	{}

	ref<vmclass>		mClasses[class_types::__max];

	ref<vmfunction>		mDeallocator;



	static inline vmshared_data*	mInstance	= nullptr;
};


PASSION_NAMESPACE_END
#endif
