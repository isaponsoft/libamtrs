#ifndef	__inarikvs__key_group__hpp
#define	__inarikvs__key_group__hpp
#include <vector>
#include "kv_data.hpp"
INARIKVS_NAMESPACE_BEGIN


// キーグループは同一のハッシュ値を持つキーの集合体
class	key_group
{
public:
	inline kv_data* add(kv_data* _kv)
	{
		mKVList.push_back(_kv);
		return	_kv;
	}

	inline auto begin() noexcept { return mKVList.begin(); }
	inline auto end() noexcept { return mKVList.end(); }
	inline bool empty() const noexcept { return mKVList.empty(); }

	inline kv_data* find(file& _vf, std::string_view _key)
	{
		for (auto& e : mKVList)
		{
			if (e->key.empty())
			{
				e->key_load(_vf);
			}
			if ((_key.size() == e->key.length) && !std::memcmp(e->key.data, _key.data(), _key.size()))
			{
				return	e;
			}
		}
		return	nullptr;
	}

	inline amtrs::ref<kv_data> del(file& _file, std::string_view _key)
	{
		for (auto kvit = begin(); kvit != end(); ++kvit)
		{
			if ((*kvit)->key.empty())
			{
				(*kvit)->key_load(_file);
			}
			if (((*kvit)->key.length == _key.size()) && !std::memcmp((*kvit)->key.data, _key.data(), _key.size()))
			{
				amtrs::ref<kv_data>	retval(*kvit);
				if (*kvit != mKVList.back())
				{
					*kvit	= std::move(mKVList.back());
				}
				mKVList.resize(mKVList.size() - 1);
				return	retval;
			}
		}
		return	{};
	}


private:
	std::vector<amtrs::ref<kv_data>>	mKVList;
};


INARIKVS_NAMESPACE_END
#endif
