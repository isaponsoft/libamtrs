/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/g3d/@>
#include <amtrs/graphics/@>
#include <amtrs/utility/@>
#include <gframe/texture_manager.hpp>
GFRAME_NAMESPACE_BEGIN
using namespace amtrs;


class	texture_manager
		: public singleton<texture_manager>
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
	try
	{
		auto	in		= filesystem::fileloader::get_instance()->open(_filename);
		if (!in.good())
		{
			AMTRS_WARN_LOG("Can't open texture \"%s\".", _filename.c_str());
			return	{};
		}
		//AMTRS_DEBUG_LOG("open texture \"%s\".", _filename.c_str());
		ref<g3d::texture>	tex		= g3d::texture::create(bitmap<rgba<uint8_t>>::load(in));
		ref<cache_texture>	retval	= new cache_texture(tex, {0,0}, tex->size());
		retval->name	= _filename;
		mCache.push_back(retval);
		return	retval;
	}
	catch (...)
	{
		AMTRS_ERROR_LOG("Exception in open texture \"%s\".", _filename.c_str());
		return	{};
	}
}

inline void texture_manager::remove(cache_texture* _cache)
{
	mCache.remove(_cache);
}


amtrs::ref<amtrs::g3d::texture> load_texture(std::string const& _filename)
{
	return	gframe::texture_manager::get_instance()->load(_filename);
}



GFRAME_NAMESPACE_END
