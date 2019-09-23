/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__device__android__opengl__hpp
#define	__libamtrs__g3d__device__android__opengl__hpp
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "device-opengl.hpp"
#define	AMTRS_ANDROID_OPENGL_NAMESPACE_BEGIN	AMTRS_G3D_NAMESPACE_BEGIN namespace android { namespace opengl {
#define	AMTRS_ANDROID_OPENGL_NAMESPACE_END	} } AMTRS_G3D_NAMESPACE_END
AMTRS_ANDROID_OPENGL_NAMESPACE_BEGIN

// ============================================================================
//! OpenGL のコンテキストを抽象化したWin32版の共通インターフェース。
// ----------------------------------------------------------------------------
class	device
		: public g3d::opengl::device
{
	using	_base_type	= g3d::opengl::device;
public:
	device() = default;
	~device() = default;

	using	_base_type::on_gain;
	using	_base_type::on_lost;



	//! コンテキストをアクティブ化します。
	virtual void active() override
	{
	}

	//! コンテキストを非アクティブ化します。
	virtual void deactive() override
	{
	}

	//! コンテキストの大きさを取得します。
	virtual size_type size() const override
	{
		return	mSurfaceSize;
	}

	virtual void present() override
	{
		if (is_context_enable())
		{
			glFlush();
		}
		_base_type::present();
	}

	size_type						mSurfaceSize	= {0, 0};

	friend	amtrs::g3d::device;
};
AMTRS_ANDROID_OPENGL_NAMESPACE_END
AMTRS_G3D_NAMESPACE_BEGIN

template<class BitmapT>
inline ref<device> device::create(BitmapT* _bitmap)
{
	return	nullptr;
}

AMTRS_G3D_NAMESPACE_END
#endif
