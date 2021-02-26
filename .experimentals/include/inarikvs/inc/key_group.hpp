/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
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

	template<class StorageT>
	inline kv_data* find(StorageT& _vf, std::string_view _key)
	{
		for (auto& e : mKVList)
		{
			e->key_load(_vf);
			if (_key == e->key())
			{
				return	e;
			}
		}
		return	nullptr;
	}

	template<class StorageT>
	inline amtrs::ref<kv_data> del(StorageT& _file, std::string_view _key)
	{
		for (auto kvit = begin(); kvit != end(); ++kvit)
		{
			(*kvit)->key_load(_file);
			if ((*kvit)->key() == _key)
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
