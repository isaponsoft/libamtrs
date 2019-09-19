#ifndef	__inarikvs__hashkey__hpp
#define	__inarikvs__hashkey__hpp
#include <iostream>
#include "../inarikvs-def.hpp"
#include "../inarikvs-types.hpp"
#include "hash.hpp"
#include "keyfile_data.hpp"
INARIKVS_NAMESPACE_BEGIN

struct	hashkey
{
	hash_type	hash		= 0;
	size_type	length		= 0;

	constexpr hashkey() = default;
	constexpr hashkey(const hashkey&) = default;
	constexpr hashkey(hashkey&&) = default;

	constexpr hashkey(std::string_view _txt)
		: hash(keyfile_data::compute_hash(_txt))
		, length((uint32_t)_txt.size())
	{}

	constexpr uint64_t compute_hash() const noexcept { return ((uint64_t)hash << 32) ^ (length << 15) + (hash >> 15) + length; }

	constexpr bool operator == (const hashkey& b) const noexcept { return hash == b.hash && length == b.length; }
	constexpr bool operator != (const hashkey& b) const noexcept { return !(*this == b); }
	constexpr bool operator <  (const hashkey& b) const noexcept { return hash < b.hash && length < b.length; }
	constexpr bool operator <= (const hashkey& b) const noexcept { return hash <= b.hash && length <= b.length; }
	constexpr bool operator >  (const hashkey& b) const noexcept { return hash > b.hash || length > b.length; }
	constexpr bool operator >= (const hashkey& b) const noexcept { return hash >= b.hash || length >= b.length; }
};
INARIKVS_NAMESPACE_END


namespace std {

template<>
struct	hash<inarikvs::hashkey>
{
	uint64_t operator () (const inarikvs::hashkey& v) const
	{
		return	v.compute_hash();
	}
};

}
#endif
