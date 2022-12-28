/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__device__hpp
#define	__libamtrs__g3d__device__hpp
#include <deque>
#include <typeindex>
#include "../window.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

struct	device_capacity
{
	unsigned int		maxTextureSize			= 1;			//!< テクスチャの最大サイズ
	bool				textureNonPowerOfTwo	= false;		//!< true の時は２の乗数ではないテクスチャをサポートする
};


class	commandlist;

 enum class	api_type
{
	api_auto,
	api_opengles_2_0,
	api_vulkan,
	api_metal,
	api_direct3d11,
	api_direct3d12,
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
	using	primitive_type	= g3d::primitive_type;

	static constexpr primitive_type	triangles		= primitive_type::triangles;
	static constexpr primitive_type	triangle_strip	= primitive_type::triangle_strip;
	static constexpr primitive_type	triangle_fan	= primitive_type::triangle_fan;

	using	size_type	= size2<int>;

	using	drawer_type	= void(device*);

	// ============================================================================
	//! ウィンドウにレンダリングするレンダラを作成します。
	// ----------------------------------------------------------------------------
	static ref<device> create(api_type _type, window* _window);

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


protected:
	device(api_type _api_type)
		: mApiType(_api_type)
	{}

	device_capacity* capacity() noexcept { return this; }

	api_type							mApiType;
	std::function<bool()>				mRenderer;
};


class	device;
class	constants;
class	material;
class	uniformset;

enum class	shader_stage
{
	none_stage			= -1,		//!< not initialized
	vertex_stage		= 0,
	tesslation_stage,
	geometory_stage,
	fragment_stage,
	compute_stage,
	_max
};


template<class T>
struct	rangedata
{
	T*		__data;
	size_t	__size;

	T* data() const noexcept { return __data; }
	size_t size() const noexcept { return __size; }

	rangedata() = default;
	rangedata(rangedata const&) = default;
	rangedata& operator = (rangedata const&) = default;

	rangedata(T* _data, size_t _size)
		: __data(_data)
		, __size(_size)
	{}

	rangedata(std::initializer_list<T> _data)
		: __data(&*_data.begin())
		, __size(std::distance(_data.begin(), _data.end()))
	{}

	template<size_t N>
	rangedata(T (&_data)[N])
		: __data(&_data[0])
		, __size(N)
	{}

	auto begin() const noexcept { return data(); }
	auto end() const noexcept { return data() + size(); }
};


//! 
inline size_t stride(g3d::attribute const& _attr)
{
	return	(_attr.type == g3d::element_type::texture2d_value)		? sizeof(void*)
		:	(_attr.type == g3d::element_type::float_value)			? sizeof(float) * _attr.count
		:	0;
}

//! 全体の大きさを計算します。
inline size_t stride(rangedata<g3d::attribute const> const& _range)
{
	size_t	s	= 0;
	for (auto& a : _range)
	{
		s	+=	stride(a);
	}
	return	s;
}

enum class	format_types
{
	constants_value	= 0,
	texture_value	= 1,
};





template<class T, class R>
struct	realizer
{
	using	instance_type	= typename T::instance_type;
	using	realized_type	= R;

	template<class... Args>
	ref<realized_type> operator () (T& _obj, Args&&... _args)
	{
		if (_obj.mInstance.empty())
		{
			return	{};
		}
		if (_obj.mInstance->realized)
		{
			return	static_cast<R*>(_obj.mInstance->realized.get());
		}
		ref<realized_type>	r	= new realized_type();
		r->initialize(_obj.mInstance.get(), std::forward<Args>(_args)...);
		_obj.mInstance->realized	= r;
		return	r;
	}
};

template<class Traits, class T, class... Args>
auto realizing(T& _obj, Args&&... _args)
{
	using	realizer_type	= realizer<T, Traits>;
	realizer_type	r;
	return	r(_obj, std::forward<Args>(_args)...);
}




//! shader module
class	shadermodule
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		shader_stage		stage		= shader_stage::none_stage;
	};

	shadermodule() = default;
	shadermodule(shadermodule const&) = default;
	shadermodule(shadermodule&&) = default;
	shadermodule& operator = (shadermodule const&) = default;
	shadermodule& operator = (shadermodule&&) = default;


	/*!
	 * ステージとリソースを指定して初期化します。
	 * ----------------------------------------------------------------------
	 * シェーダーを作成します。ただし、シェーダーのコンパイルやロードは
	 * グラフィックスエンジン側の描画シーケンスの中で行われます。
	 * ----------------------------------------------------------------------
	 * _stage
	 *		シェーダーが対応するステージ。
	 * 		materialに設定時にこの値がデフォルト値として使用されます。
	 * _resource
	 *		グラフィックスエンジンがシェーダーをコンパイル／ロードするさいに
	 *		呼び出すコールバック。
	 *		void(void const* _data, size_t _dataSize) の関数オブジェクトが
	 *		引数に渡されます。
	 */
	shadermodule(shader_stage _stage);

	shader_stage stage() const noexcept
	{
		return	mInstance->stage;
	}

	bool empty() const noexcept { return mInstance.empty(); }
private:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


//! パイプラインで使用するデータのレイアウトを示します。
class	material_layout
{
public:
	struct	layoutdata
			: rangedata<g3d::attribute const>
	{
		using	range_type	= rangedata<g3d::attribute const>;

		shader_stage			stage;
		format_types			type;

		layoutdata() = default;
		layoutdata(layoutdata const&) = default;
		layoutdata& operator = (layoutdata const&) = default;

		layoutdata(shader_stage _stage, format_types _type, range_type _attributes)
			: range_type(std::move(_attributes))
			, stage(_stage)
			, type(_type)
		{}

		layoutdata(shader_stage _stage, range_type _attributes)
			: range_type(std::move(_attributes))
			, stage(_stage)
			, type(format_types::constants_value)
		{}

		layoutdata(shader_stage _stage, format_types _type)
			: range_type(nullptr, 0)
			, stage(_stage)
			, type(_type)
		{}
	};

	struct	instance_type : ref_object
	{
		ref<ref_object>						realized;
		std::vector<g3d::attribute>			attributes;
		rangedata<g3d::attribute const>		input;
		std::vector<layoutdata>				uniforms;
	};

	material_layout() = default;
	material_layout(material_layout const&) = default;
	material_layout(material_layout&&) = default;
	material_layout& operator = (material_layout const&) = default;
	material_layout& operator = (material_layout&&) = default;


	material_layout(rangedata<g3d::attribute const> _input, rangedata<layoutdata const> _uniforms = {nullptr, 0});

	rangedata<g3d::attribute const> inputformat() const noexcept
	{
		return	mInstance->input;
	}

	layoutdata const* uniform(shader_stage _stage) const noexcept
	{
		for (auto& u : mInstance->uniforms)
		{
			if (u.stage == _stage)
			{
				return	&u;
			}
		}
		return	nullptr;
	}

	auto uniforms() const noexcept
	{
		return	make_bucket(mInstance->uniforms.begin(), mInstance->uniforms.end());
	}

	//! すべてのUniformデータを合算した大きさを計算します。
	size_t unioforms_stride() const noexcept
	{
		size_t	s	= 0;
		for (auto& u : uniforms())
		{
			s	+= stride(u);
		}
		return	s;
	}

	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};




//! テクスチャを示します。
class	textureset
{
public:
	textureset() = default;
	textureset(textureset const&) = default;
	textureset(textureset&&) = default;
	textureset& operator = (textureset const&) = default;
	textureset& operator = (textureset&&) = default;

	enum class	format_types
	{
		rgba_u8		= 1,		//!< { u8, u8, u8, u8 } 
		rgba_f32	= 2,		//!< { float, float, float, float }
		f16			= 2,		//!< { float }
		f32			= 3,		//!< { float }
		s16			= 4,		//!< { s16 }
		u8			= 5,		//!< { u8 }
	};

	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		size2<unsigned int>	size;
		format_types		format;		
		uint32_t			usage;
	};

	static constexpr uint32_t		usage_depth		= 1<<0;
	static constexpr uint32_t		usage_stencil	= 1<<1;

	static constexpr format_types	rgba_u8			= format_types::rgba_u8;
	static constexpr format_types	rgba_f32		= format_types::rgba_f32;
	static constexpr format_types	f16				= format_types::f16;
	static constexpr format_types	f32				= format_types::f32;
	static constexpr format_types	s16				= format_types::s16;
	static constexpr format_types	u8				= format_types::u8;

	// ========================================================================
	//! _size
	//!		usage に depth, stencil が指定されている場合、size に {0, 0}
	//!		を指定するとフレームバッファの大きさに合わせて自動的に調整されます。
	textureset(size2<unsigned int> _size, format_types _fmt = format_types::rgba_u8, uint32_t _usage = 0U);

	size2<unsigned int> const& size() const noexcept
	{
		return	mInstance->size;
	}

	format_types format() const noexcept
	{
		return	mInstance->format;
	}

	uint32_t usage() const noexcept
	{
		return	mInstance->usage;
	}

	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


//! 書き込み先のVRAMを指定します。
//! カラーバッファ、深度バッファ、Zバッファの組み合わせです。
class	framebuffer
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>	realized;
		textureset		color;
		textureset		depth;
		textureset		stencil;
	};

	//! _color に nullptr を指定した場合、描画先としてスワップチェーンに関連付けされている
	//! カラーバッファ（≒ウィンドウ）を対象とするようにドライバに伝えます。
	//! 1枚のテクスチャでdepth と stencil を補えるデバイスがありますがその場合は
	//! depthとstencilに同じテクスチャを指定してください。
	framebuffer(textureset _color, textureset _depth, textureset _stencil);

	framebuffer() = default;
	framebuffer(framebuffer const&) = default;
	framebuffer(framebuffer&&) = default;
	framebuffer& operator = (framebuffer const&) = default;
	framebuffer& operator = (framebuffer&&) = default;

	textureset const& color() const noexcept { return mInstance->color; }
	textureset const& depth() const noexcept { return mInstance->depth; }
	textureset const& stencil() const noexcept { return mInstance->stencil; }

	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


//! テクスチャのサンプリング方法を指定します。
class	imagesampler
{
public:
	enum class	filter_modes
	{
		nearest	= 0,
		liner	= 1,
	};

	enum class	access_modes
	{
		repeat	= 0,
		mirror_repeat,
		clamp_edge,
		clamp_border,
	};

	static constexpr filter_modes	filter_nearest	= filter_modes::nearest;
	static constexpr filter_modes	filter_liner	= filter_modes::liner;
	static constexpr access_modes	access_repeat	= access_modes::repeat;
	static constexpr access_modes	access_mirror	= access_modes::mirror_repeat;
	static constexpr access_modes	access_edge		= access_modes::clamp_edge;
	static constexpr access_modes	access_border	= access_modes::clamp_border;

	struct	samplerdata
	{
		filter_modes	filter	= filter_nearest;
		access_modes	access	= access_repeat;
	};

	struct	instance_type : ref_object
	{
		ref<ref_object>	realized;
		samplerdata		data;
	};


	imagesampler() = default;
	imagesampler(imagesampler const&) = default;
	imagesampler(imagesampler&&) = default;
	imagesampler& operator = (imagesampler const&) = default;
	imagesampler& operator = (imagesampler&&) = default;

	imagesampler(samplerdata _data);

	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};



// ****************************************************************************
//! 定数バッファ
// ----------------------------------------------------------------------------
//! uniformsetにbindして使用できます。
// ----------------------------------------------------------------------------
class	constants
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>							realized;
		material_layout							layout;
		uint32_t								index;
	};

	// ========================================================================
	//! レイアウトに対応する定数バッファを生成します。
	// ------------------------------------------------------------------------
	constants(material_layout _layot, uint32_t _index);

	material_layout const& format() const noexcept { return mInstance->layout; }

	bool empty() const noexcept { return mInstance.empty(); }

	constants() = default;
	constants(constants const&) = default;
	constants(constants&&) = default;
	constants& operator = (constants const&) = default;
	constants& operator = (constants&&) = default;
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};



// ****************************************************************************
//! マテリアル
// ----------------------------------------------------------------------------
//! シェーダーと入力データのレイアウトを表します。
//! マテリアルの内容が変更されると描画パイプラリンが再構築されるためロスタイムが
//! 発生します。
// ----------------------------------------------------------------------------
class	material
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>	realized;
		material_layout	layout;
		shadermodule	modules[(size_t)shader_stage::_max];
	};

	material(material_layout _format, std::initializer_list<shadermodule> _shaders);

	shadermodule const& shader(shader_stage _stage) const noexcept { return mInstance->modules[(int)_stage]; }

	material_layout const& layout() const noexcept { return mInstance->layout; }

	operator material_layout const& () const noexcept { return layout(); }

	bool empty() const noexcept { return mInstance.empty(); }

	material() = default;
	material(material const&) = default;
	material(material&&) = default;
	material& operator = (material const&) = default;
	material& operator = (material&&) = default;
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};



// ****************************************************************************
//! 頂点バッファ
// ----------------------------------------------------------------------------
class	vertexbuffer
{
public:
	//! ドライバーが実体を保持するための構造体
	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		size_t				size;
	};

	vertexbuffer() = default;
	vertexbuffer(vertexbuffer const&) = default;
	vertexbuffer(vertexbuffer&&) = default;
	vertexbuffer& operator = (vertexbuffer const&) = default;
	vertexbuffer& operator = (vertexbuffer&&) = default;


	vertexbuffer(size_t _size);

	//! バッファのソースデータのサイズ取得します。
	auto size() const noexcept { return mInstance->size; }

	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};



// ****************************************************************************
//! Uniformデータを保持します。
// ----------------------------------------------------------------------------
//! layout に従って定数データやテクスチャ（サンプラ）を保持します。
//! commandlistにこの値をbindすることでUniformを設定できます。
//! g3dエンジンはuniformオブジェクトを通すことでパイプラインの
//! 最適化を行うことができます。
// ----------------------------------------------------------------------------
class	uniformset
{
public:
	struct	instance_type
			: ref_object
	{
		struct	binddata : ref_object{};
		struct	constants_data : binddata
		{
			constants			val1;
		};
		struct	tetxure_data : binddata
		{
			textureset		val1;
			imagesampler		val2;
		};

		material_layout					layout;
		bool							modified	= true;
		shared_buffer<ref<binddata>>	binds;
		ref<ref_object>					realized;
	};


	uniformset() = default;
	uniformset(uniformset const&) = default;
	uniformset(uniformset&&) = default;
	uniformset& operator = (uniformset const&) = default;
	uniformset& operator = (uniformset&&) = default;

	//! レイアウトを指定して作成します。
	uniformset(material_layout _layout);

	//! 定数データをバインドします。
	uniformset& bind(uint32_t _index, constants _uniform);

	//! テクスチャとサンプラをバインドします。
	uniformset& bind(uint32_t _index, textureset _texture, imagesampler _sampler);

	material_layout const& layout() const noexcept { return mInstance->layout; }
	operator material_layout const& () const noexcept { return layout(); }


	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};




// ****************************************************************************
//! レンダリングを行う時の描画方法を指定します。
// ----------------------------------------------------------------------------
//! 深度バッファの比較方法やカラーブレンドの方法などを指定します。
// ----------------------------------------------------------------------------
class	renderstate
{
public:
	//! カリング
	enum class	culling_modes
	{
		none	= 0,
		front	= 1,
		back	= 2,
	};
	static constexpr culling_modes	cull_mode_none	= culling_modes::none;
	static constexpr culling_modes	cull_mode_front	= culling_modes::front;
	static constexpr culling_modes	cull_mode_back	= culling_modes::back;


	//! 比較方法
	enum class	compare_modes
	{
		never	= 0,			//!< 常に不合格
		always,					//!< 常に合格
		equal,
		not_equal,
		less,
		less_or_equal,
		grater,
		grater_or_equal,
	};
	static constexpr compare_modes	compare_never			= compare_modes::never;
	static constexpr compare_modes	compare_always			= compare_modes::always;
	static constexpr compare_modes	compare_equal			= compare_modes::equal;
	static constexpr compare_modes	compare_not_equal		= compare_modes::not_equal;
	static constexpr compare_modes	compare_less			= compare_modes::less;
	static constexpr compare_modes	compare_less_or_equal	= compare_modes::less_or_equal;
	static constexpr compare_modes	compare_grater			= compare_modes::grater;
	static constexpr compare_modes	compare_grater_or_equal	= compare_modes::grater_or_equal;

	//! ステンシルOp
	enum class	stensil_operators
	{
		keep		= 0,
		zero,
		replace,
		invert,
		inc_and_clamp,
		inc_and_wrap,
		dec_and_clamp,
		dec_and_wrap,
	};
	static constexpr stensil_operators	stencil_op_keep				= stensil_operators::keep;
	static constexpr stensil_operators	stencil_op_zero				= stensil_operators::zero;
	static constexpr stensil_operators	stencil_op_replace			= stensil_operators::replace;
	static constexpr stensil_operators	stencil_op_invert			= stensil_operators::invert;
	static constexpr stensil_operators	stencil_op_inc_and_clamp	= stensil_operators::inc_and_clamp;
	static constexpr stensil_operators	stencil_op_inc_and_wrap		= stensil_operators::inc_and_wrap;
	static constexpr stensil_operators	stencil_op_dec_and_clamp	= stensil_operators::dec_and_clamp;
	static constexpr stensil_operators	stencil_op_dec_and_wrap		= stensil_operators::dec_and_wrap;


	//! depth test
	struct	depth_test
	{
		bool			enable	= true;
		bool			write	= true;
		compare_modes	compare	= compare_modes::less;
	};


	//! stencil test
	struct	stencil_test
	{
		bool				enable		= false;
		compare_modes		compare		= compare_modes::always;
		stensil_operators	stencil_fail= stensil_operators::keep;
		stensil_operators	depth_fail	= stensil_operators::keep;
		stensil_operators	depth_pass	= stensil_operators::keep;
		uint32_t			compareMask	= 0xff;
		uint32_t			writeMask	= 0xff;
		uint32_t			reference	= 1;
	};



	struct	status
	{
		culling_modes	culling	= cull_mode_none;
		depth_test		depth;
		stencil_test	stencil;
	};

	//! ドライバーが実体を保持するための構造体
	struct	instance_type : ref_object, status
	{
		instance_type(status&& _s)
			: status(std::move(_s))
		{}

		ref<ref_object>		realized;
	};


	renderstate() = default;
	renderstate(renderstate const&) = default;
	renderstate(renderstate&&) = default;
	renderstate& operator = (renderstate const&) = default;
	renderstate& operator = (renderstate&&) = default;

	//! _depth
	//!		深度テストの方法を指定します。
	//!		nullptr を指定した場合、 enable = false を指定したのと
	//!		同じ扱いになります。
	renderstate(status&& _status)
	{
		mInstance	= new instance_type(std::move(_status));
	}

	renderstate& operator = (status _s)
	{
		mInstance	= new instance_type(std::move(_s));
		return	*this;
	}


	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


//! レンダリングパイプライン
class	pipeline
{
public:
	template<class Traits>
	using	realize_type	= typename Traits::pipeline_type;

	//! ドライバーが実体を保持するための構造体
	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		renderstate			rs;
		material			mat;
	};


	pipeline() = default;
	pipeline(pipeline const&) = default;
	pipeline(pipeline&&) = default;
	pipeline& operator = (pipeline const&) = default;
	pipeline& operator = (pipeline&&) = default;

	//! _rs
	//!		empty() を渡した場合、直前に指定されたものを
	//!		再利用します。
	pipeline(material _material, renderstate const& _rs);


	material const& mat() const noexcept { return mInstance->mat; }
	material_layout const& layout() const noexcept { return mat(); }

	operator material const& () const noexcept { return mat(); }
	operator material_layout const& () const noexcept { return layout(); }


	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


class	commandexecuter;
class	cmdlist_pipeline;


struct	viewport
		: rect<float>
{
	using	super_type	= rect<float>;

	using	super_type::super_type;

	viewport(viewport const&) = default;

	viewport(float _x, float _y, float _w, float _h, float _near, float _far)
		: super_type(_x, _y, _w, _h)
		, near_z(_near)
		, far_z(_far)
	{}

	viewport(super_type const& _r, float _near, float _far)
		: super_type(_r)
		, near_z(_near)
		, far_z(_far)
	{}

	float			near_z;
	float			far_z;
};


struct	scissor
		: rect<int>
{
	using	rect::rect;
};



class	commandlist
		: public ref_object
{
	struct	subcommand
	{
		ref<commandlist>	cmd;
	};
public:
	static constexpr size_t		allocate_size	= 2048;
	static constexpr size_t		align_size		= sizeof(void*) * 2;

	struct	bufferstate
	{
		int		usingsize	= 0;
	};

	using	commandbuff			= shared_buffer<char, bufferstate>;

	commandlist()
	{
		create_buffer();
	}

	~commandlist()
	{
		clear();
	}

	//! 記録されているコマンドを実行します。
	//! 通常、デバイスドライバから呼ばれます。
	void execute(commandexecuter& _exec);

	//! 記録されているコマンドをクリアします。
	void clear();


	void sub(ref<commandlist> _cmd);

	template<class T>
	ref<T> sub();

	void update(constants _target, shared_buffer<void> _data);

	template<class T>
	void update(constants _target, T const& _data);

	void update(vertexbuffer _target, shared_buffer<void> _data);

	void update(textureset _target, shared_buffer<void> _data);

	void update(shadermodule _target, shared_buffer<void> _data);

protected:
	void pipeline(class pipeline _pipeline);

	void renderstate_push(class renderstate _state);
	void renderstate_pop();

	//! 現在のパイプラインに定数データをバインドします。
	void bind(uint32_t _index, constants _uniform);

	//! 現在のパイプラインにテクスチャとサンプラをバインドします。
	void bind(uint32_t _index, textureset _resource, imagesampler _sampler);

	void bind(uniformset _uniform);

	void draw(vertexbuffer _target, uint32_t _offset, uint32_t _count);

	//! ビューポートとシザーを設定します。
	//! nullptrを渡した場合は設定済みの値をそのまま使用します。
	void viewport_scissor(viewport const* _viewport, scissor const* _scissor);

	void renderpass_begin(framebuffer _target, rgba<float> _clearColor, float _depth = 1, uint32_t _stencil = 0);

	void renderpass_end();

	//! コマンドの記録の開始を行います。
	void begin();

	//! コマンドの記録の終了し、コマンドの処理を行います。
	void end();

	//! 画面へ反映さえます。
	void present();

private:
	template<class T>
	T* regist(T _data);

	template<class T>
	size_t align()
	{
		return	(sizeof(std::type_index) + (sizeof(T) + align_size - 1) / align_size * align_size);
	}

	commandbuff* create_buffer()
	{
		mBuffer.push_back(commandbuff(allocate_size, bufferstate{ .usingsize = 0 }));
		return	&mBuffer.back();
	}

	std::deque<commandbuff>		mBuffer;

	friend	cmdlist_pipeline;

};


class	cmdlist_pipeline
		: public commandlist
{
public:
	using	commandlist::bind;
	using	commandlist::draw;
	using	commandlist::viewport_scissor;
	using	commandlist::renderstate_push;
	using	commandlist::renderstate_pop;

	cmdlist_pipeline() = default;

	cmdlist_pipeline(commandlist* _parent, class pipeline _pipe)
	{
		_parent->pipeline(std::move(_pipe));
		_parent->sub(this);
	}

};


class	cmdlist_renderpass
		: public commandlist
{
public:
	using	commandlist::viewport_scissor;
	using	commandlist::renderstate_push;
	using	commandlist::renderstate_pop;

	ref<cmdlist_pipeline> pipeline(class pipeline _pipe)
	{
		return	new cmdlist_pipeline(this, std::move(_pipe));
	}
};


class	cmdlist_root
		: public commandlist
{
public:
	ref<cmdlist_renderpass> renderpass(framebuffer _target, rgba<float> _clearColor, float _depth = 1, uint32_t _stencil = 0)
	{
		ref<cmdlist_renderpass>	retval	= new cmdlist_renderpass();
		commandlist::renderpass_begin(std::move(_target), std::move(_clearColor), _depth, _stencil);
		commandlist::sub(retval);
		commandlist::renderpass_end();
		return	retval;
	}
};


class	cmdlist_presenter
		: public commandlist
{
public:
	using	commandlist::present;

	ref<cmdlist_root> root()
	{
		ref<cmdlist_root>	retval	= new cmdlist_root();
		commandlist::begin();
		commandlist::sub(retval);
		commandlist::end();
		return	retval;
	}
};




class	commandexecuter
{
public:
	//! Uniform データの更新を行います。
	struct	constants_update
	{
		constants			target;
		shared_buffer<void>	source;
	};

	//! Uniform データの更新を行います。
	struct	vertexbuffer_update
	{
		vertexbuffer		target;
		shared_buffer<void>	source;
	};

	//! textureset データの更新を行います。
	struct	textureset_update
	{
		textureset			target;
		shared_buffer<void>	source;
	};

	//! shadermodule の更新を行います。
	struct	shadermodule_update
	{
		shadermodule		target;
		shared_buffer<void>	source;
	};

	//! デフォルトのレンダリングステータスをPushします。
	struct	renderstatus_push
	{
		renderstate			state;		// empty() の場合は pop します。
	};

	//! レンダリングパスを開始します。
	struct	renderpass_begin
	{
		framebuffer	target;
		rgba<float>			color;
		float				depth;
		uint32_t			stencil;
	};

	//! レンダリングパスを終了します。
	struct	renderpass_end
	{
	};

	//! パイプラインを変更します。
	struct	pipeline_change
	{
		pipeline			pipe;
	};

	//! テクスチャをカレントパイプラインに設定します。
	struct	uniform_bind
	{
		uniformset			u;
	};

	//! 頂点データを描画します。
	//! インデックス情報を使用する場合は draw_inddicis を使用してください。
	struct	draw_vertex
	{
		vertexbuffer		target;
		uint32_t			offset;
		uint32_t			count;
	};

	struct	viewport_scissor
	{
		uint32_t			flags;		// 0x01: viewport, 0x02 : scissor
		struct viewport		viewport;
		struct scissor		scissor;
	};

	struct	begin
	{
	};


	struct	end
	{
	};


	//! 画面に対してレンダリング結果を反映させます。
	struct	present
	{
	};



	virtual void exec(constants_update& _data) = 0;
	virtual void exec(vertexbuffer_update& _data) = 0;
	virtual void exec(textureset_update& _data) = 0;
	virtual void exec(shadermodule_update& _data) = 0;
	virtual void exec(renderstatus_push& _data) = 0;
	virtual void exec(renderpass_begin& _data) = 0;
	virtual void exec(renderpass_end& _data) = 0;
	virtual void exec(pipeline_change& _data) = 0;
	virtual void exec(viewport_scissor& _data) = 0;
	virtual void exec(uniform_bind& _data) = 0;
	virtual void exec(draw_vertex& _data) = 0;
	virtual void exec(begin& _data) = 0;
	virtual void exec(end& _data) = 0;
	virtual void exec(present& _data) = 0;

};




inline void commandlist::sub(ref<commandlist> _cmd)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_cmd.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::sub(_cmd) : _cmd is empty");
	}
	#endif
	regist<subcommand>({
		.cmd	= std::move(_cmd),
	});
}

template<class T>
inline ref<T> commandlist::sub()
{
	ref<T>	retval	= new T();
	sub(retval);
	return	retval;
}

inline void commandlist::update(constants _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::constants_update>({
		.target	= std::move(_target),
		.source	= std::move(_data)
	});
}

template<class T>
inline void commandlist::update(constants _target, T const& _data)
{
	update(std::move(_target), {&_data, 1});
}


inline void commandlist::update(vertexbuffer _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::vertexbuffer_update>({
		.target	= std::move(_target),
		.source	= std::move(_data)
	});
}


inline void commandlist::update(textureset _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::textureset_update>({
		.target	= std::move(_target),
		.source	= std::move(_data)
	});
}


inline void commandlist::update(shadermodule _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::shadermodule_update>({
		.target	= std::move(_target),
		.source	= std::move(_data)
	});
}


inline void commandlist::viewport_scissor(viewport const* _viewport, scissor const* _scissor)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (!_viewport && !_scissor)
	{
		AMTRS_DEBUG_LOG("commandlist::viewport_scissor(_viewport, _scissor) : _viewport and _scissor is null");
	}
	#endif
	regist<commandexecuter::viewport_scissor>({
		.flags		= (_viewport ? 0x01U : 0x00U) | (_scissor ? 0x02U : 0x00U),
		.viewport	= _viewport ? *_viewport : viewport{},
		.scissor	= _scissor  ? *_scissor  : scissor{}
	});
}


inline void commandlist::renderstate_push(class renderstate _state)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_state.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::renderstate_push(_state) : _state is empty");
	}
	#endif
	regist<commandexecuter::renderstatus_push>({
		.state	= std::move(_state)
	});
}


inline void commandlist::renderstate_pop()
{
	regist<commandexecuter::renderstatus_push>({
		.state	= renderstate()
	});
}

inline void commandlist::pipeline(class pipeline _pipeline)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_pipeline.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::pipeline(_pipeline) : _pipeline is empty");
	}
	#endif
	regist<commandexecuter::pipeline_change>({
		.pipe	= std::move(_pipeline)
	});
}


inline void commandlist::bind(uniformset _uniform)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_uniform.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::bind(_uniform) : _uniform is empty");
	}
	#endif
	regist<commandexecuter::uniform_bind>({
		.u			= std::move(_uniform),
	});
}

inline void commandlist::draw(vertexbuffer _target, uint32_t _offset, uint32_t _count)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::draw(_target) : _target is empty");
	}
	#endif
	regist<commandexecuter::draw_vertex>({
		.target	= std::move(_target),
		.offset	= _offset,
		.count	= _count
	});
}

inline void commandlist::renderpass_begin(framebuffer _target, rgba<float> _clearColor, float _depth, uint32_t _stencil)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::renderpass_begin(_target) : _target is empty");
	}
	#endif
	regist<commandexecuter::renderpass_begin>({
		.target		= std::move(_target),
		.color		= _clearColor,
		.depth		= _depth,
		.stencil	= _stencil
	});
}

inline void commandlist::renderpass_end()
{
	regist<commandexecuter::renderpass_end>({
	});
}

inline void commandlist::begin()
{
	regist<commandexecuter::begin>({
	});
}

inline void commandlist::end()
{
	regist<commandexecuter::end>({
	});
}

inline void commandlist::present()
{
	regist<commandexecuter::present>({
	});
}


template<class T>
T* commandlist::regist(T _data)
{
	auto*	current		= &mBuffer.back();
	int		capacity	= static_cast<int>(current->size()) - current->header().usingsize;
	if (capacity < (sizeof(std::type_index) + sizeof(T)))
	{
		current	= create_buffer();
	}
	char*	buff		= current->data() + current->header().usingsize;
	new (buff) std::type_index(typeid(T));
	buff += sizeof(std::type_index);
	new (buff) T(std::move(_data));
	current->header().usingsize	+= static_cast<int>(align<T>());
	return	reinterpret_cast<T*>(buff);
}


AMTRS_G3D_NAMESPACE_END
#endif
