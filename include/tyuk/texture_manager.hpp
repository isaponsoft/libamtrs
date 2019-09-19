#ifndef	__toyoukebime__texture_manager__hpp
#define	__toyoukebime__texture_manager__hpp
#include <amtrs/graphics.hpp>
#include <amtrs/utility.hpp>
#include "tyuk-def.hpp"
#include "singleton_creater.hpp"
TYUK_NAMESPACE_BEGIN


class	texture_manager
		: public singleton<texture_manager, singleton_creator<texture_manager>>
		, public ref_object
{
public:
	ref<g3d::texture> load(const std::string& _filename);



protected:
	struct	cache_texture;

	void remove(cache_texture* _cache);

	std::list<cache_texture*>	mCache;

	template<class>
	friend	struct	singleton_creator;
	friend	struct	cache_texture;
};

// 以下内部実装


struct	texture_manager::cache_texture : public g3d::sub_texture
{
	using	_base_type	= g3d::sub_texture;
	using	_base_type::_base_type;

	virtual ~cache_texture() { texture_manager::get_instance()->remove(this); }
	std::string	name;
};


inline ref<g3d::texture> texture_manager::load(const std::string& _filename)
{
	auto	it	= std::find_if(mCache.begin(), mCache.end(), [&](auto a) -> bool
				{
					return	a->name == _filename;
				});
	if (it != mCache.end())
	{
		return	*it;
	}

	// Open image file.
	auto	in		= filesystem::fileloader::get_instance()->open(_filename);
	if (in.fail())
	{
		AMTRS_DEBUG_LOG("Can't open texture \"%s\".", _filename.c_str());
		throw	std::logic_error(std::string("Can't open"));
	}
    ref<g3d::texture>	tex		= g3d::texture::create(bitmap<rgba<uint8_t>>::load(in));
	ref<cache_texture>	retval	= new cache_texture(tex, {0,0}, tex->size());
	retval->name	= _filename;
	mCache.push_back(retval);
	return	retval;
}

inline void texture_manager::remove(cache_texture* _cache)
{
	mCache.remove(_cache);
}



TYUK_NAMESPACE_END
#endif
