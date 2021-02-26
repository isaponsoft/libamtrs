/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__table__hpp
#define	__inarikvs__table__hpp
#include  <iomanip>
#include <map>
#include <set>
#include "keyweiter.hpp"
#include "key_group.hpp"
#include "kv_data.hpp"
#include "logger.hpp"
#include "tree.hpp"
INARIKVS_NAMESPACE_BEGIN

template<class Storage>
class	table
{
public:
	using	storage_type	= Storage;

	void set_storage(storage_type* _index, storage_type* _value, logger<storage_type>* _log)
	{
		mIndexFile	= _index;
		mValueFile	= _value;
		mLogger		= _log;
	}

	bool del(std::string_view _key)
	{
		uint64_t	idx	= kv_data::tree_hash(_key).first;
		auto		it	= mTable.find(idx);
		if (it != mTable.end())
		{
			auto	kv	= it->second.del(*mValueFile, _key);
			if (kv)
			{
				mLogger->add(log_type::del, kv);
				if (it->second.empty())
				{
					mTable.erase(idx);
				}
				return	true;
			}
		}
		return	false;
	}



	result set(std::string_view _key, std::string_view _value)
	{
		using	vt	= typename tree::basic_unordered_map<uint64_t, key_group>::value_type;

		auto[idx,h]		= kv_data::tree_hash(_key);
		auto		r	= mTable.insert(vt{idx, key_group{}});
		auto&		kg	= r.first->second;
		kv_data*	kv	= kg.find(*mValueFile, _key);
		if (!kv)
		{
			// 新規追加
			kv	= kg.add(new kv_data());
			kv->set_key(_key);
			kv->set_value(_value);
			kv->set_state(kv_state::created);
			mLogger->add(log_type::set, kv);
			return	result::create;
		}
		else
		{
			// 上書き
			kv->load(*mValueFile);
			if (kv->value_compare(_value))
			{
				// 値の変更がない場合は上書を無視する(ディスクへの書き込みをなるべく少なくする)
				return	result::nochange;
			}
			kv->set_value(_value);
			if (kv->state() == kv_state::keep)
			{
				kv->set_state(kv_state::update);
				mLogger->add(log_type::set, kv);
			}
			return	result::change;
		}
	}


	kv_data* get(std::string_view _key)
	{
		auto		it	= mTable.find(kv_data::tree_hash(_key).first);
		if (it == mTable.end())
		{
			// 見つからなかった
			return	nullptr;
		}

		auto*	kv	= it->second.find(*mValueFile, _key);
		kv->load(*mValueFile);
		return	kv;
	}


	amtrs::ref<kv_data> restore(segment_desc& _seg, uint64_t _pos)
	{
		amtrs::ref<kv_data>	kv		= new kv_data(_pos, _seg);
		auto				r		= mTable.insert({kv->tree_hash(), key_group{}});
		auto&				kg		= r.first->second;
		kv->set_index_pos(_pos);
		kg.add(kv);
		return	kv;
	}

	// ========================================================================
	//! キーを列挙し、コールバックを呼び出します。
	// ------------------------------------------------------------------------
	template<class Callback>
	kv_data* keys(Callback&& _callback)
	{
		for (auto& kg : mTable)
		{
			for (kv_data* kv : kg.second)
			{
				kv->load(*mValueFile);
				if (_callback(kv))
				{
					return	kv;
				}
			}
		}
		return	nullptr;
	}

private:
	tree::basic_unordered_map<uint64_t, key_group>	mTable;	
	logger<storage_type>*							mLogger;
	storage_type*									mIndexFile	= nullptr;
	storage_type*									mValueFile	= nullptr;
};


INARIKVS_NAMESPACE_END
#endif
