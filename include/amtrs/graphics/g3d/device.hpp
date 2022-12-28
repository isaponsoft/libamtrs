/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__device__hpp
#define	__libamtrs__graphics__g3d__device__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN
using namespace graph;


struct	device_capacity
{
	unsigned int		maxTextureSize			= 1;			//!< テクスチャの最大サイズ
	bool				textureNonPowerOfTwo	= false;		//!< true の時は２の乗数ではないテクスチャをサポートする
};


class	device
		: public ref_object
		, protected device_capacity
{
public:
	using	primitive_type	= g3d::primitive_type;

	static constexpr primitive_type	triangles		= primitive_type::triangles;
	static constexpr primitive_type	triangle_strip	= primitive_type::triangle_strip;
	static constexpr primitive_type	triangle_fan	= primitive_type::triangle_fan;

	using	size_type	= size2<int>;

	using	drawer_type	= void(device*);
	using	api_type	= g3d::api_type;

	struct	createparams
	{
		class window*		window	= nullptr;
		api_type			api		= api_type::api_vulkan;
	};

	static ref<device> create(createparams& _cp);

	// ============================================================================
	//! デバイスの能力を取得します。
	// ----------------------------------------------------------------------------
	const device_capacity* capacity() const noexcept { return this; }


	void set_renderer(std::function<bool()>&& _renderer) { mRenderer = _renderer; }


	//! APIの種類を取得します。
	api_type apitype() const noexcept { return mApiType; }

	//! レンダラの名前を取得します。
	virtual std::string name() const = 0;

	//! コンテキストの大きさを取得します。
	virtual size_type size() const = 0;

	//! コンテキストをアクティブ化します。
	//! wglMakeCurrent(dc, context) などを呼び出します。
	virtual void active() = 0;

	//! コンテキストを非アクティブ化します。
	//! wglMakeCurrent(dc, nullptr) などを呼び出します。
	virtual void deactive() = 0;

	//! 描画コマンドを実行します。
	//! 必要に応じて、レンダリングターゲットの swap などが行われます。
	virtual void execute(ref<commandlist> _commands) {}

	//! 描画内容を更新します。
	//! 必要に応じて、レンダリングターゲットの swap などが行われます。
	virtual void present() = 0;

	//! コンテキストが有効か調べます。
	virtual bool is_context_enable() const = 0;

	//! デバイスをリセットします。
	virtual void reset() = 0;

protected:
	device(api_type _api_type)
		: mApiType(_api_type)
	{}

	device_capacity* capacity() noexcept { return this; }

	api_type							mApiType;
	std::function<bool()>				mRenderer;
};


AMTRS_G3D_NAMESPACE_END
#endif
