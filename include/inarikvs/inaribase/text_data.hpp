#ifndef	__inarikvs_text_data__hpp
#define	__inarikvs_text_data__hpp
#include "../inarikvs-def.hpp"
#include "../inarikvs-types.hpp"
#include "file.hpp"
#include "hashkey.hpp"
#include "keyfile_data.hpp"
INARIKVS_NAMESPACE_BEGIN


struct	text_data : hashkey
{
	char*			data		= nullptr;		//!< データを保存しているメモリ

	inline text_data() = default;
	inline text_data(const text_data&) = delete;
	inline text_data& operator = (const text_data&) = delete;
	inline text_data(text_data&& _s)
			: hashkey(std::move(_s))
	{
		data		= _s.data;		_s.data		= nullptr;
	}

	~text_data()
	{
		destroy();
	}


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
		length				= _txt.size();
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


	inline void load(file& _file, int64_t _pos, uint32_t _size)
	{
		data	= new char[_size + 1];
		_file.read(_pos, data, _size);
		data[_size] = 0;
		length	= _size;
	}
};


INARIKVS_NAMESPACE_END
#endif
