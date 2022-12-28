/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__sstring__hpp
#define	__libamtrs__string__sstring__hpp
#include "../memory/simplearray.hpp"
#include "text.hpp"
AMTRS_NAMESPACE_BEGIN

// シンプルな文字列
class	sstring
{
	using	super_type	= simplearray<char>;
public:
	using	size_type	= std::size_t;
	using	text_type	= text;
	using	value_type	= char;
	static constexpr size_type	npos	= static_cast<size_type>(-1);

	sstring() = default;
	sstring(sstring const&) = default;
	sstring(sstring&&) = default;
	sstring& operator = (sstring const&) = default;
	sstring& operator = (sstring&&) = default;

	static sstring value_of(intmax_t _n) { return value_of(_n, 10); }
	AMTRS_API static sstring value_of(intmax_t _n, int _base);

	sstring(size_type _n)
		: sstring(nullptr, _n)
	{}

	sstring(value_type const* _s)
		: sstring(_s, len(_s))
	{}

	sstring(value_type const* _s, value_type const* _e)
		: sstring(_s, (size_type)(_e - _s))
	{}

	AMTRS_API sstring(value_type const* _s, size_type _n);

	sstring(text_type const& _s)
		: sstring(_s.data(), _s.size())
	{}

	AMTRS_API ~sstring();

	value_type* begin() noexcept { return mBuff.data(); }
	value_type* end() noexcept { return mBuff.data() + size(); }
	value_type const* cbegin() const noexcept { return mBuff.data(); }
	value_type const* cend() const noexcept { return mBuff.data() + size(); }
	value_type const* begin() const noexcept { return cbegin(); }
	value_type const* end() const noexcept { return cend(); }

	value_type* data() noexcept { return mBuff.data() ? mBuff.data() : const_cast<value_type*>(zero()); }
	value_type const* data() const noexcept { return mBuff.data() ? mBuff.data() : zero(); }
	value_type* c_str() noexcept { return data(); }
	value_type const* c_str() const noexcept { return data(); }
	size_type size() const noexcept { return mBuff.empty() ? 0 : mBuff.size() - 1; }
	bool empty() const noexcept { return size() == 0; }

//	operator text_type () const noexcept { return text_type(data(), data() + size()); }

	AMTRS_API sstring replace(text_type _from, text_type _to) const;

	value_type& operator [] (int _n) noexcept { return data()[_n]; }
	value_type const& operator [] (int _n) const noexcept { return data()[_n]; }
	bool operator <  (text_type _r) const noexcept { return text_type(*this) <  _r; }
	bool operator >  (text_type _r) const noexcept { return text_type(*this) >  _r; }
	bool operator <= (text_type _r) const noexcept { return text_type(*this) <= _r; }
	bool operator >= (text_type _r) const noexcept { return text_type(*this) >= _r; }
	bool operator == (text_type _r) const noexcept { return text_type(*this) == _r; }
	bool operator != (text_type _r) const noexcept { return text_type(*this) != _r; }

	AMTRS_API sstring operator + (text_type _r) const;
	AMTRS_API sstring& operator += (text_type _r) { *this = *this + _r; return *this; }

	AMTRS_API void append(text_type _r);

	void swap(sstring& r)
	{
		mBuff.swap(r.mBuff);
	}

	AMTRS_API size_type find(text_type _s);

	AMTRS_API bool resize(size_type _n);

	template<class R> bool operator == (R const& _r) const noexcept
	{
		return	text_type(data(), size()) == _r;
	}

	template<class R> bool operator != (R const& _r) const noexcept
	{
		return	text_type(data(), size()) != _r;
	}

	void clear()
	{
		mBuff.clear();
	}

	size_type hash() const noexcept
	{
		return	text_type(*this).hash();
	}

private:
	AMTRS_API sstring(intmax_t _n);

	AMTRS_API static value_type const* zero();

	static size_type len(value_type const* _s)
	{
		size_type	i	= 0;
		while (_s[i]) { ++i; }
		return	i;
	}

	super_type	mBuff;
};


// シンプルな文字列
class	wsstring
{
	using	super_type	= simplearray<wchar_t>;
public:
	using	size_type	= std::size_t;
	using	text_type	= wtext;
	using	value_type	= wchar_t;
	static constexpr size_type	npos	= static_cast<size_type>(-1);

	wsstring() = default;
	wsstring(wsstring const&) = default;
	wsstring(wsstring&&) = default;
	wsstring& operator = (wsstring const&) = default;
	wsstring& operator = (wsstring&&) = default;

	static wsstring value_of(intmax_t _n) { return value_of(_n, 10); }
	AMTRS_API static wsstring value_of(intmax_t _n, int _base);

	wsstring(size_type _n)
		: wsstring(nullptr, _n)
	{}

	wsstring(value_type const* _s)
		: wsstring(_s, len(_s))
	{}

	wsstring(value_type const* _s, value_type const* _e)
		: wsstring(_s, (size_type)(_e - _s))
	{}

	AMTRS_API wsstring(value_type const* _s, size_type _n);

	wsstring(text_type const& _s)
		: wsstring(_s.data(), _s.size())
	{}

	AMTRS_API ~wsstring();

	value_type* begin() noexcept { return mBuff.data(); }
	value_type* end() noexcept { return mBuff.data() + size(); }
	value_type const* cbegin() const noexcept { return mBuff.data(); }
	value_type const* cend() const noexcept { return mBuff.data() + size(); }
	value_type const* begin() const noexcept { return cbegin(); }
	value_type const* end() const noexcept { return cend(); }

	value_type* data() noexcept { return mBuff.data() ? mBuff.data() : const_cast<value_type*>(zero()); }
	value_type const* data() const noexcept { return mBuff.data() ? mBuff.data() : zero(); }
	value_type* c_str() noexcept { return data(); }
	value_type const* c_str() const noexcept { return data(); }
	size_type size() const noexcept { return mBuff.empty() ? 0 : mBuff.size() - 1; }
	bool empty() const noexcept { return size() == 0; }

	operator text_type () const noexcept { return {begin(), end()}; }

	AMTRS_API wsstring replace(text_type _from, text_type _to);

	value_type& operator [] (int _n) noexcept { return data()[_n]; }
	value_type const& operator [] (int _n) const noexcept { return data()[_n]; }
	bool operator <  (text_type _r) const noexcept { return text_type(*this) <  _r; }
	bool operator >  (text_type _r) const noexcept { return text_type(*this) >  _r; }
	bool operator <= (text_type _r) const noexcept { return text_type(*this) <= _r; }
	bool operator >= (text_type _r) const noexcept { return text_type(*this) >= _r; }
	bool operator == (text_type _r) const noexcept { return text_type(*this) == _r; }
	bool operator != (text_type _r) const noexcept { return text_type(*this) != _r; }

	AMTRS_API wsstring operator + (text_type _r) const;

	AMTRS_API void append(text_type _r);

	void swap(wsstring& r)
	{
		mBuff.swap(r.mBuff);
	}

	AMTRS_API size_type find(text_type _s);

	AMTRS_API bool resize(size_type _n);

	template<class R> bool operator == (R const& _r) const noexcept
	{
		return	text_type(data(), size()) == _r;
	}

	template<class R> bool operator != (R const& _r) const noexcept
	{
		return	text_type(data(), size()) != _r;
	}

	void clear()
	{
		mBuff.clear();
	}

private:
	AMTRS_API wsstring(intmax_t _n);

	AMTRS_API static value_type const* zero();

	static size_type len(value_type const* _s)
	{
		size_type	i	= 0;
		while (_s[i]) { ++i; }
		return	i;
	}

	super_type	mBuff;
};

AMTRS_NAMESPACE_END
namespace std
{
	template<>
	struct	hash<amtrs::sstring>
	{
	public:
		size_t operator() (amtrs::sstring const& data) const noexcept
		{
			return	data.hash();
		}
	};
}
#endif
