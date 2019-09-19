#ifndef	__inarikvs__index_file_block__hpp
#define	__inarikvs__index_file_block__hpp
#include <type_traits>
#include <amtrs/amtrs.hpp>
#include "../inarikvs-def.hpp"
#include "../inarikvs-types.hpp"
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

//! インデックスファイルブロック
struct	keyfile_data
{
	using	pos_type	= int64_t;
	using	size_type	= uint32_t;
	using	hash_type	= uint32_t;



	static constexpr pos_type	align_size	= 8;
	static constexpr pos_type	align_bit	= __bit(align_size, 1, 2);
	static constexpr pos_type	npos		= static_cast<pos_type>(-1);


	int64_t			_pos	: 61;	//!< ファイルの位置(8byte単位)
	int64_t			_key_h	:  3;	//!< キーの長さ(上位3bit)
	//...
	uint32_t		_hash	: 24;	//!< ハッシュ値(1600万)
	uint32_t		_key_l	:  8;	//!< キーの長さ(下位8bit)
	//...
	uint32_t		_val	: 29;	//!< 値の長さ
	uint32_t		_type	:  3;	//!< 種類


	constexpr void clear() noexcept
	{
		*this	= {0};
		_pos	= npos;
	}

	constexpr size_type filesize() const noexcept { return (key_length() + val_length() + align_size - 1) & ~(align_size-1); }

	constexpr bool empty() const noexcept { return _pos == npos; }

	constexpr data_type type() const noexcept { return static_cast<data_type>(_type); }

	constexpr keyfile_data& type(data_type _v) noexcept { _type = static_cast<int8_t>(_v); return *this; }

	constexpr pos_type filepos() const noexcept { return empty() ? npos : (_pos << align_bit); }
	constexpr keyfile_data& filepos(pos_type _v) noexcept { _pos = _v>>align_bit; return *this; }

	constexpr size_type key_length() const noexcept { return (_key_h << 8) + _key_l; }
	constexpr keyfile_data& key_length(size_type _v) noexcept { _key_l = static_cast<uint8_t>(_v); _key_h = static_cast<uint8_t>(_v>>8); return *this; }

	constexpr size_type val_length() const noexcept { return _val; }
	constexpr keyfile_data& val_length(size_type _v) noexcept { _val = _v; return *this; }

	constexpr hash_type hash() const noexcept { return _hash; }
	constexpr keyfile_data& hash(hash_type _v) noexcept { _hash = _v; return *this; }

	constexpr keyfile_data& key(std::string_view _key) noexcept
	{
		key_length(_key.size());
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
		keyfile_data	tmp;
		tmp.hash(h);
		return	tmp.hash();
	}

	static constexpr hash_type compute_hash(std::string_view _key) noexcept { return compute_hash(_key.begin(), _key.end()); }

	static keyfile_data make(std::string_view _key) noexcept
	{
		keyfile_data	retval;
		retval.type(data_type::string_data);
		retval.key(_key);
		retval.filepos(0);
		retval.val_length(0);
		return	retval;
	}
};
static_assert(sizeof(keyfile_data) == 0x10);
static_assert(std::is_pod<keyfile_data>::value == true);



INARIKVS_NAMESPACE_END
#endif
