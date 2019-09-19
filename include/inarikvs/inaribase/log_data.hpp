#ifndef	__inarikvs__log_data__hpp
#define	__inarikvs__log_data__hpp
#include "kv_data.hpp"
INARIKVS_NAMESPACE_BEGIN

struct	log_data
{
	log_type			type		= log_type::del;
	amtrs::ref<kv_data>	kv;
};


INARIKVS_NAMESPACE_END
#endif
