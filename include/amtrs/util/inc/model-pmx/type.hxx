/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__util__model_pmx__types__hpp
#define	__libamtrs__util__model_pmx__types__hpp
#include "../string/sstring.hpp"
#include "namespace.hxx"
AMTRS_MODEL_PMX_NAMESPACE_BEGIN

// ****************************************************************************
//	.pmx file format parse utility.
// ----------------------------------------------------------------------------
//	format reference:
//		https://gist.github.com/ulrikdamm/8274171
// ****************************************************************************
using	vec2		= graph::vec2<float>;
using	vec3		= graph::vec3<float>;
using	vec4		= graph::vec4<float>;
using	string_type	= sstring;


struct	pmxdata;


enum class	text_encoding_type
{
	utf16	= 0,
	utf8	= 1,
};

enum class	index_size_type
{
	int8	= 1,
	int16	= 2,
	int32	= 4,
};

enum		render_flags
{
	double_side			= 1<<0,			//!< 両面
	ground_shadow		= 1<<1,			//!< 地面への影
	self_shadowmap		= 1<<2,			//!< セルフシャドウマップ
	self_shadow			= 1<<3,			//!< セルフシャドウ
	edge				= 1<<4,			//!< エッジ
};

enum class	sphire_mode
{
	disable		= 0,		//!< 無効
	sph			= 1,		//!< 乗算
	spa			= 2,		//!< 加算
	sub_texture	= 3,		//!< サブテクスチャ
};

enum class	toon_resource
{
	texture		= 0,		//!< 個別にテクスチャを使用する
	shaded		= 1,		//!< 共有テクスチャを使用する
};

enum 		bone_flags
{
	bone_joint			= 1<<0,			//!< 0x0001	接続先の指定 Off: 座標オフセットで指, On: ボーンで指定
	rotation_enable		= 1<<1,			//!< 0x0002	回転可能かどうか
	translate_enable	= 1<<2,			//!< 0x0004	移動可能かどうか
	visibe				= 1<<3,			//!< 0x0008	表示
	controllable		= 1<<4,			//!< 0x0010	操作可能
	ik					= 1<<5,			//!< 0x0020	IK support.
	//					= 1<<6,
	local_appendix		= 1<<7,			//!< 0x0080	ローカル付与対象 Off: ユーザー変形値／IKリンク／多重付与, On: 親のローカル変形量
	rotate_appendix		= 1<<8,			//!< 0x0100	回転付与
	translate_appendix	= 1<<9,			//!< 0x0200	移動付与
	fixed_axis			= 1<<10,		//!< 0x0400	軸固定
	local_axis			= 1<<11,		//!< 0x0800	ローカル軸
	transform_after_phy	= 1<<12,		//!< 0x1000	物理後変形
	external_transform	= 1<<13,		//!< 0x2000	外部親変形
};


enum class	pmx_wait_type
{
	bdef1,
	bdef2,
	bdef4,
	sdef,
};

struct	pmx_bone
{
	pmx_wait_type	type;
};

struct	pmx_bone_bdef1 : pmx_bone
{
	int32_t			bone;
};

struct	pmx_bone_bdef2 : pmx_bone
{
	int32_t			bone1;
	int32_t			bone2;
	float			wait;
};

struct	pmx_bone_bdef4 : pmx_bone
{
	int32_t			bone1;
	int32_t			bone2;
	int32_t			bone3;
	int32_t			bone4;
	float			wait1;
	float			wait2;
	float			wait3;
	float			wait4;
};

struct	pmx_bone_sdef : pmx_bone
{
	int32_t			bone1;
	int32_t			bone2;
	float			wait;
	vec3	sdef_c;
	vec3	sdef_r0;
	vec3	sdef_r1;
};



struct	header
{
	char						signature[4];			// "PMX "
	float						version;				// 2.0
	std::size_t					length;					// 8 fixed.
	text_encoding_type			text_encoding;
	std::size_t					appendix_uv;			// 0 - 4
	index_size_type				vertex_index_size;
	index_size_type				texture_index_size;
	index_size_type				material_index_size;
	index_size_type				bone_index_size;
	index_size_type				morph_index_size;
	index_size_type				rigid_body_index_size;

	inline static header create(bytereader& _reader);
};

struct	data_reader
{
	using	readtext_proc	= string_type(*)(bytereader& _reader);
	using	readindex_proc	= int32_t(*)(bytereader& _reader);

	readtext_proc	text;
	readindex_proc	vtx_index;
	readindex_proc	tex_index;
	readindex_proc	mtl_index;
	readindex_proc	bone_index;
	readindex_proc	morph_index;
	readindex_proc	rig_index;

	inline static data_reader create(header& _header);
};



struct	model_info
{
	string_type	name;
	string_type	name_eng;
	string_type	comment;
	string_type	comment_eng;

	inline static model_info create(data_reader& _reader, bytereader& _buffer);
};


struct	vertex
{
	vec3	position;
	vec3	normal;
	vec2	uv;
	vec4	appendix_uv[4];
	pmx_bone*		bone			= nullptr;
	float			edge_ratio;

	vertex() = default;
	vertex(const vertex&) = delete;
	vertex(vertex&& _r)
		: position(std::move(_r.position))
		, normal(std::move(_r.normal))
		, uv(std::move(_r.uv))
		, appendix_uv{_r.appendix_uv[0], _r.appendix_uv[1], _r.appendix_uv[2], _r.appendix_uv[3]}
		, edge_ratio(std::move(_r.edge_ratio))
	{
		bone	= _r.bone;
		_r.bone	= nullptr;
	}
	~vertex()
	{
		if (bone)
		{
			delete	bone;
		}
	}

	inline static vertex create(data_reader& _reader, bytereader& _buffer, std::size_t _appendix_uv_size);
};


struct	material
{
	string_type					name;
	string_type					name_eng;
	ao::g3d::color				diffuse;
	vec3				specular;
	float						specular_power;
	vec3				ambient;
	unsigned int				render_flags;		// render_flags の組み合わせ
	ao::g3d::color				edge_color;
	float						edge_size;
	int32_t						texture_index;
	int32_t						spire_texture_index;
	sphire_mode					sphire;
	toon_resource				toon_type;
	int32_t						toon_index;
	string_type					extra;
	int32_t						vertex_size;

	inline static material create(data_reader& _reader, bytereader& _buffer);
};


struct	ik_link
{
	int32_t			bone_index;
	bool			angle_limit;
	vec3	lower_limit;
	vec3	upper_limit;

	inline static ik_link create(data_reader& _reader, bytereader& _buffer);
};


struct	bone
{
	string_type					name;
	string_type					name_eng;
	vec3						position;
	int32_t						parent_bone_index;
	int32_t						transform_depth;
	unsigned int				bone_flags;

	// bone_joint
	union
	{
		vec3						offset;
		int32_t						bone_index;
	}							joint;

	// rotation_appendix
	struct
	{
		int32_t						parent_bone_index;
		float						ratio;
	}							rotation_appendix;

	// translate_appendix
	struct
	{
		int32_t						parent_bone_index;
		float						ratio;
	}							translate_appendix;

	// fixed_axis
	vec3						axis;

	// local_axis
	vec3						local_x_axis;
	vec3						local_z_axis;

	// external_transform
	int32_t						external_transform_key;

	// ik
	int32_t						ik_target_bone_index;
	int32_t						ik_loop_count;
	float						ik_angle_limit;
	std::vector<ik_link>		ik_link_list;

	inline static bone create(data_reader& _reader, bytereader& _buffer);
};


struct	pmxdata
{
	typename header				header;
	model_info					model_info;
	std::vector<vertex>			vertex_list;
	std::vector<uint32_t>		surface_list;
	std::vector<string_type>	texture_path_list;
	std::vector<material>		material_list;
	std::vector<bone>			bone_list;

	static pmxdata parse(const std::byte* _pmx_buffer, std::size_t _pmx_buffer_size);
};

inline void operator >> (bytereader& _buff, vec2& _vec) { _buff.read_le(_vec, 2); }
inline void operator >> (bytereader& _buff, vec3& _vec) { _buff.read_le(_vec, 3); }
inline void operator >> (bytereader& _buff, vec4& _vec) { _buff.read_le(_vec, 4); }


inline pmxdata pmxdata::parse(const std::byte* _pmx_buffer, std::size_t _pmx_buffer_size)
{
	bytereader		buff(_pmx_buffer, _pmx_buffer_size);
	pmxdata			thiz;
	data_reader		reader;


	// ヘッダ生成
	thiz.header		= header::create(buff);

	// ヘッダを元にデータを読みこむ関数を生成
	reader			= data_reader::create(thiz.header);

	// モデル情報を生成
	thiz.model_info	= model_info::create(reader, buff);

	// read vertex list.
	{
		std::size_t		size	= buff.read_le<uint32_t>();
		thiz.vertex_list.reserve(size);
		while (size > 0)
		{
			thiz.vertex_list.push_back(vertex::create(reader, buff, thiz.header.appendix_uv));
			--size;
		}
	}

	// read surface list.
	{
		std::size_t		size	= buff.read_le<uint32_t>();
		thiz.surface_list.reserve(size);
		while (size > 0)
		{
			thiz.surface_list.push_back(reader.vtx_index(buff));
			--size;
		}
	}

	// texture path list
	{
		std::size_t		size	= buff.read_le<uint32_t>();
		thiz.texture_path_list.reserve(size);
		while (size > 0)
		{
			thiz.texture_path_list.push_back(reader.text(buff));
			--size;
		}
	}

	// material list
	{
		std::size_t		size	= buff.read_le<uint32_t>();
		thiz.material_list.reserve(size);
		while (size > 0)
		{
			thiz.material_list.push_back(material::create(reader, buff));
			--size;
		}
	}

	// bone list
	{
		std::size_t		size	= buff.read_le<uint32_t>();
		thiz.bone_list.reserve(size);
		while (size > 0)
		{
			thiz.bone_list.push_back(bone::create(reader, buff));
			--size;
		}
	}

	return	thiz;
}


header header::create(bytereader& _reader)
{
	header	thiz;
	_reader.read_le(thiz.signature, 4);
	thiz.version				= _reader.read_le<float>();
	thiz.length					= _reader.read_le<uint8_t>();
	thiz.text_encoding			= static_cast<text_encoding_type>(_reader.read_le<uint8_t>());
	thiz.appendix_uv			= _reader.read_le<uint8_t>();
	thiz.vertex_index_size		= static_cast<index_size_type>(_reader.read_le<uint8_t>());
	thiz.texture_index_size		= static_cast<index_size_type>(_reader.read_le<uint8_t>());
	thiz.material_index_size	= static_cast<index_size_type>(_reader.read_le<uint8_t>());
	thiz.bone_index_size		= static_cast<index_size_type>(_reader.read_le<uint8_t>());
	thiz.morph_index_size		= static_cast<index_size_type>(_reader.read_le<uint8_t>());
	thiz.rigid_body_index_size	= static_cast<index_size_type>(_reader.read_le<uint8_t>());
	return	thiz;
}



data_reader data_reader::create(header& _header)
{
	data_reader	thiz;
	if (_header.text_encoding == text_encoding_type::utf16)
	{
		thiz.text	= [](bytereader& _reader) -> string_type
		{
			std::size_t				len	= _reader.read_le<uint32_t>() / sizeof(wchar_t);
			std::vector<wchar_t>	text(len);
			_reader.read_le(text.data(), len);
			return	str::to_u8string({text.data(), text.size()});
		};
	}
	else
	{
		thiz.text	= [](bytereader& _reader) -> string_type
		{
			std::size_t				len	= _reader.read_le<uint32_t>();
			std::vector<char>		text(len);
			_reader.read_le(text.data(), len);
			return	{text.data(), text.size()};
		};
	}

	auto	make_index_reader	= [](index_size_type _st) -> readindex_proc
	{
		switch (_st)
		{
			case index_size_type::int8  :	return	[](bytereader& _reader) -> int32_t { return _reader.read_le<int8_t>();  };
			case index_size_type::int16 :	return	[](bytereader& _reader) -> int32_t { return _reader.read_le<int16_t>(); };
			case index_size_type::int32 :	return	[](bytereader& _reader) -> int32_t { return _reader.read_le<int32_t>(); };
			default:	throw std::runtime_error("index type error.");
		}
	};
	thiz.vtx_index		= make_index_reader(_header.vertex_index_size);
	thiz.tex_index		= make_index_reader(_header.texture_index_size);
	thiz.mtl_index		= make_index_reader(_header.material_index_size);
	thiz.bone_index		= make_index_reader(_header.bone_index_size);
	thiz.morph_index	= make_index_reader(_header.morph_index_size);
	thiz.rig_index		= make_index_reader(_header.rigid_body_index_size);
	return	thiz;
}


model_info model_info::create(data_reader& _reader, bytereader& _buffer)
{
	model_info	thiz;
	thiz.name			= _reader.text(_buffer);
	thiz.name_eng		= _reader.text(_buffer);
	thiz.comment		= _reader.text(_buffer);
	thiz.comment_eng	= _reader.text(_buffer);
	return	thiz;
}


vertex vertex::create(data_reader& _reader, bytereader& _buffer, std::size_t _appendix_uv_size)
{
	vertex	vtx;
	_buffer >> vtx.position;
	_buffer >> vtx.normal;
	_buffer >> vtx.uv;

	for (decltype(_appendix_uv_size) aidx = 0; aidx < _appendix_uv_size; ++aidx)
	{
		_buffer >> vtx.appendix_uv[aidx];
	}
	pmx_wait_type	bt	= static_cast<pmx_wait_type>(_buffer.read_le<uint8_t>());
	switch (bt)
	{
		case pmx_wait_type::bdef1 :
		{
			auto*	bw	= new pmx_bone_bdef1();
			bw->bone	= _reader.bone_index(_buffer);
			vtx.bone	= bw;
			break;
		}

		case pmx_wait_type::bdef2 :
		{
			auto*	bw	= new pmx_bone_bdef2();
			bw->bone1	= _reader.bone_index(_buffer);
			bw->bone2	= _reader.bone_index(_buffer);
			bw->wait	= _buffer.read_le<float>();
			vtx.bone	= bw;
			break;
		}

		case pmx_wait_type::bdef4 :
		{
			auto*	bw	= new pmx_bone_bdef4();
			bw->bone1	= _reader.bone_index(_buffer);
			bw->bone2	= _reader.bone_index(_buffer);
			bw->bone3	= _reader.bone_index(_buffer);
			bw->bone4	= _reader.bone_index(_buffer);
			bw->wait1	= _buffer.read_le<float>();
			bw->wait2	= _buffer.read_le<float>();
			bw->wait3	= _buffer.read_le<float>();
			bw->wait4	= _buffer.read_le<float>();
			vtx.bone	= bw;
			break;
		}

		case pmx_wait_type::sdef :
		{
			auto*	bw		= new pmx_bone_sdef();
			bw->bone1		= _reader.bone_index(_buffer);
			bw->bone2		= _reader.bone_index(_buffer);
			bw->wait		= _buffer.read_le<float>();
			_buffer >> bw->sdef_c;
			_buffer >> bw->sdef_r0;
			_buffer >> bw->sdef_r1;
			vtx.bone		= bw;
			break;
		}
	}
	vtx.bone->type	= bt;
	vtx.edge_ratio	= _buffer.read_le<float>();
	return	vtx;
}


material material::create(data_reader& _reader, bytereader& _buffer)
{
	material	mtl;
	mtl.name				= _reader.text(_buffer);
	mtl.name_eng			= _reader.text(_buffer);
	_buffer >> mtl.diffuse;
	_buffer >> mtl.specular;
	mtl.specular_power		= _buffer.read_le<float>();
	_buffer >> mtl.ambient;
	mtl.render_flags		= _buffer.read_le<uint8_t>();
	_buffer >> mtl.edge_color;
	mtl.edge_size			= _buffer.read_le<float>();
	mtl.texture_index		= _reader.tex_index(_buffer);
	mtl.spire_texture_index	= _reader.tex_index(_buffer);
	mtl.sphire				= static_cast<sphire_mode>(_buffer.read_le<uint8_t>());
	mtl.toon_type			= static_cast<toon_resource>(_buffer.read_le<uint8_t>());
	switch (mtl.toon_type)
	{
		case toon_resource::texture:	mtl.toon_index = _reader.tex_index(_buffer);	break;
		case toon_resource::shaded:		mtl.toon_index = _buffer.read_le<uint8_t>();	break;
	}
	mtl.extra				= _reader.text(_buffer);
	mtl.vertex_size			= _buffer.read_le<int32_t>();
	return	mtl;
}


ik_link ik_link::create(data_reader& _reader, bytereader& _buffer)
{
	ik_link	thiz;
	thiz.bone_index		= _reader.bone_index(_buffer);
	thiz.angle_limit	= _buffer.read_le<uint8_t>() ? true : false;
	if (thiz.angle_limit)
	{
		_buffer >> thiz.lower_limit;
		_buffer >> thiz.upper_limit;
	}
	return	thiz;
}


bone bone::create(data_reader& _reader, bytereader& _buffer)
{
	bone	thiz;
	thiz.name				= _reader.text(_buffer);
	thiz.name_eng			= _reader.text(_buffer);
	_buffer >> thiz.position;
	thiz.parent_bone_index	= _reader.bone_index(_buffer);
	thiz.transform_depth	= _buffer.read_le<int32_t>();
	thiz.bone_flags			= _buffer.read_le<uint16_t>();

	if (thiz.bone_flags & bone_flags::bone_joint)
	{
		thiz.joint.bone_index	= _reader.bone_index(_buffer);
	}
	else
	{
		_buffer >> thiz.joint.offset;
	}

	if (thiz.bone_flags & bone_flags::rotate_appendix)
	{
		thiz.rotation_appendix.parent_bone_index	= _reader.bone_index(_buffer);
		thiz.rotation_appendix.ratio				= _buffer.read_le<float>();
	}

	if (thiz.bone_flags & bone_flags::translate_appendix)
	{
		thiz.translate_appendix.parent_bone_index	= _reader.bone_index(_buffer);
		thiz.translate_appendix.ratio				= _buffer.read_le<float>();
	}

	if (thiz.bone_flags & bone_flags::fixed_axis)
	{
		_buffer >> thiz.axis;
	}

	if (thiz.bone_flags & bone_flags::local_axis)
	{
		_buffer >> thiz.local_x_axis;
		_buffer >> thiz.local_z_axis;
	}

	if (thiz.bone_flags & bone_flags::external_transform)
	{
		thiz.external_transform_key					= _buffer.read_le<int32_t>();
	}

	if (thiz.bone_flags & bone_flags::ik)
	{
		thiz.ik_target_bone_index			= _reader.bone_index(_buffer);
		thiz.ik_loop_count					= _buffer.read_le<int32_t>();
		thiz.ik_angle_limit					= _buffer.read_le<float>();
		std::size_t		size	= _buffer.read_le<int32_t>();
		while (size > 0)
		{
			thiz.ik_link_list.push_back(ik_link::create(_reader, _buffer));
			--size;
		};
	}

	return	thiz;
}


AMTRS_MODEL_PMX_NAMESPACE_END
#endif
