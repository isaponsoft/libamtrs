#ifndef	__inarikvs_types__hpp
#define	__inarikvs_types__hpp
#include "inarikvs-def.hpp"
INARIKVS_NAMESPACE_BEGIN

enum class	result
{
	create,
	nochange,
	change
};

enum class	log_type
{
	del,
	set,
};


enum class	kv_state
{
	keep,				//!< 状態は維持したまま
	created,			//!< 作成されたばかりでまだ１度も保存されていない
	del,				//!< キーが削除された
	update,				//!< 値が更新された
};


using	hash_type		= uint32_t;
using	size_type		= uint32_t;

INARIKVS_NAMESPACE_END
#endif
