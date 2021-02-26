/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__segment__hpp
#define	__inarikvs__segment__hpp
#include <type_traits>
#include <amtrs/amtrs.hpp>
#include "inarikvs-def.hpp"
#include "inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN


enum class	data_type : int8_t
{
	string_data	= 0,			//!< 文字型
	list_data	= 1,			//!< リスト型
};


constexpr int __bit(int _v, int _b, int _n)
{
	return	_n == _v ? _b : __bit(_v, _b+1, _n*2);
}

//! セグメント情報
struct	segment_desc
{
	using	pos_type	= int64_t;
	using	size_type	= uint32_t;
	using	hash_type	= uint32_t;



	static constexpr pos_type	align_size	= 8;
	static constexpr pos_type	align_bit	= __bit(align_size, 1, 2);
	static constexpr pos_type	npos		= static_cast<pos_type>(-1);


	int64_t			_pos	: 61;	//!< ファイルの位置(8byte単位),未使用の場合は npos がセットされる
	int64_t			_key_h	:  3;	//!< キーの長さ(上位3bit),リストのアイテムなどは 0 がセットされる
	//...
	uint32_t		_hash	: 24;	//!< ハッシュ値(1600万),リストのアイテムなどは 0 がセットされる
	uint32_t		_key_l	:  8;	//!< キーの長さ(下位8bit),リストのアイテムなどは 0 がセットされる
	//...
	uint32_t		_val	: 29;	//!< 値の長さ
	uint32_t		_type	:  3;	//!< 種類


	constexpr void clear() noexcept
	{
		*this	= {0};
		_pos	= npos;
	}

	static constexpr size_type filesize(uint32_t _keyLength, uint32_t _valueLength) noexcept { return (_keyLength + _valueLength + align_size - 1) & ~(align_size-1); }

	constexpr size_type filesize() const noexcept { return filesize(key_length(), val_length()); }

	constexpr bool empty() const noexcept { return _pos == npos; }

	constexpr data_type type() const noexcept { return static_cast<data_type>(_type); }

	constexpr segment_desc& type(data_type _v) noexcept { _type = static_cast<int8_t>(_v); return *this; }

	constexpr pos_type filepos() const noexcept { return empty() ? npos : (_pos << align_bit); }
	constexpr segment_desc& filepos(pos_type _v) noexcept { _pos = _v>>align_bit; return *this; }

	constexpr size_type key_length() const noexcept { return static_cast<size_type>((_key_h << 8) + _key_l); }
	constexpr segment_desc& key_length(size_type _v) noexcept { _key_l = static_cast<uint8_t>(_v); _key_h = static_cast<uint8_t>(_v>>8); return *this; }

	constexpr size_type val_length() const noexcept { return _val; }
	constexpr segment_desc& val_length(size_type _v) noexcept { _val = _v; return *this; }

	constexpr hash_type hash() const noexcept { return _hash; }
	constexpr segment_desc& hash(hash_type _v) noexcept { _hash = _v; return *this; }

	constexpr segment_desc& key(std::string_view _key) noexcept
	{
		key_length(static_cast<size_type>(_key.size()));
		hash(compute_hash(_key));
		return	*this;
	}

	template<class It>
	static constexpr hash_type compute_hash(It _first, It _last) noexcept
	{
		hash_type	h	= 0x11241;
		It			it	= _first;
		while (it != _last)
		{
			h = h * 31 + *it++;
		}
		segment_desc	tmp;
		tmp.hash(h);
		return	tmp.hash();
	}

	static constexpr hash_type compute_hash(std::string_view _key) noexcept { return compute_hash(_key.begin(), _key.end()); }

	static segment_desc make(std::string_view _key) noexcept
	{
		segment_desc	retval;
		retval.type(data_type::string_data);
		retval.key(_key);
		retval.filepos(0);
		retval.val_length(0);
		return	retval;
	}
};
static_assert(sizeof(segment_desc) == 0x10);
static_assert(std::is_pod<segment_desc>::value == true);



INARIKVS_NAMESPACE_END
#endif
