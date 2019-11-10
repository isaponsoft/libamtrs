#ifndef	__passion__vm__hpp
#define	__passion__vm__hpp
#include <unordered_map>
#include <amtrs/logging/@>
#include "def.hpp"
#include "types.hpp"
#include "vmclass.hpp"
#include "vmobject.hpp"
#include "vmstrobj.hpp"
PASSION_NAMESPACE_BEGIN


//!	クラス一覧を管理する。
class	vmclass_manager
{
public:
	vmclass_manager();

	vmclass_manager(vmclass_manager* _parent)
		: mParent(_parent)
	{
		mSharedData	= mParent->mSharedData;
		mClsGlobal	= mParent->mClsGlobal;
		mClsInt		= mParent->mClsInt;
		mClsString	= mParent->mClsString;
	}

	~vmclass_manager()
	{
		mClasses.clear();
		mSharedData.clear();
	}


	// ========================================================================
	//!	クラスを作成します。
	// ------------------------------------------------------------------------
	//! すでに同名のクラスがあった場合は、例外を投げます。
	// ------------------------------------------------------------------------
	vmclass* create_class(const vmstring& _class_name, std::size_t _base_class_count = 0, vmclass* _base_classes[] = nullptr);


	// ========================================================================
	//!	ネイティブクラスを作成します。
	// ------------------------------------------------------------------------
	template<class NTC>
	vmclass* create_native_class();



	// ========================================================================
	//!	クラスのプロトタイプを行います。
	// ------------------------------------------------------------------------
	//! すでに同名のクラスがあった場合は、登録済みのクラスを返します。
	// ------------------------------------------------------------------------
	vmclass* prototype_class(const vmstring& _class_name);

	vmclass* get_class(const vmstring& _className) const
	{
		auto	clazz	= mClasses.find(_className.c_str());
		return	clazz != mClasses.end() ? clazz->second.get()
			:	mParent                 ? mParent->get_class(_className)
			:	nullptr;
	}


	vmclass* get_global_scope() const noexcept { return mClsGlobal; }

	const auto& classes() const noexcept { return mClasses; }

	template<class Callback>
	size_t enumlate_classes(Callback _callback)
	{
		size_t	count	= 0;
		for (auto e : mClasses)
		{
			if (!_callback(string_view(e.first), static_cast<vmclass*>(e.second)))
			{
				break;
			}
			++count;
		}
		return	count;
	}

	// ========================================================================
	//!	クラスを登録します。
	// ------------------------------------------------------------------------
	//! すでに同名のクラスがあった場合は、false を返します。
	// ------------------------------------------------------------------------
	bool add_class(ref<vmclass> _class)
	{
		if (get_class(_class->identify()))
		{
			return	false;
		}
		mClasses[_class->identify()] = std::move(_class);
		return	true;
	}


private:
	vmclass_manager*							mParent	= nullptr;
	std::unordered_map<vmstring, ref<vmclass>>	mClasses;

	ref<vmshared_data>							mSharedData;

	// よく使うクラスのキャッシュ
	vmclass*									mClsGlobal;
	vmclass*									mClsInt;
	vmclass*									mClsString;
};


PASSION_NAMESPACE_END
#endif
