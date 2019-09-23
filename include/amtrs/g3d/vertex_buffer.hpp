/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__vertex_buffer__hpp
#define	__libamtrs__g3d__vertex_buffer__hpp
#include <string_view>
#include "def.hpp"
#include "attribute.hpp"
#include "memory.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

template<class VertexT>
class	vertex_buffer;




// ============================================================================
//! 頂点メモリ
// ----------------------------------------------------------------------------
//! D3D12/Vulkanなどへの対応もあるため、amtrsでは必ず頂点バッファを使う必要が
//! あります。
// ----------------------------------------------------------------------------
class	vertex_memory
		: public ref_object
		, public memory<std::size_t, std::size_t>::mapped_type
{
public:
	using	memory_type			= memory<std::size_t, std::size_t>;
	using	mapped_type			= typename memory_type::mapped_type;
	using	offset_type			= typename memory_type::offset_type;
	using	size_type			= typename memory_type::size_type;

	vertex_memory(size_type _size)
		: mSize(_size)
	{}

	offset_type offset() const noexcept { return mOffset; }

	//! サイズを取得します。
	size_type size() const noexcept { return mSize; }

protected:
	offset_type				mOffset		= {0};
	size_type				mSize		= {0};
};


// ============================================================================
//! 頂点バッファ
// ----------------------------------------------------------------------------
//! vertex_memory を使いやすくするためのラッパー
// ----------------------------------------------------------------------------
template<class VertexT>
class	vertex_buffer
		: public vertex_memory
{
public:
	using	vertex_type	= VertexT;

	vertex_buffer(size_type _vertexCount)
		: vertex_memory(_vertexCount * sizeof(vertex_type))
	{}

	// ========================================================================
	//! 頂点の要素数を返します。
	// ------------------------------------------------------------------------
	size_type count() const noexcept { return this->size() / sizeof(vertex_type); }
};


AMTRS_G3D_NAMESPACE_END
#endif
