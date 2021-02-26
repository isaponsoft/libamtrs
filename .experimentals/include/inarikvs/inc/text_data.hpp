/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs_text_data__hpp
#define	__inarikvs_text_data__hpp
#include "inarikvs-def.hpp"
#include "inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN


struct	text_data
{
	inline text_data() = default;
	inline text_data(const text_data&) = delete;
	inline text_data& operator = (const text_data&) = delete;
	inline text_data(text_data&& _s)
	{
		length		= _s.length;	_s.length	= 0;
		data		= _s.data;		_s.data		= nullptr;
	}

	explicit text_data(size_type _size)
		: length(_size)
	{}

	~text_data()
	{
		destroy();
	}

	size_type size() const noexcept { return length; }


	explicit operator std::string_view () const noexcept
	{
		return	std::string_view(data, length);
	}


	inline bool operator == (std::string_view _txt)
	{
		return	std::string_view(*this) == _txt;
	}

	inline void create(std::string_view _txt)
	{
		destroy();
		data	= new char[_txt.size() + 1];
		std::memcpy(data, _txt.data(), _txt.size());
		data[_txt.size()]	= 0;
		length				= static_cast<size_type>(_txt.size());
	}

	inline void destroy()
	{
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
	}

	inline bool empty() const noexcept { return data == nullptr; }

	//! ジャーナル形式でロードします。
	template<class StorageT>
	void journal_load(StorageT& _storage)
	{
		uint32_t	len	= 0;
		_storage.read(&len, sizeof(len));

		destroy();

		length	= len;
		data		= new char[length + 1];
		_storage.read(data, length);
		data[length]	= 0;
	}



	template<class StorageT>
	inline void load(StorageT& _storage, int64_t _pos, uint32_t _size)
	{
		data	= new char[_size + 1];
		_storage.read(_pos, data, _size);
		data[_size] = 0;
		length	= _size;
	}

private:
	size_type		length		= 0;
	char*			data		= nullptr;		//!< データを保存しているメモリ
};


INARIKVS_NAMESPACE_END
#endif
