/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if	0
#ifndef	__libamtrs__g3d__texture__hpp
#define	__libamtrs__g3d__texture__hpp
#include "../graphics.hpp"
AMTRS_G3D_NAMESPACE_BEGIN
class	mapped_texture;


// ============================================================================
//! テクスチャを抽象化したインターフェース。
// ----------------------------------------------------------------------------
class	texture
		: public ref_object
{
public:
	using	memory_type			= memory<vec2<int>, size2<int>>;
	using	mapped_type			= typename memory_type::mapped_type;
	using	offset_type			= typename memory_type::offset_type;
	using	size_type			= typename memory_type::size_type;

	static constexpr unsigned int	norestore	= 1<<0;			//!< テクスチャの復帰バッファを使わないようにします。


	//! 論理テクスチャでのUV座標を物理座標上でのUV座標に変換するための構造体。
	// ------------------------------------------------------------------------
	//! 2x2のマトリックスですが、回転が不要なためこのようになっています。
	struct	texelcoord
	{
		vec2<float>		position	= {0, 0};		//!< 位置
		vec2<float>		scale		= {1, 1};		//!< サイズ
		vec2<float> operator * (vec2<float> _uv) const noexcept
		{
			return	{
				_uv.x * scale.x + position.x,
				_uv.y * scale.y + position.y
			};
		}
	};

	static ref<texture> create(size_type _size, color_format _fmt = color_format::rgba8, unsigned int _flags = 0);

	static ref<texture> create(bitmap<rgba<uint8_t>>&& _bmp, unsigned int _flags = 0);
	static ref<texture> create(bitmap<uint8_t>&& _bmp, unsigned int _flags = 0);

	static ref<texture> create(imagebuff<uint8_t>&& _bmp, unsigned int _flags = 0);

	//! テクスチャのUV変換情報を取得します。
	//! 通常は position = (0,0), scale = (1,1) です。
	//! _physical_size に size_type のポインタを渡すと物理的なテクスチャサイズを格納します。
	virtual texelcoord coordinate() = 0;


	//! 物理テクスチャを返します。
	virtual mapped_texture* mapped() = 0;


	//! テクスチャが失われた時に呼び出すコールバックを設定します。
	void set_lost_callback(std::function<void()>&& _callback) { mLostCallback = std::move(_callback); }


	color_format format() const noexcept { return mFormat; }

	int miplevel() const noexcept { return mMiplevel; }

	offset_type offset() const noexcept { return mOffset; }

	unsigned int flags() const noexcept { return mFlags; }

	//! サイズを取得します。
	size_type size() const noexcept { return mSize; }

protected:
	color_format			mFormat;
	unsigned int			mFlags		= 0;
	int						mMiplevel	= 0;
	offset_type				mOffset		= {0, 0};
	size_type				mSize		= {0, 0};
	std::function<void()>	mLostCallback;
};



// ============================================================================
//! 物理テクスチャ。
// ----------------------------------------------------------------------------
//! GPUのメモリにマッピングされたテクスチャです。
// ----------------------------------------------------------------------------
class	mapped_texture
		: public texture
		, public texture::mapped_type
{
public:
	using	memory_type			= texture::memory_type;
	using	mapped_type			= texture::mapped_type;
	using	offset_type			= typename memory_type::offset_type;
	using	size_type			= typename memory_type::size_type;

	mapped_texture(color_format _format, size_type _size, int _miplevel, unsigned int _flags)
	{
		this->mFormat	= _format;
		this->mFlags	= _flags;
		this->mMiplevel	= _miplevel;
		this->mSize		= _size;
	}


	//! テクスチャのUV変換情報を取得します。
	//! 通常は position = (0,0), scale = (1,1) です。
	//! _physical_size に size_type のポインタを渡すと物理的なテクスチャサイズを格納します。
	virtual texelcoord coordinate() override
	{
		texelcoord		coord;
		if (get_memory())
		{
			const auto	upSize		= vec2<float>(size2<float>(get_memory()->size()));
			const auto	thisSize	= size2<float>(size());
			coord.scale			= vec2<float>(thisSize / upSize);
		}
		return	coord;
	}


	//! 物理テクスチャを返します。
	//! このテクスチャは自身が物理でテクスチャであるため自身を返します。
	virtual mapped_texture* mapped() override { return this; }
};


// ============================================================================
//! テクスチャの一部をテクスチャとして抽象化します。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
class	sub_texture
		: public texture
{
public:
	sub_texture(texture* _uptexture, offset_type _pos, size_type _size)
		: mUptexture(_uptexture)
	{
		this->mOffset	= _pos;
		this->mSize		= _size;
		this->mFormat	= _uptexture->format();
		this->mMiplevel	= 0;
	}


	//! テクスチャのUV変換情報を取得します。
	//! 通常は position = (0,0), scale = (1,1) です。
	//! _physical_size に size_type のポインタを渡すと物理的なテクスチャサイズを格納します。
	virtual texelcoord coordinate() override
	{
		auto		upSize	= vec2<float>(size2<float>(mUptexture->size()));
		texelcoord	retval	= mUptexture->coordinate();
		retval.position	+= vec2<float>(mOffset) / vec2<float>(size2<float>(upSize)) * retval.scale;
		retval.scale	*= vec2<float>(size2<float>(mSize) / upSize);
		return	retval;
	}

	//! 物理テクスチャを返します。
	virtual mapped_texture* mapped() override { return mUptexture->mapped(); }

	texture* upstream() const noexcept { return mUptexture; }

protected:
	ref<texture>	mUptexture;		//! 上流のテクスチャ
};



// ============================================================================
//! テクスチャをアトラス化し、自動的に領域を確保できるようにします。
// ----------------------------------------------------------------------------
//!	example)
//!		atls	= new texture_atls(mapped_texture(bmp));
//!		t1		= atls->allocate({100, 100});
//!		t2		= atls->allocate({30, 30});
// ----------------------------------------------------------------------------
class	texture_atls
		: public sub_texture
{
	using	_base_type	= sub_texture;
public:
	using	offset_type	= _base_type::offset_type;
	using	size_type	= _base_type::size_type;

	texture_atls(texture* _uptexture, offset_type _pos, size_type _size)
		: _base_type(_uptexture, _pos, _size)
		, mAreaManager(_size)
	{}


	ref<sub_texture> allocate(size_type _size)
	{
		auto	r	= mAreaManager.allocate(_size);
		if (r.empty())
		{
			return	{};
		}
		return	new local_texture(this, r.position(), r.size());
	}

private:
	struct	local_texture : sub_texture
	{
		using	sub_texture::sub_texture;
		~local_texture()
		{
			static_cast<texture_atls*>(upstream())->mAreaManager.deallocate(this->offset());
		}
	};


	atls<int>	mAreaManager;

	friend	local_texture;
};


inline ref<texture> texture::create(size_type _size, color_format _fmt, unsigned int _flags)
{
	ref<mapped_texture>	mt;
	size2<int>		ts		= {2, 2};
	while (ts.width  < _size.width ) { ts.width  <<= 1; }
	while (ts.height < _size.height) { ts.height <<= 1; }
	mt	= new mapped_texture(_fmt, {ts.width, ts.height}, 0, _flags);
	return	new sub_texture(mt, {0,0}, {_size.width, _size.height});
}


inline ref<texture> texture::create(bitmap<rgba<uint8_t>>&& _bmp, unsigned int _flags)
{
	ref<mapped_texture>	mt;
	size2<int>		ts		= {2, 2};
	while (ts.width  < _bmp.size().width ) { ts.width  <<= 1; }
	while (ts.height < _bmp.size().height) { ts.height <<= 1; }
	mt	= new mapped_texture(color_format::rgba8, {ts.width, ts.height}, 0, _flags);
	mt->set_translate([bmp = std::move(_bmp)](auto* _mem)
	{
		_mem->transfer({0, 0}, bmp.pixels().data(), bmp.size());
	});
	return	new sub_texture(mt, {0,0}, {_bmp.size().width, _bmp.size().height});
}


inline ref<texture> texture::create(bitmap<uint8_t>&& _bmp, unsigned int _flags)
{
	ref<mapped_texture>	mt;
	size2<int>		ts		= {2, 2};
	while (ts.width  < _bmp.size().width ) { ts.width  <<= 1; }
	while (ts.height < _bmp.size().height) { ts.height <<= 1; }
	mt	= new mapped_texture(color_format::gray8, {ts.width, ts.height}, 0, _flags);
	mt->set_translate([bmp = std::move(_bmp)](auto* _mem)
	{
		_mem->transfer({0, 0}, bmp.pixels().data(), bmp.size());
	});
	return	new sub_texture(mt, {0,0}, {_bmp.size().width, _bmp.size().height});
}


inline ref<texture> texture::create(imagebuff<uint8_t>&& _img, unsigned int _flags)
{
	ref<mapped_texture>	mt;
	size2<int>		ts		= {2, 2};
	while (ts.width  < _img.size().width ) { ts.width  <<= 1; }
	while (ts.height < _img.size().height) { ts.height <<= 1; }
	mt	= new mapped_texture(color_format::gray8, {ts.width, ts.height}, 0, _flags);
	mt->set_translate([img = _img](auto* _mem)
	{
		_mem->transfer({0, 0}, img.data(), img.size());
	});
	return	new sub_texture(mt, {0,0}, {_img.size().width, _img.size().height});
}



inline vec2<float> operator * (const vec2<float>& _v, const texture::texelcoord& _tc)
{
	return	_tc * _v;
}

inline vec2<float>& operator *= (vec2<float>& _v, const texture::texelcoord& _tc)
{
	_v	= _tc * _v;
	return	_v;
}


AMTRS_G3D_NAMESPACE_END
#endif
#endif
