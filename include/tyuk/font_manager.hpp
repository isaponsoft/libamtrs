#ifndef	__toyoukebime__font_manager__hpp
#define	__toyoukebime__font_manager__hpp
#include <unordered_map>
#include <amtrs/graphics.hpp>
#include <amtrs/utility.hpp>
#include "tyuk-def.hpp"
#include "singleton_creater.hpp"
TYUK_NAMESPACE_BEGIN

template<class KeyT>
class	font_manager
		: public singleton<font_manager<KeyT>, singleton_creator<font_manager<KeyT>>>
		, public ref_object
{
public:
	using	key_type	= KeyT;

	bool has(const key_type& _key) const noexcept { return mFontList.end() != mFontList.find(_key); }

	void set(const key_type& _key, ref<font>&& _font) { mFontList[_key] = std::move(_font); }

	font* get(const key_type& _key) const noexcept
	{
		auto	it	= mFontList.find(_key);
		return	it == mFontList.end() ? nullptr : it->second.get();
	}


protected:
	float									mFontScale	= 1.0f;
	std::unordered_map<key_type, ref<font>>	mFontList;

	template<class>
	friend	struct	singleton_creator;
};



TYUK_NAMESPACE_END
#endif
