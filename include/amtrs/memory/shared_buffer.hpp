/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__memory__shared_buffer__hpp
#define	__libamtrs__memory__shared_buffer__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
struct	nullstruct{};

// *****************************************************************************
//! 共有バッファ。
// ----------------------------------------------------------------------------
//! バッファをポインタで管理するため、複数の場所で同じインスタンスを参照できます。
//! 参照カウンタで寿命管理された配列で、std::shared_ptr<T[]> に近いものになって
//! います。shared_ptrとの違いとして、
//!		・サイズを持っている
//!		・begin(), end() のイテレータが使える
//!	というメリットがあります。
//! また、確保されたバッファの最後尾には必ず char ひとつ分の未使用領域が確保
//! され、0 でクリアされます。つまり、shared_buffer<char>::data() を使った場合
//! には、必ず 0 で終わる文字列のバッファとして利用できることを意味します。
// ----------------------------------------------------------------------------
template<class T, class HeaderT = nullstruct>
class	shared_buffer
{
public:
	using	size_type		= std::size_t;
	using	element_type	= T;
	using	value_type		= T;
	using	pointer			= value_type*;
	using	const_pointer	= typename std::add_const<pointer>::type;
	using	header_type		= HeaderT;

	shared_buffer() = default;
	shared_buffer(const shared_buffer& _r) = default;
	shared_buffer(shared_buffer&&) = default;
	shared_buffer& operator = (const shared_buffer&) = default;
	shared_buffer& operator = (shared_buffer&&) = default;
	shared_buffer& operator = (std::nullptr_t) { mHeap = nullptr; return *this; }


	// ========================================================================
	//! 指定したサイズのバッファを確保します。
	// ------------------------------------------------------------------------
	shared_buffer(size_type _size);

	// ========================================================================
	//! イテレータで指定するデータをコピーして新しいバッファを確保します。
	// ------------------------------------------------------------------------
	template<class FirstIt, class LastIt>
	shared_buffer(FirstIt _first, LastIt _last);

	template<class I>
	auto& operator [] (I _index) noexcept { return data()[_index]; }
	template<class I>
	auto& operator [] (I _index) const noexcept { return data()[_index]; }

	// ========================================================================
	//! 同じヒープを指しているか調べます。
	// ------------------------------------------------------------------------
	bool operator == (const shared_buffer& _r) const noexcept { return mHeap == _r.mHeap; }

	// ========================================================================
	//! 同じヒープを指していないか調べます。
	// ------------------------------------------------------------------------
	bool operator != (const shared_buffer& _r) const noexcept { return mHeap != _r.mHeap; }

	// ========================================================================
	//! イテレータを使用し中身が同じか調べます。
	// ------------------------------------------------------------------------
	bool eaual(const shared_buffer& _r) const noexcept { return std::equal(std::begin(*this), std::end(*this), std::begin(_r), std::end(_r)); }

	// ========================================================================
	//! イテレータを使用し中身が同じか調べます。
	// ------------------------------------------------------------------------
	template<class FirstIt, class LastIt>
	bool eaual(FirstIt _first, LastIt _last) const noexcept { return std::equal(std::begin(*this), std::end(*this), _first, _last); }

	// ========================================================================
	//! string へ変換します。
	// ------------------------------------------------------------------------
	template<class Traits, class Allocator>
	explicit operator std::basic_string<value_type, Traits, Allocator> () const noexcept { return {data(), size()}; }

	shared_buffer& assign(const shared_buffer& _r) { mHeap = _r.mHeap; return *this; }
	shared_buffer& assign(shared_buffer&& _r) { mHeap = std::move(_r.mHeap); return *this; }

	template<class It>
	shared_buffer& assign(It _first, It _last);
	

	// ========================================================================
	//! バッファの先頭を返します。
	// ------------------------------------------------------------------------
	      pointer data()       noexcept { return empty() ? nullptr : mHeap->data(); }

	// ========================================================================
	//! バッファの先頭を返します。
	// ------------------------------------------------------------------------
	const_pointer data() const noexcept { return empty() ? nullptr : mHeap->data(); }

	// ========================================================================
	//! バッファを持っているかどうか調べます。
	// ------------------------------------------------------------------------
	//! size() == 0 でもバッファを持っている場合は false になります。
	// ------------------------------------------------------------------------
	bool empty() const noexcept { return mHeap.empty(); }

	// ========================================================================
	//! バッファのサイズを返します。
	// ------------------------------------------------------------------------
	//! バッファを持っていない場合も 0 になります。
	// ------------------------------------------------------------------------
	std::size_t size() const noexcept { return empty() ? 0 : mHeap->length; }

	// ========================================================================
	//! バッファを開放します。
	// ------------------------------------------------------------------------
	void release() { mHeap = nullptr; }

	auto& front() noexcept { return *begin(); }
	auto& back() noexcept { return *(end()-1); }
	auto& front() const noexcept { return *begin(); }
	auto& back() const noexcept { return *(end()-1); }

	header_type& header() noexcept { return mHeap->header; }
	const header_type& header() const noexcept { return mHeap->header; }

	auto begin() noexcept { return mHeap ? mHeap->data() : nullptr; }
	auto begin() const noexcept { return mHeap ? mHeap->data() : nullptr; }
	auto end() noexcept { return mHeap ? mHeap->data() + mHeap->length : nullptr; }
	auto end() const noexcept { return mHeap ? mHeap->data() + mHeap->length : nullptr; }

	size_type hash() noexcept { return std::hash<std::basic_string_view<T>>{}(std::basic_string_view<T>{data(), size()}); }
	size_type hash() const noexcept { return std::hash<std::basic_string_view<T>>{}(std::basic_string_view<T>{data(), size()}); }


protected:


	struct	heap : ref_object
	{
		size_type		length	= 0;
		header_type		header;

		~heap()
		{
			while (length > 0)
			{
				auto*	v	= &data()[--length];
				v->~value_type();
			}
		}

		value_type* data() noexcept { return reinterpret_cast<value_type*>(this + 1); }
		const value_type* data() const noexcept { return reinterpret_cast<const value_type*>(this + 1); }

		static ref<heap> create(size_type _len)
		{
			auto		sz		= sizeof(heap) + sizeof(value_type) * _len;
			char*		mem		= new char[sz + 1];
			mem[sz]	= 0x00;
			return	new (mem) heap();
		}
	};

	ref<heap>	mHeap;
};


template<class T, class HeaderT>
shared_buffer<T, HeaderT>::shared_buffer(size_type _size)
{
	size_type	length	= _size;
	mHeap	= heap::create(length);
	auto*		data	= mHeap->data();
	while (mHeap->length < _size)
	{
		new (&data[mHeap->length++]) value_type();
	}
}


template<class T, class HeaderT>
template<class FirstIt, class LastIt>
shared_buffer<T, HeaderT>::shared_buffer(FirstIt _first, LastIt _last)
{
	assign(_first, _last);
}


template<class T, class HeaderT>
template<class It>
auto shared_buffer<T, HeaderT>::assign(It _first, It _last) -> shared_buffer&
{
	size_type	length	= std::distance(_first, _last);
	if (size() != length)
	{
		mHeap	= heap::create(length);
		auto*		data	= mHeap->data();
		while (_first != _last)
		{
			new (&data[mHeap->length++]) value_type(*_first++);
		}
	}
	else
	{
		auto*		data	= mHeap->data();
		while (_first != _last)
		{
			data[mHeap->length++]	= value_type(*_first++);
		}
	}
	return	*this;
}


AMTRS_NAMESPACE_END
namespace std {

template<class T, class H>
struct	hash<AMTRS_NAMESPACE::shared_buffer<T, H>>
{
	std::size_t operator()(AMTRS_NAMESPACE::shared_buffer<T, H> const& s) const noexcept
    {
        return	s.hash();
    }
};

}
#endif
