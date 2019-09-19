#ifndef	__inarikvs_kv_data__hpp
#define	__inarikvs_kv_data__hpp
#include <amtrs/amtrs.hpp>
#include "../inarikvs-def.hpp"
#include "../inarikvs-types.hpp"
#include "keyfile_data.hpp"
#include "text_data.hpp"
INARIKVS_NAMESPACE_BEGIN


struct	kv_data : amtrs::ref_object, keyfile_data
{
	text_data			key;
	text_data			value;
	kv_state			state		= kv_state::keep;
	int64_t				index_pos	= -1;					//!< index file 上の位置

	inline kv_data() = default;
	inline kv_data(kv_data&&) = default;
	inline kv_data(const kv_data&) = delete;
	inline kv_data& operator = (const kv_data&) = delete;


	inline kv_data(int64_t _index_pos, const keyfile_data& _ifb)
		: keyfile_data(_ifb)
		, state(kv_state::keep)
		, index_pos(_index_pos)
	{
		key.hash		= keyfile_data::hash();
		key.length		= keyfile_data::key_length();
		value.length	= keyfile_data::val_length();
	}


	inline void key_load(file& _vf)
	{
		if (key.empty())
		{
			key.load(_vf, filepos(), key_length());
		}
	}


	inline void val_load(file& _vf)
	{
		if (value.empty())
		{
			value.load(_vf, filepos() + key_length(), val_length());
		}
	}


	inline void load(file& _vf)
	{
		key_load(_vf);
		val_load(_vf);
	}


	inline const struct hashkey& hashkey() const noexcept
	{
		return	key;
	}
};



INARIKVS_NAMESPACE_END
#endif
