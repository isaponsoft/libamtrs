/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__passion__vm__vm_string__hpp
#define	__passion__vm__vm_string__hpp
#include <algorithm>
#include <unordered_set>
#include <ostream>
#include <string_view>
#include <amtrs/amtrs.hpp>
AMTRS_NAMESPACE_BEGIN

struct	vmstring;


// ============================================================================
//!	同一の文字列のメモリを共有する文字列クラスです。
// ----------------------------------------------------------------------------
//!	新しい文字列を作成するとき、同一の文字列が既に存在するか調べ、文字列が存在
//!	する場合には新たなインスタンスを用意せずに、既存のインスタンスを返します。
//!	そのため、スクリプト言語の処理など、同一の文字列を多く使うようなプログラム
//!	において省メモリ化が図れます。また、同一の文字列は必ずポインタが一致する
//!	事が保証されるため、同一の文字列かどうかはポインタの比較だけで済むため、
//!	やはりスクリプト言語のような文字列の比較が多いプログラムにおいて力を
//!	発揮します。
//!
//!	また、共有文字列は辞書データを共有するという特性上、値を書き換えることは
//!	禁止になっています。data() で所得したポインタの中身を直接書き換えると
//!	意図しない動作になります。
// ----------------------------------------------------------------------------
template<class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
class	basic_vmstring
{
public:
	struct	instance;

	using	value_type		= CharT;
	using	element_type	= CharT;
	using	pointer			= const CharT*;
	using	const_pointer	= const CharT*;
	using	view_type		= std::basic_string_view<CharT, Traits>;
	using	size_type		= std::size_t;
	using	traits_type		= Traits;

	static const std::size_t	npos = std::basic_string_view<CharT>::npos;

	basic_vmstring() = default;
	basic_vmstring(const basic_vmstring& ) = default;
	basic_vmstring(      basic_vmstring&&) = default;

	//!	指定するビューから文字列を生成します。
	basic_vmstring(const_pointer _s) : basic_vmstring(view_type{_s}) {}
	basic_vmstring(const_pointer _s, std::size_t _len) : basic_vmstring(view_type{_s, _len}) {}

	//!	指定するビューから文字列を生成します。
	basic_vmstring(view_type _s);

	//!	指定する標準文字列から文字列を生成します。
	template<class _A>
	basic_vmstring(std::basic_string<CharT, Traits, _A> _s) : basic_vmstring(view_type{_s.data(), _s.size()}) {}

	operator passion_string_view () const noexcept { return {data(), size()}; }

	basic_vmstring& operator = (const basic_vmstring& ) = default;
	basic_vmstring& operator = (      basic_vmstring&&) = default;
	basic_vmstring& operator = (view_type _r) { *this = basic_vmstring{_r}; return *this; }
	basic_vmstring& operator = (const_pointer _r) { *this = basic_vmstring{_r}; return *this; }
	template<class _A> basic_vmstring& operator = (const std::basic_string<CharT, Traits, _A>& _r) { *this = basic_vmstring{_r}; return *this; }


	//! 文字列同士を比較します。
	bool operator == (const basic_vmstring& _r) const noexcept;
	bool operator != (const basic_vmstring& _r) const noexcept;
	bool operator == (view_type _r) const noexcept;
	bool operator != (view_type _r) const noexcept;
	bool operator == (const_pointer _r) const noexcept;
	bool operator != (const_pointer _r) const noexcept;
	template<class _A> bool operator == (const std::basic_string<CharT, Traits, _A>& _s) const noexcept;
	template<class _A> bool operator != (const std::basic_string<CharT, Traits, _A>& _s) const noexcept;

	//! 文字列同士を連結します。
	basic_vmstring operator + (const basic_vmstring& _r) const { return *this + (view_type)_r; }
	basic_vmstring operator + (view_type _r) const { return create((view_type)*this, _r); }
	basic_vmstring operator + (const_pointer _r) const { return *this + view_type{_r}; }
	template<class _A> basic_vmstring operator + (const std::basic_string<CharT, Traits, _A>& _r) const { return *this + view_type{_r.data(), _r.size()}; }
	basic_vmstring& operator += (const basic_vmstring& _r) { *this = *this + _r; return *this; }
	basic_vmstring& operator += (view_type _r) { *this = *this + _r; return *this; }
	basic_vmstring& operator += (const_pointer _r) { *this = *this + _r; return *this; }
	template<class _A> basic_vmstring operator += (const std::basic_string<CharT, Traits, _A>& _r) { *this = *this + _r; return *this; }

	const value_type& operator [] (std::size_t _index) const { return data()[_index]; }

	const_pointer c_str() const noexcept { return data(); }
	const_pointer data() const noexcept;
	std::size_t size() const noexcept;
	bool empty() const noexcept { return size() == 0; }
	std::size_t hash() const noexcept;
	
	size_type find(const basic_vmstring& str, size_type pos = 0) const;
	size_type find(const CharT* s, size_type pos, size_type n) const;
	size_type find(const CharT* s, size_type pos = 0) const;
	size_type find(CharT c, size_type pos = 0) const;

	size_type rfind(const CharT* s, size_type pos, size_type n) const;
	size_type rfind(view_type s, size_type pos = 0) const;
	size_type rfind(const CharT* s, size_type pos = 0) const;


	size_type find_first_of(const CharT* s, std::size_t pos = 0) const;
	size_type find_last_of(CharT c, size_type pos = npos) const noexcept { return ((view_type)*this).find_last_of(c, pos); }

	basic_vmstring& replace(size_type _pos, size_type _size, const CharT* _replacement, std::size_t _replacement_size); 
	basic_vmstring& replace(size_type _pos, size_type _size, view_type _replacement) { return replace(_pos, _size, _replacement.data(), _replacement.size()); } 

	basic_vmstring substr(std::size_t _pos, std::size_t _size = npos) const { return {((view_type)*this).substr(_pos, _size)}; }

	operator view_type () const noexcept { return {data(), size()}; }
	auto begin() const noexcept -> decltype(std::declval<view_type&>().cbegin()) { return ((view_type)*this).cbegin(); }
	auto end() const noexcept -> decltype(std::declval<view_type&>().cend()) { return ((view_type)*this).cend(); }
	auto cbegin() const noexcept -> decltype(std::declval<view_type&>().cbegin()) { return ((view_type)*this).cbegin(); }
	auto cend() const noexcept -> decltype(std::declval<view_type&>().cend()) { return ((view_type)*this).cend(); }

	auto& front() noexcept { return *begin(); }
	auto& back() noexcept { return *(end()-1); }
	auto& front() const noexcept { return *begin(); }
	auto& back() const noexcept { return *(end()-1); }

	template<class BSAllocator>
	operator std::basic_string<CharT, Traits, BSAllocator> () const { return std::basic_string<CharT, Traits, BSAllocator>(begin(), end()); }

	static basic_vmstring create(view_type _l, view_type _r);

	// 文字列本体
	struct	instance : ref_object
	{
		static ref<instance> create(view_type _l, view_type _r = {});
		CharT* data(){ return reinterpret_cast<CharT*>(this + 1); }
	private:
		std::size_t					m_hash;
		std::size_t					m_size;
		CharT*						m_data;

		friend	basic_vmstring;
	};

	ref<instance> get() const { return m_instance; }
	basic_vmstring(instance* _instance) : m_instance(_instance) {}

private:
	// unorederd_set で instance をキーとして登録するためのラッパー
	struct	instance_ref
	{
		instance_ref(instance* _s) : m_instance(_s) {}
		instance_ref() : m_instance(nullptr) {}
		instance_ref(const instance_ref&) = default;
		instance_ref(      instance_ref&&) = default;
		instance_ref& operator = (const instance_ref&) = default;
		instance_ref& operator = (      instance_ref&&) = default;
		bool operator == (const instance_ref& _o) const { return m_instance == _o.m_instance; }
		bool operator != (const instance_ref& _o) const { return m_instance != _o.m_instance; }
		bool operator == (const std::string_view& _o) const
		{
			return	((m_instance->m_size == _o.size()) && (xmemcmp(m_instance->data(), _o.data(), _o.size()) == 0))
					? true
					: false;
		}

		instance*	m_instance;	// キー自身は参照カウンタを増やさないようにしておく
	};

	struct	get_hash
	{
		size_t operator()(const instance_ref& _o) const { return _o.m_instance->m_hash; }
	};

	ref<instance>	m_instance;
};


// ****************************************************************************
//! basic_vmstring を強い型付けする
// ----------------------------------------------------------------------------
struct	vmstring : basic_vmstring<char>
{
	using	_base_type	= basic_vmstring<char>;
	using	_base_type::_base_type;
	using	_base_type::operator =;
	using	_base_type::operator ==;
	using	_base_type::operator !=;

	template<class T> vmstring  operator + (T _r)  const { return vmstring(_base_type::operator + (_r)); }
	template<class T> vmstring& operator += (T _r)       { vmstring(_base_type::operator += (_r)); return *this; }
};



template<class CharT, class Traits, class Allocator>
auto operator + (std::basic_string_view<CharT, Traits> _l, const basic_vmstring<CharT, Traits, Allocator> _r)
		-> basic_vmstring<CharT, Traits, Allocator>
{
	return	basic_vmstring<CharT, Traits, Allocator>::create(_l, (std::basic_string_view<CharT, Traits>)_r);
}

template<class CharT, class Traits, class Allocator>
auto operator + (const CharT* _l, const basic_vmstring<CharT, Traits, Allocator> _r)
		-> basic_vmstring<CharT, Traits, Allocator>
{
	return	basic_vmstring<CharT, Traits, Allocator>::create((std::basic_string_view<CharT, Traits>)_l, (std::basic_string_view<CharT, Traits>)_r);
}

template<class CharT, class Traits, class Allocator, class AllocatorBS>
auto operator + (const std::basic_string<CharT, Traits, AllocatorBS>& _l, const basic_vmstring<CharT, Traits, Allocator> _r)
		-> basic_vmstring<CharT, Traits, Allocator>
{
	return	basic_vmstring<CharT, Traits, Allocator>::create(std::basic_string_view<CharT, Traits>{_l.c_str(), _l.size()}, (std::basic_string_view<CharT, Traits>)_r);
}





template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::create(view_type _l, view_type _r) -> basic_vmstring
{
	basic_vmstring<CharT, Traits, Allocator>	retval;
	retval.m_instance	= instance::create(_l, _r);
	return	retval;
}



template<class CharT, class Traits, class Allocator>
basic_vmstring<CharT, Traits, Allocator>::basic_vmstring(std::basic_string_view<CharT, Traits> _s)
	: m_instance(instance::create(_s))
{
}


template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::data() const noexcept -> const_pointer
{
	static const value_type	_empty[] = {'\0'};
	return	m_instance.empty()
			? _empty
			: m_instance->data();
}


template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::size() const noexcept -> std::size_t
{
	return	m_instance.empty()
			? 0
			: m_instance->m_size;
}


template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::hash() const noexcept -> std::size_t
{
	return	m_instance.empty()
			? std::hash<std::basic_string<CharT, Traits>>{}("")
			: m_instance->m_hash;
}



template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::instance::create(view_type _l, view_type _r) -> ref<instance>
{
	auto						len		= _l.size() + _r.size();
	if (len == 0)
	{
		return	{};
	}
	ref<instance>	thiz	= new (new char[sizeof(instance) + sizeof(value_type) * (len + 1)]) instance();
	auto*						mem		= thiz->data();
	std::memcpy(mem            , _l.data(), _l.size());
	std::memcpy(mem + _l.size(), _r.data(), _r.size());
	mem[len]			= '\0';
	thiz->m_data		= thiz->data();
	thiz->m_hash		= std::hash<std::string>{}(mem);
	thiz->m_size		= len;

	return	thiz;
}


template<class CharT, class Traits, class Allocator>
bool basic_vmstring<CharT, Traits, Allocator>::operator == (const basic_vmstring& _r) const noexcept
{
	if ((m_instance == _r.m_instance) || (*this == (view_type)_r))
	{
		return	true;
	}
	return	false;
}


template<class CharT, class Traits, class Allocator>
bool basic_vmstring<CharT, Traits, Allocator>::operator != (const basic_vmstring& _r) const noexcept
{
	return	!(*this == _r);
}


template<class CharT, class Traits, class Allocator>
bool basic_vmstring<CharT, Traits, Allocator>::operator == (view_type _r) const noexcept
{
	return	size() == _r.size()
			? (!std::memcmp(data(), _r.data(), _r.size()))
			: false;
}


template<class CharT, class Traits, class Allocator>
bool basic_vmstring<CharT, Traits, Allocator>::operator != (view_type _r) const noexcept
{
	return	!(*this == _r);
}


template<class CharT, class Traits, class Allocator>
bool basic_vmstring<CharT, Traits, Allocator>::operator == (const_pointer _r) const noexcept
{
	return	(!std::strcmp(data(), _r));
}


template<class CharT, class Traits, class Allocator>
bool basic_vmstring<CharT, Traits, Allocator>::operator != (const_pointer _r) const noexcept
{
	return	!(*this == _r);
}


template<class CharT, class Traits, class Allocator>
template<class _A>
bool basic_vmstring<CharT, Traits, Allocator>::operator == (const std::basic_string<CharT, Traits, _A>& _r) const noexcept
{
	return	size() == _r.size()
			? (!std::memcmp(data(), _r.data(), _r.size()))
			: false;
}


template<class CharT, class Traits, class Allocator>
template<class _A>
bool basic_vmstring<CharT, Traits, Allocator>::operator != (const std::basic_string<CharT, Traits, _A>& _r) const noexcept
{
	return	!(*this == _r);
}

template<class CharT, class Traits, class Allocator>
std::size_t basic_vmstring<CharT, Traits, Allocator>::find_first_of(const CharT* _s, std::size_t _pos) const
{
	if (_pos <= size())
	{
		auto	sz	= Traits::length(_s);
		if ((_pos + sz) <= size())
		{
			auto	lastPos	= size() - sz;
			for (auto pos = _pos; pos != lastPos; ++pos)
			{
				if (!std::memcmp(data() + pos, _s, sz))
				{
					return	pos;
				}
			}
		}
	}
	return	npos;
}



template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::find(const CharT* _s, size_type _pos) const -> size_type
{
	auto	len		= Traits::length(_s);
	auto	s		= size();
	if (s >= len)
	{
		auto	lastPos	= s - len;
		auto	pos		= _pos;
		while (pos <= lastPos)
		{
			if (!std::memcmp(data() + pos, _s, len))
			{
				return	pos;
			}
			++pos;
		}
	}
	return	npos;
}



template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::find(CharT _c, size_type _pos) const -> size_type
{
	return	std::string_view(*this).find_first_of(_c, _pos);
}


template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::rfind(const CharT* _s, size_type _pos) const -> size_type
{
	auto	len		= Traits::length(_s);
	if (len > size())
	{
		return	npos;
	}
	auto	lastPos	= size() - len;
	auto	pos		= lastPos;
	for (;;)
	{
		if (!std::memcmp(data() + pos, _s, len))
		{
			return	pos;
		}
		if (pos == _pos)
		{
			return	npos;
		}
		--pos;
	}
}


template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::rfind(view_type _s, size_type _pos) const -> size_type
{
	auto	len		= _s.size();
	if (len > size())
	{
		return	npos;
	}
	auto	lastPos	= size() - len;
	auto	pos		= lastPos;
	for (;;)
	{
		if (!std::memcmp(data() + pos, _s.data(), len))
		{
			return	pos;
		}
		if (pos == _pos)
		{
			return	npos;
		}
		--pos;
	}
}

template<class CharT, class Traits, class Allocator>
auto basic_vmstring<CharT, Traits, Allocator>::replace(size_type _pos, size_type _size, const CharT* _replacement, std::size_t _replacement_size) -> basic_vmstring&
{
	std::basic_string<CharT, Traits>	t(data(), size());
	*this	= {t.replace(_pos, _size, _replacement, _replacement_size)};
	return	*this;
}


AMTRS_NAMESPACE_END
namespace std {

template<class CharT, class Traits, class Allocator>
struct	hash<amtrs::basic_vmstring<CharT, Traits, Allocator>>
{
	using	pointer	= typename amtrs::basic_vmstring<CharT, Traits, Allocator>::const_pointer;

	std::size_t operator () (amtrs::basic_vmstring<CharT, Traits, Allocator> const& _s) const noexcept
	{
		return	_s.hash();
	}

	std::size_t operator () (pointer _s) const noexcept
	{
		return	std::hash<basic_string<CharT, Traits, Allocator>>{}(_s);
	}
};


template<class CharT, class Traits, class Allocator>
auto to_string(const amtrs::basic_vmstring<CharT, Traits, Allocator>& _s) -> string
{
	return	{_s.data(), _s.size()};
}


template< class CharT, class Traits, class Allocator>
basic_ostream<CharT, Traits>& operator<< (basic_ostream<CharT, Traits>& _os, const amtrs::basic_vmstring<CharT, Traits, Allocator>& _s)
{
	return	_os << _s.c_str();
}

template<>
struct	hash<PASSION_NAMESPACE::vmstring>
{
	std::size_t operator () (PASSION_NAMESPACE::vmstring const& _s) const noexcept
	{
		return	_s.hash();
	}
};


}
#endif
