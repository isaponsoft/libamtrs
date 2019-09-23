/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__draw_command__hpp
#define	__libamtrs__g3d__draw_command__hpp
#include "def.hpp"
#include "attribute.hpp"
#include "draw_status.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vertex_buffer.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


enum class	primitive_type
{
	triangles,
	triangle_strip,
	triangle_fan,
};


// ============================================================================
//! device::draw() で使用する描画コマンドの情報。
// ----------------------------------------------------------------------------
struct	draw_command : ref_object
{
	draw_status			status;
	ref<shader>			shader;
	primitive_type		primitive	= primitive_type::triangles;
	ref<vertex_memory>	vertex;
	std::size_t			first		= 0;
	std::size_t			count		= 0;

	virtual void* get_uniform() noexcept = 0;
};

template<class UniformT>
struct	command_queue : draw_command
{
	using	uniform_type	= UniformT;

	virtual void* get_uniform() noexcept { return &uniform; }

	uniform_type	uniform;
};


// ============================================================================
//! draw_command を簡単に扱うためのユーティリティ
// ----------------------------------------------------------------------------
//! 指定する共有データ構造を持つ draw_command を定義します。
// ----------------------------------------------------------------------------
template<class CmdTraits>
struct	draw_command_holder : command_queue<typename CmdTraits::uniform>
{
	using	command_traits		= CmdTraits;
	using	uniform_type		= typename command_traits::uniform;
	using	vertex_type			= typename command_traits::vertex;
	using	vertex_buffer_type	= vertex_buffer<vertex_type>;
	using	queue_type			= command_queue<uniform_type>;

	ref<draw_command> make_draw_command(std::size_t _first = 0)
	{
		return	make_draw_command(_first, vertex_buffer.empty() ? 0 : vertex_buffer->count());
	}

	ref<draw_command> make_draw_command(std::size_t _first, std::size_t _count)
	{
		this->vertex		= vertex_buffer;
		this->first			= _first;
		this->count			= _count;
		return	this;
	}

	ref<vertex_buffer_type>		vertex_buffer;
};


AMTRS_G3D_NAMESPACE_END
#endif
