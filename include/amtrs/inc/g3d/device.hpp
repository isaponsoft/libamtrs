/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__device__hpp
#define	__libamtrs__g3d__device__hpp
#include <string>
#include "../../filesystem.hpp"
#include "def.hpp"
#include "attribute.hpp"
#include "draw_command.hpp"
#include "shader.hpp"
#include "texture.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

struct	device_capacity
{
	unsigned int		maxTextureSize			= 1;			//!< テクスチャの最大サイズ
	bool				textureNonPowerOfTwo	= false;		//!< true の時は２の乗数ではないテクスチャをサポートする
};




// ============================================================================
//! レンダラのコンテキストを抽象化したインターフェースです。
// ----------------------------------------------------------------------------
//! OpenGL におけるコンテキストや、Direct3Dなら IDirect3DDeviceNN に相当します。
//! 比較的低レベルなインターフェースを提供します。
// ----------------------------------------------------------------------------
class	device
		: public ref_object
		, protected device_capacity
{
public:
	enum	api_type
	{
		opengl,				//!< OpenGL and OpenGL ES.
		direct3d,			//!< Microsoft Direct3D(DirectX)
		metal,				//!< Apple Metal
		vulkan,				//!< Khronos Group Vulkan
	};

	using	primitive_type	= g3d::primitive_type;

	static constexpr primitive_type	triangles		= primitive_type::triangles;
	static constexpr primitive_type	triangle_strip	= primitive_type::triangle_strip;
	static constexpr primitive_type	triangle_fan	= primitive_type::triangle_fan;

	using	size_type	= size2<unsigned int>;

	using	drawer_type	= void(device*);

	// ============================================================================
	//! ウィンドウにレンダリングするレンダラを作成します。
	// ----------------------------------------------------------------------------
	//static ref<device> create(window* _window);

	// ============================================================================
	//! デバイスの能力を取得します。
	// ----------------------------------------------------------------------------
	const device_capacity* capacity() const noexcept { return this; }


	void set_renderer(std::function<bool()>&& _renderer) { mRenderer = _renderer; }

	// ============================================================================
	//! ビットマップにレンダリングするレンダラを作成します。
	// ----------------------------------------------------------------------------
	//! present() を行ったときに _bitmap で指定する basic_bitmap と互換のある
	//! コンテナへレンダリングした結果を書き込みます。
	// ----------------------------------------------------------------------------
	template<class BitmapT>
	static ref<device> create(BitmapT* _bitmap);


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

	//! 描画内容を更新します。
	//! 必要に応じて、レンダリングターゲットの swap などが行われます。
	virtual void present() = 0;

	//! コンテキストが有効か調べます。
	virtual bool is_context_enable() const = 0;

	// ============================================================================
	//! 頂点シェーダーとフラグメントシェーダー(ピクセルシェーダー)をかけ合わせます。
	// ----------------------------------------------------------------------------
	virtual ref<shader> create_shader(const attribute* _attr, std::size_t _attr_size, const attribute* _uniform, std::size_t _uniform_size, std::string_view _vertex, std::string_view _fragmen) = 0;

	template<std::size_t AN, std::size_t UN>
	ref<shader> create_shader(const attribute (&_attr)[AN], const attribute (&_uniform)[UN], std::string_view _vertex, std::string_view _fragment)
	{
		return	create_shader(_attr, AN, _uniform, UN, _vertex, _fragment);
	}


/*
	virtual ref<vertex_memory> create_vertex_memory(std::size_t _memory_size) = 0;

	template<class VertexT>
	ref<vertex_buffer<VertexT>> create_vertex_buffer(std::size_t _vertex_count)
	{
		auto	t	= create_vertex_memory(sizeof(VertexT) * _vertex_count);
		return	new vertex_buffer<VertexT>(t);
	}
*/

	// ============================================================================
	//! パイプラインを描画します。
	// ----------------------------------------------------------------------------
	virtual void draw(draw_command* _command_list) = 0;


protected:
	device(api_type _api_type)
		: mApiType(_api_type)
	{}

	device_capacity* capacity() noexcept { return this; }

	api_type							mApiType;
	std::function<bool()>				mRenderer;
};

AMTRS_G3D_NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	#include "device-win32.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
	#include "device-android-opengl.hpp"
#endif

#endif
