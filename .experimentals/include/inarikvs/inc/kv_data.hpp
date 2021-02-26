/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs_kv_data__hpp
#define	__inarikvs_kv_data__hpp
#include <amtrs/amtrs.hpp>
#include "inarikvs-def.hpp"
#include "inarikvs-types.hpp"
#include "segment_desc.hpp"
#include "text_data.hpp"
INARIKVS_NAMESPACE_BEGIN



class	kv_data
		: public amtrs::ref_object
{
public:
	inline kv_data() = default;
	inline kv_data(kv_data&&) = default;
	inline kv_data(const kv_data&) = delete;
	inline kv_data& operator = (const kv_data&) = delete;


	inline kv_data(int64_t _index_pos, const segment_desc& _ifb)
		: mState(kv_state::keep)
		, mIndexPos(_index_pos)
		, mSavedStatus(_ifb)
		, mKey(text_data(_ifb.key_length()))
		, mValue(text_data(_ifb.val_length()))
	{}



	//! 
	inline auto hash() const noexcept { return saved_status().hash(); }

	//! 保存済みのステータスを取得する。
	const segment_desc& saved_status() const noexcept { return mSavedStatus; }

	//! 保存済みかどうか調べる。
	bool saved() const noexcept { return !mSavedStatus.empty(); }

	//! 値ファイルに保存する際のサイズを取得します。
	uint32_t value_file_size() const noexcept { return segment_desc::filesize(mKey.size(), mValue.size()); }
/*
	void journal_key_load(file& _file)
	{
		mKey.journal_load(_file);
	}

	void journal_value_load(file& _file)
	{
		mValue.journal_load(_file);
	}
*/
	std::string_view key() const noexcept { return (std::string_view)mKey; }
	std::string_view value() const noexcept { return (std::string_view)mValue; }

	//! セーブしたことを記録する。
	void save(uint64_t _pos)
	{
		mSavedStatus.key_length(mKey.size());
		mSavedStatus.val_length(mValue.size());
		mSavedStatus.filepos(_pos);
	}

	//! 未セーブ状態にする。
	void unsave() noexcept
	{
		mSavedStatus.filepos(segment_desc::npos);
	}

	//! キーをセットします。
	void set_value(std::string_view _value)
	{
		mValue.create(_value);
	}

	//! キーをセットします。
	void set_key(std::string_view _key)
	{
		auto	h	= kv_data::tree_hash(_key).second;
		mKey.create(_key);
		mSavedStatus.hash(h);
	}

	bool value_compare(std::string_view _v)
	{
		return	mValue == _v;
	}


	//! キーデータを復旧します。
	template<class StorageT>
	inline void key_load(StorageT& _storage)
	{
		if (mKey.empty())
		{
			mKey.load(std::forward<StorageT>(_storage), mSavedStatus.filepos(), mSavedStatus.key_length());
		}
	}


	template<class StorageT>
	inline void val_load(StorageT& _storage)
	{
		if (mValue.empty())
		{
			mValue.load(std::forward<StorageT>(_storage), mSavedStatus.filepos() + mSavedStatus.key_length(), mSavedStatus.val_length());
		}
	}


	template<class StorageT>
	inline void load(StorageT& _storage)
	{
		key_load(std::forward<StorageT>(_storage));
		val_load(std::forward<StorageT>(_storage));
	}


	// ========================================================================
	//! 木で使用するハッシュ値を生成します。
	// ------------------------------------------------------------------------
	constexpr uint64_t tree_hash() const noexcept
	{
		return	tree_hash(mSavedStatus.hash(), mKey.size());
	}


	// ========================================================================
	//! 木で使用するハッシュ値を生成します。
	// ------------------------------------------------------------------------
	static constexpr uint64_t tree_hash(hash_type _keyHash, size_type _keyLength) noexcept
	{
		return	((uint64_t)_keyHash << 32) ^ (_keyLength << 15) + (_keyHash >> 15) + _keyLength;
	}


	// ========================================================================
	//! 木で使用するハッシュ値を生成します。
	// ------------------------------------------------------------------------
	static constexpr std::pair<uint64_t, size_type> tree_hash(std::string_view _key) noexcept
	{
		auto	hash	= segment_desc::compute_hash(_key);
		auto	length	= _key.size();
		return	{ tree_hash(hash, static_cast<size_type>(length)), hash };
	}

	kv_state state() const noexcept { return mState; }

	void set_state(kv_state _s) noexcept { mState = _s; }

	auto index_pos() const noexcept { return mIndexPos; }
	void set_index_pos(int64_t _pos) noexcept { mIndexPos = _pos; }

	void clear_index_pos() noexcept { mIndexPos = -1; }

private:
	kv_state			mState		= kv_state::keep;
	int64_t				mIndexPos	= -1;					//!< index file 上の位置

	segment_desc		mSavedStatus;						//!< ファイルに保存済みの情報
	text_data			mKey;
	text_data			mValue;
};



INARIKVS_NAMESPACE_END
#endif
