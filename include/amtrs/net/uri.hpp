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


	Reference
		https://tools.ietf.org/html/rfc3986

******************************************************************************/
#ifndef	__libamtrs__net__uri__hpp
#define	__libamtrs__net__uri__hpp
#include <cctype>
#include "../string/find.hpp"
#include "def.hpp"
AMTRS_NET_NAMESPACE_BEGIN namespace uri {



template<class CharT>
using	view_t	= std::basic_string_view<CharT>;


template<class CharT = char>
struct	info
{
	using	view_type	= view_t<CharT>;


	view_type	uri;
	view_type	scheme;
	view_type	authority;
	view_type	userinfo;
	view_type	username;
	view_type	password;
	view_type	host;
	view_type	port;
	view_type	path;
	view_type	query;
	view_type	fragment;


	info() = default;
	info(const info&) = default;
	info& operator = (const info&) = default;

	info(view_type _uri);
};


//! クエリイテレータ
template<class CharT>
class	query_param
		: public view_t<CharT>
{
public:
	using	view_type	= view_t<CharT>;
	using	value_type	= view_type;

	query_param() = default;
	query_param(const view_type& _r) : view_type(_r) {}
	query_param& operator = (const view_type& _r) { *static_cast<view_type*>(this) = _r; return *this; }

	template<class... T>
	query_param(T&&... _args)
		: view_type(std::forward<T>(_args)...)
	{}

	view_type field() const noexcept
	{
		auto	pos	= view_type::find_first_of('=');
		if (pos == view_type::npos)
		{
			return	view_type::substr(0, 0);
		}
		return	view_type::substr(0, pos);
	}

	view_type value() const noexcept
	{
		auto	pos	= view_type::find_first_of('=');
		if (pos == view_type::npos)
		{
			return	*this;
		}
		return	view_type::substr(pos + 1);
	}
};


//! クエリイテレータ
template<class CharT>
class	query_iterator
{
public:
	using	view_type	= view_t<CharT>;
	using	value_type	= query_param<CharT>;

	class	iterator
	{
	public:
		iterator() = default;
		iterator(const iterator&) = default;
		iterator& operator = (const iterator&) = default;

		const value_type& operator * () const noexcept { return  get(); }
		const value_type* operator ->() const noexcept { return &get(); }

		iterator& operator ++ () noexcept
		{
			mCursor	= mCursor.substr(get().size());
			if (!mCursor.empty() && mCursor[0] == '&')
			{
				mCursor	= mCursor.substr(1);
			}
			return	*this;
		}

		iterator operator ++ (int) noexcept
		{
			iterator	ret(*this);
			++(*this);
			return	ret;
		}

		bool operator == (const iterator& _r) const noexcept { return mCursor == _r.mCursor; }
		bool operator != (const iterator& _r) const noexcept { return mCursor != _r.mCursor; }
	private:
		iterator(view_type _query)
			: mCursor((value_type)_query)
		{}

		const value_type& get() const noexcept
		{
			if (mValue.data() != mCursor.data())
			{
				auto	pos	= mCursor.find_first_of('&');
				if (pos == value_type::npos)
				{
					mValue	= mCursor;
				}
				else
				{
					mValue	= value_type(mCursor.data(), pos);
				}
			}
			return	mValue;
		}

		mutable value_type	mValue	= {nullptr, 0};
		value_type			mCursor;

		friend	query_iterator;
	};

	query_iterator() = default;
	query_iterator(const query_iterator&) = default;
	query_iterator& operator = (const query_iterator&) = default;

	query_iterator(view_type _query)
		: mQuery(_query)
	{}

	auto begin() const noexcept { return iterator(mQuery); }
	auto end() const noexcept { return iterator(view_type(mQuery.data() + mQuery.size(), 0)); }

private:
	view_type	mQuery;
};



template<class CharT = char>
auto parse(info<CharT>& _buff, view_t<CharT> _uri) -> view_t<CharT>;


// ============================================================================
//! URLからスキームを抜き出します。
// ----------------------------------------------------------------------------
template<class CharT = char>
auto scheme(view_t<CharT> _uri) -> view_t<CharT>;


// ============================================================================
//! ユーザー情報(USERNAME:PASSWORD)を抜き出します。
// ----------------------------------------------------------------------------
//! パスワードの使用はセキュリティの観点から非推奨です。
// ----------------------------------------------------------------------------
template<class CharT = char>
auto userinfo(view_t<CharT> _uri) -> view_t<CharT>;








// ============================================================================
//! IPv4Address で始まっているならIPv4Address を示す範囲を返します。
// ----------------------------------------------------------------------------
//!	dec-octet "." dec-octet "." dec-octet "." dec-octet
// ----------------------------------------------------------------------------
template<class CharT = char>
auto ipv4address_starts_with(view_t<CharT> _uri) -> view_t<CharT>;


// ============================================================================
//! IPv6Address で始まっているならIPv6Address を示す範囲を返します。
// ----------------------------------------------------------------------------
//! そうでないなら empty() を返します。
// ----------------------------------------------------------------------------
template<class CharT = char>
auto ipv6address_starts_with(view_t<CharT> _uri) -> view_t<CharT>;





//! %XXかどうか調べます。
template<class CharT = char>
auto pct_starts_with(view_t<CharT> _a) -> bool
{
	if (_a.size() < 3)
	{
		return	false;
	}
	return	(_a[0] == '%') && (std::isxdigit(_a[1])) && (std::isxdigit(_a[2]));
}


//! gen-delimsか調べます
inline bool isgendelims(char _c)
{
	return	_c == ':' || _c == '/' || _c == '?' || _c == '#'
		||	_c == '[' || _c == ']' || _c == '@';
}


//! sub-delimsか調べます
inline bool issubdelims(char _c)
{
	return	_c == '!' || _c == '$' || _c == '&' || _c == '\''
		||	_c == '(' || _c == ')'
		||	_c == '*' || _c == '+' || _c == ',' || _c == ';' || _c == '=';
}


//! reservedか調べます
inline bool isreserved(char _c)
{
	return	isgendelims(_c) || issubdelims(_c);
}


//! unreservedか調べます
inline bool isunreserved(char _c)
{
	return	std::isalpha(_c)
		||	std::isdigit(_c)
		||	(_c == '-')
		||	(_c == '.')
		||	(_c == '_')
		||	(_c == '~');
}



//! ? か調べます
inline bool isquestionmark(char _c)
{
	return	_c == '?';
}


// ============================================================================
//! スキームとして認識可能かどうか調べます。
// ----------------------------------------------------------------------------
template<class CharT = char>
auto is_scheme(view_t<CharT> _scheme) -> bool
{
	if (_scheme.size() == 0)
	{
		return	false;
	}

	auto	cur	= _scheme.data();
	auto	end	= _scheme.data() + _scheme.size();
	if (!std::isalpha(*cur++))
	{
		return	false;
	}
	while (cur != end)
	{
		auto	c	= *cur++;
		if (!std::isalpha(c) && !std::isdigit(c) && (c != '+') && (c != '-') && (c != '.'))
		{
			return	false;
		}
	}
	return	true;
}


// ============================================================================
//! スキームを抜き出します。
// ----------------------------------------------------------------------------
template<class CharT>
auto scheme(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	pos	= find_string_first_of(_uri, view_t<CharT>("://"));
	if (pos == view_t<CharT>::npos)
	{
		return	_uri.substr(0, 0);
	}
	auto	scm	= _uri.substr(0, pos);
	return	is_scheme(scm)
			? scm
			: _uri.substr(0, 0);
}





// ============================================================================
//! ユーザー情報(USERNAME:PASSWORD)を抜き出します。
// ----------------------------------------------------------------------------
//! userinfo = unreserved / pct-encoded / sub-delims / ":" paaword
//! パスワードの使用はセキュリティの観点から非推奨です。
// ----------------------------------------------------------------------------
template<class CharT>
auto userinfo_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	cur	= _uri.data();
	auto	end	= cur + _uri.size();
	while (cur != end)
	{
		if (pct_starts_with(view_t<CharT>(cur, end - cur)))
		{
			cur += 3;
			continue;
		}

		auto	c	= *cur;
		if (!isunreserved(c) && !issubdelims(c) && (c != ':'))
		{
			break;
		}
		++cur;
	}
	return	_uri.substr(0, cur - _uri.data());
}


// ============================================================================
//! userinfo = unreserved / pct-encoded / sub-delims / ":" paaword
//! password = 非推奨
// ----------------------------------------------------------------------------
template<class CharT = char>
auto is_userinfo(view_t<CharT> _uri) -> bool
{
	return	userinfo_starts_with(_uri) == _uri;
}



// ============================================================================
//! ユーザー情報(USERNAME:PASSWORD)を抜き出します。
// ----------------------------------------------------------------------------
//! パスワードの使用はセキュリティの観点から非推奨です。
// ----------------------------------------------------------------------------
template<class CharT>
auto userinfo(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	scm		= scheme(_uri);
	if (scm.empty())
	{
		return	scm;
	}
	auto	uinfo	= _uri.substr(scm.size() + 3);
	auto	spltpos	= uinfo.find_first_of(view_t<CharT>("/@"));
	if (spltpos != view_t<CharT>::npos)
	{
		uinfo	= uinfo.substr(0, spltpos);
	}
	return	uinfo;
}





// ============================================================================
//! ユーザー情報(USERNAME:PASSWORD)を抜き出します。
// ----------------------------------------------------------------------------
//! username = unreserved / pct-encoded / sub-delims
//! パスワードの使用はセキュリティの観点から非推奨です。
// ----------------------------------------------------------------------------
template<class CharT>
auto username_starts_with(view_t<CharT> _userinfo) -> view_t<CharT>
{
	auto	cur	= _userinfo.data();
	auto	end	= cur + _userinfo.size();
	while (cur != end)
	{
		if (pct_starts_with(view_t<CharT>(cur, end - cur)))
		{
			cur += 3;
			continue;
		}

		auto	c	= *cur;
		if (!isunreserved(c) && !issubdelims(c))
		{
			break;
		}
		++cur;
	}
	return	_userinfo.substr(0, cur - _userinfo.data());
}


// ============================================================================
//! ユーザー情報(USERNAME:PASSWORD)を抜き出します。
// ----------------------------------------------------------------------------
//! username = unreserved / pct-encoded / sub-delims
//! パスワードの使用はセキュリティの観点から非推奨です。
// ----------------------------------------------------------------------------
template<class CharT>
auto password_starts_with(view_t<CharT> _userinfo) -> view_t<CharT>
{
	auto	cur	= _userinfo.data();
	auto	end	= cur + _userinfo.size();
	while (cur != end)
	{
		if (pct_starts_with(view_t<CharT>(cur, end - cur)))
		{
			cur += 3;
			continue;
		}

		auto	c	= *cur;
		if (!isunreserved(c) && !issubdelims(c) && (c != ':'))
		{
			break;
		}
		++cur;
	}
	return	_userinfo.substr(0, cur - _userinfo.data());
}




template<class CharT = char>
auto h16_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	size_t	len	= 0;
	while (len < 4 && len < _uri.size())
	{
		if (!std::isxdigit(_uri[len]))
		{
			break;
		}
		++len;
	}
	return	_uri.substr(0, len);
}


//!	DIGIT				; 0-9
//!	%x31-39 DIGIT		; 10-99
//!	'1' 2DIGIT			; 10-99
//!	'2' %x30-34 DIGIT
//!	'25' %x30-35
template<class CharT = char>
auto decoctet_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	if (_uri.size() == 0)
	{
		return	_uri;
	}

	auto	c	= _uri[0];

	// 0
	if (c == '0')
	{
		return	_uri.substr(0, 1);
	}

	// 1, 1[0-9][0-9]
	if (c == '1')
	{
		if ((_uri.size() == 1) || !std::isdigit(_uri[1]))
		{
			return	_uri.substr(0, 1);
		}
		if ((_uri.size() == 2) || !std::isdigit(_uri[2]))
		{
			return	_uri.substr(0, 2);
		}
		return	_uri.substr(0, 3);
	}


	// 2 or 2[0-9] or 2[0-5][0-9]
	if (c == '2')
	{
		// 2
		if ((_uri.size() == 1) || !std::isdigit(_uri[1]))
		{
			return	_uri.substr(0, 1);
		}

		// 20-29 !DIGIT
		if ((_uri.size() == 2) || !std::isdigit(_uri[2]))
		{
			return	_uri.substr(0, 2);
		}
		
		// 2[0-4][0-9]
		if (_uri[1] < '5')
		{
			return	_uri.substr(0, 3);
		}

		// 25[0-4]
		if (_uri[1] == '5' && (_uri[2] <= '4'))
		{
			return	_uri.substr(0, 3);
		}

		// 2[6-9]
		return	_uri.substr(0, 2);
	}


	if (!std::isdigit(c))
	{
		return	_uri.substr(0, 0);
	}

	// [3-9] !DIG
	if ((_uri.size() == 1) || (!std::isdigit(_uri[1])))
	{
		return	_uri.substr(0, 1);
	}

	return	_uri.substr(0, 2);
}



//!	dec-octet "." dec-octet "." dec-octet "." dec-octet
template<class CharT>
auto ipv4address_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	// size < size(0.0.0.0)
	if (_uri.size() < 7)
	{
		return	_uri.substr(0, 0);
	}
	auto	uri	= _uri;
	auto	dec	= decoctet_starts_with(uri);
	if (dec.empty())
	{
		return	_uri.substr(0, 0);
	}
	for (int i = 0; i < 3; ++i)
	{
		uri = uri.substr(dec.size());
		if ((uri.size() < 2) || (uri[0] != '.'))
		{
			return	_uri.substr(0, 0);
		}
		uri	= uri.substr(1);
		dec	= decoctet_starts_with(uri);
		if (dec.empty())
		{
			return	_uri.substr(0, 0);
		}
	}
	return	_uri.substr(0, (uri.data() + dec.size()) - _uri.data());
}


//! ( h16 ":" h16 )
//!	IPv4address
template<class CharT = char>
auto ls32_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	// check h16:h16
	{
		auto	uri	= _uri;
		if (auto a = h16_starts_with(uri); !a.empty())
		{
			uri	= uri.substr(a.size());
			if ((uri.size() > 1) && (uri[0] == ':'))
			{
				uri	= uri.substr(1);
				if (auto b = h16_starts_with(uri); !b.empty())
				{
					return	_uri.substr(0, a.size() + 1 + b.size());
				}
			}
		}
	}

	// check ipv4
	if (auto ipv4 = ipv4address_starts_with(_uri); !ipv4.empty())
	{
		return	ipv4;
	}

	return	_uri.substr(0, 0);
}


//!
//!                            6( h16 ":" ) ls32
//!                       "::" 5( h16 ":" ) ls32
//!
//! [               h16 ] "::" 4( h16 ":" ) ls32
//! [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
//! [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
//! [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
//! [ *4( h16 ":" ) h16 ] "::"              ls32
//! [ *5( h16 ":" ) h16 ] "::"              h16
//! [ *6( h16 ":" ) h16 ] "::"
template<class CharT>
auto ipv6address_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	// "h16:" * _count
	auto	check_h16 = [](view_t<CharT> _uri, size_t _count) -> view_t<CharT>
	{
		auto	uri	= _uri;
		for (int i = 0; i < _count; ++i)
		{
			auto	t	= h16_starts_with(uri); 
			if (t.empty())
			{
				return	_uri.substr(0, 0);
			}
			uri	= uri.substr(t.size());
			if ((uri.size() < 1) || (uri[0] != ':'))
			{
				return	_uri.substr(0, 0);
			}
			uri	= uri.substr(1);
		}
		return	_uri.substr(0, uri.data() - _uri.data());
	};

	// count[h16:]h16  count <= _limit
	auto	count_h16 = [](view_t<CharT> _uri, size_t _limit = 6) -> std::pair<size_t, view_t<CharT>>
	{
		size_t	count	= 0;
		auto	uri		= _uri;
		for (;count < _limit; ++count)
		{
			auto	t	= h16_starts_with(uri); 
			if (t.empty())
			{
				return	{0, _uri.substr(0, 0)};
			}
			uri	= uri.substr(t.size());
			if ((uri.size() < 1) || (uri[0] != ':'))
			{
				return	{count, _uri.substr(0, uri.data() - _uri.data())};
			}
			uri	= uri.substr(1);
		}

		auto	t	= h16_starts_with(uri); 
		if (t.empty())
		{
			return	{0, _uri.substr(0, 0)};
		}
		return	{count, _uri.substr(0, (t.data() + t.size()) - _uri.data()) };
	};



	// 6(h16:)ls32
	if (auto t = check_h16(_uri, 6); !t.empty())
	{
		t	= ls32_starts_with(_uri.substr((t.data() + t.size()) - _uri.data()));
		if (!t.empty())
		{
			return	_uri.substr(0, (t.data() + t.size()) - _uri.data());
		}
	}

	// ::5(h16:)ls32
	if (_uri.size() > 2 && starts_with(_uri, (view_t<CharT>)"::"))
	{
		if (auto t = check_h16(_uri.substr(2), 5); !t.empty())
		{
			t	= ls32_starts_with(_uri.substr((t.data() + t.size()) - _uri.data()));
			if (!t.empty())
			{
				return	_uri.substr(0, (t.data() + t.size()) - _uri.data());
			}
		}
	}

	if ((_uri.size() < 3) || (_uri[0] != '['))
	{
		return	_uri.substr(0, 0);
	}

	auto	r	= count_h16(_uri.substr(1));
	if (r.second.empty())
	{
		return	_uri.substr(0, 0);
	}
	auto	t	= _uri.substr((r.second.data() + r.second.size()) - _uri.data());
	if (t.size() == 0)
	{
		return	_uri.substr(0, 0);
	}
	if (t[0] != ']')
	{
		return	_uri.substr(0, 0);
	}
	t	= t.substr(1);
	if ((t.size() < 2) || !starts_with(t, (view_t<CharT>)"::"))
	{
		return	_uri.substr(0, 0);
	}
	t	= t.substr(2);
	if (r.first == 6)
	{
		return	_uri.substr(0, t.data() - _uri.data());
	}


	if (r.first <= 4)
	{
		if (r.first < 4)
		{
			t	= check_h16(t, 4 - r.first);
			if (t.empty())
			{
				return	_uri.substr(0, 0);
			}
		}
		t	= ls32_starts_with(_uri.substr((t.data() + t.size()) - _uri.data()));
		if (t.empty())
		{
			return	_uri.substr(0, 0);
		}
		return	_uri.substr(0, (t.data() + t.size()) - _uri.data());
	}

	if (r.first == 5)
	{
		t	= h16_starts_with(t); 
		if (t.empty())
		{
			return	_uri.substr(0, 0);
		}
		return	_uri.substr(0, (t.data() + t.size()) - _uri.data());
	}

	return	_uri.substr(0, 0);
}


template<class CharT = char>
auto port_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	cur	= _uri.data();
	auto	end	= _uri.data() + _uri.size();
	while (cur != end)
	{
		auto	c	= *cur;
		if (std::isdigit(c))
		{
			++cur;
			continue;
		}
		break;
	}
	return	_uri.substr(0, cur - _uri.data());
}


template<class CharT = char>
auto regname_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	cur	= _uri.data();
	auto	end	= _uri.data() + _uri.size();
	while (cur != end)
	{
		auto	c	= *cur;
		if (isunreserved(c) || issubdelims(c))
		{
			++cur;
			continue;
		}
		if (pct_starts_with(_uri.substr(0, end-cur)))
		{
			cur += 3;
			continue;
		}
		break;
	}
	return	_uri.substr(0, cur - _uri.data());
}


template<class CharT = char>
auto host_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	if (auto ipv4 = ipv4address_starts_with(_uri); !ipv4.empty())
	{
		return	ipv4;
	}
	if (auto ipv6 = ipv6address_starts_with(_uri); !ipv6.empty())
	{
		return	ipv6;
	}
	if (auto regn = regname_starts_with(_uri); !regn.empty())
	{
		return	regn;
	}
	return	_uri.substr(0, 0);
}


template<class CharT = char>
auto pchar_starts_with(view_t<CharT> _c) -> view_t<CharT>
{
	if (_c.empty())
	{
		return	_c;
	}
	if (pct_starts_with(_c))
	{
		return	_c.substr(0, 3);
	}
	auto	c	= _c[0];
	return	isunreserved(c) || issubdelims(c) || (c == ':') || (c == '@')
			? _c.substr(0, 1)
			: _c.substr(0, 0);
}



template<class CharT = char>
auto path_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	uri	= _uri;
	for (;;)
	{
		auto	pc	= pchar_starts_with(uri);
		if (!pc.empty())
		{
			uri	= uri.substr(pc.size());
			continue;
		}

		if (uri.size() > 0 && uri[0] == '/')
		{
			uri	= uri.substr(1);
			continue;
		}

		break;
	}
	return	_uri.substr(0, uri.data() - _uri.data());
}



template<class CharT = char>
auto query_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	uri	= _uri;
	for (;;)
	{
		auto	pc	= pchar_starts_with(uri);
		if (!pc.empty())
		{
			uri	= uri.substr(pc.size());
			continue;
		}

		if ((uri.size() > 0) && ((uri[0] == '/') || (uri[0] == '?')))
		{
			uri	= uri.substr(1);
			continue;
		}

		break;
	}
	return	_uri.substr(0, uri.data() - _uri.data());
}


template<class CharT = char>
auto fragment_starts_with(view_t<CharT> _uri) -> view_t<CharT>
{
	auto	uri	= _uri;
	for (;;)
	{
		auto	pc	= pchar_starts_with(uri);
		if (!pc.empty())
		{
			uri	= uri.substr(pc.size());
			continue;
		}

		if ((uri.size() > 0) && ((uri[0] == '/') || (uri[0] == '?')))
		{
			uri	= uri.substr(1);
			continue;
		}

		break;
	}
	return	_uri.substr(0, uri.data() - _uri.data());
}





template<class CharT>
auto parse(info<CharT>& _buff, view_t<CharT> _uri) -> view_t<CharT>
{
	_buff.scheme	= {};
	_buff.authority	= {};
	_buff.userinfo	= {};
	_buff.username	= {};
	_buff.password	= {};
	_buff.host		= {};
	_buff.port		= {};
	_buff.path		= {};
	_buff.query		= {};
	_buff.fragment	= {};


	auto	empty	= _uri.substr(0, 0);
	auto	uri		= _uri;

	auto	next	= [](view_t<CharT> _uri, view_t<CharT> _section, size_t _off = 0) -> view_t<CharT>
					{
						return	_uri.substr((_section.data() + _section.size()) - _uri.data() + _off);
					};

	// scheme://
	_buff.scheme	= scheme(uri);
	if (_buff.scheme.empty())
	{
		return	empty;
	}
	uri				= next(uri, _buff.scheme, 3);


	// authority
	// => userinfo
	//	  => username
	//    => password
	// => host
	//    => hostname
	//    => port

	// userinfo があるか調べる
	auto	userinfo	= userinfo_starts_with(uri);
	if (!userinfo.empty())
	{
		// userinfo の後ろ側を調べる
		auto	t	= uri.substr(userinfo.data() + userinfo.size() - uri.data());
		if (t.size() >= 1)
		{
			if (t[0] == '@')
			{
				// userinfo 確定
				uri				= next(uri, userinfo, 1);
				_buff.userinfo	= userinfo;
				_buff.username	= username_starts_with(userinfo);
				auto	uif		= next(userinfo, _buff.username);
				if (uif.size() > 0)
				{
					if (uif[0] != ':')
					{
						return	empty;
					}
					_buff.password	= password_starts_with(uif.substr(1));
					if (!next(uif, _buff.password).empty())
					{
						return	empty;
					}
				}

			}
		}
	}

	_buff.host		= host_starts_with(uri);
	if (_buff.host.empty())
	{
		return	empty;
	}
	uri				= next(uri, _buff.host);
	if (uri.empty())
	{
		return	_uri.substr(0, uri.data() - _uri.data());
	}

	// ポート
	if (uri[0] == ':')
	{
		uri			= uri.substr(1);
		_buff.port	= port_starts_with(uri);
		uri			= next(uri, _buff.port);
	}

	_buff.authority	= _uri.substr(userinfo.data() - _uri.data(), uri.data() - userinfo.data());


	// path
	if (!uri.empty() && uri[0] == '/')
	{
		_buff.path		= path_starts_with(uri);
		uri				= next(uri, _buff.path);
	}

	// query
	if (!uri.empty() && isquestionmark(uri[0]))
	{
		uri			= uri.substr(1);
		_buff.query	= query_starts_with(uri);
		uri			= next(uri, _buff.query);
	}

	// fragment
	if (!uri.empty() && (uri[0] == '#'))
	{
		uri			= uri.substr(1);
		_buff.fragment	= fragment_starts_with(uri);
		uri				= next(uri, _buff.fragment);
	}

	_buff.uri	= _uri.substr(0, uri.data() - _uri.data());;
	return	_buff.uri;
}

template<class CharT>
info<CharT>::info(view_type _uri)
{
	parse(*this, _uri);
}




} AMTRS_NET_NAMESPACE_END
#endif
