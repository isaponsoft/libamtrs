#ifndef	__libamtrs__string__string__hpp
#define	__libamtrs__string__string__hpp
#ifdef	__cplusplus
#include <algorithm>
#include <iosfwd>
#include "string.h"
#include "string_view.hpp"
AMTRS_NAMESPACE_BEGIN



// ============================================================================
//!	amtrs_string の c++ ラッパー。
// ----------------------------------------------------------------------------
class	string
{
public:
	using	value_type		= char;
	using	element_type	= char;
	using	pointer			= const char*;
	using	const_pointer	= const char*;
	using	string_type		= std::string;
	using	size_type		= std::size_t;
	using	traits_type		= typename string_type::traits_type;

	static const std::size_t	npos = std::string_view::npos;

	explicit string(amtrs_string*&& _a) : mInstance(_a) { _a = nullptr; }

	string() : mInstance(nullptr) {}
	string(const string& _a) : mInstance(amtrs_string_duplicate(_a.mInstance)) {}
	string(string&& _a) : mInstance(_a.mInstance) { _a.mInstance = nullptr; }
	string(const_pointer _s) : mInstance(amtrs_string_create_from_string_view(_s)) {}
	string(amtrs_string_view _s) : mInstance(amtrs_string_create_from_string_view(_s)) {}
	string(std::string_view _s) : mInstance(amtrs_string_create_from_string_view(_s)) {}
	string(const std::string& _s) : mInstance(amtrs_string_create_from_string_view((amtrs_string_view)(std::string_view)_s)) {}

	//!	指定するふたつのビューを連結した文字列を生成します。
	string(string_view _a, string_view _b) : mInstance(amtrs_string_join(_a, _b)) {}
	string(std::string_view _a, std::string_view _b) : mInstance(amtrs_string_join(_a, _b)) {}


	~string()
	{
		clear();
	}


	template<class It>
	static string join(It _first, It _last, std::string_view _sep)
	{
		if (_first == _last)
		{
			return	string();
		}
		if ((_first + 1) == _last)
		{
			return	string(std::string_view(*_first));
		}

		std::size_t	total		= 0;
		auto		it			= _first;
		total	+= std::string_view(*it).size();
		for (++it; it != _last; ++it)
		{
			total	+= _sep.size() + std::string_view(*it).size();
		}

		amtrs_string*	str	= amtrs_string_create(total);
		if (str)
		{
			char*				d	= const_cast<char*>(str->text);
			std::string_view	v;
			it	= _first;
			v	= std::string_view(*it);
			memcpy(d, v.data(), v.size());
			d	+= v.size();
			for (++it; it != _last; ++it)
			{
				memcpy(d, _sep.data(), _sep.size());
				d	+= _sep.size();

				v	= std::string_view(*it);
				memcpy(d, v.data(), v.size());
				d	+= v.size();
			}
		}
		return	string(std::move(str));
	}

	// **************************************************************
	// implicit Convert.
	// **************************************************************

	operator amtrs_string_view () const noexcept { return {data(), size()}; }
	operator std::string_view () const noexcept { return {data(), size()}; }

	template<class Allocator>
	operator std::basic_string<char, traits_type, Allocator> () const { return std::basic_string<char, traits_type, Allocator>(begin(), end()); }

	void clear()
	{
		if (mInstance)
		{
			amtrs_string_destroy(mInstance);
			mInstance = nullptr;
		}
	}

	string& operator = (const string& _a)
	{
		clear();
		if (!_a.empty())
		{
			mInstance	= amtrs_string_duplicate(_a.mInstance);
		}
		return	*this;
	}

	string& operator = (string&& _r)
	{
		clear();
		mInstance		= _r.mInstance;
		_r.mInstance	= nullptr;
		return	*this;
	}

	string& operator = (const_pointer _r)
	{
		clear();
		mInstance	= amtrs_string_create_from_string_view((string_view)_r);
		return	*this;
	}

	string& operator = (amtrs_string_view _r)
	{
		clear();
		if (!_r.empty())
		{
			mInstance	= amtrs_string_create_from_string_view(_r);
		}
		return	*this;
	}

	string& operator = (std::string_view _r)
	{
		clear();
		if (!_r.empty())
		{
			mInstance	= amtrs_string_create_from_string_view(_r);
		}
		return	*this;
	}

	template<class Allocator>
	string& operator = (const std::basic_string<char, traits_type, Allocator>& _r)
	{
		clear();
		mInstance	= amtrs_string_create_from_string_view((std::string_view)_r);
		return	*this;
	}


	//! 文字列同士を比較します。
	bool operator == (const string& _r) const noexcept { return (std::string_view)*this == (std::string_view)_r; }
	bool operator != (const string& _r) const noexcept { return !(*this == _r); }
	bool operator == (std::string_view _r) const noexcept { return size() == _r.size() ? (!std::memcmp(data(), _r.data(), _r.size())) : false; }
	bool operator != (std::string_view _r) const noexcept { return !(*this == _r); }
	bool operator == (const_pointer _r) const noexcept { return (!std::strcmp(data(), _r)); }
	bool operator != (const_pointer _r) const noexcept { return !(*this == _r); }
	template<class Allocator> bool operator == (const std::basic_string<char, std::char_traits<char>, Allocator>& _r) const noexcept { return size() == _r.size() ? (!std::memcmp(data(), _r.data(), _r.size())) : false; }
	template<class Allocator> bool operator != (const std::basic_string<char, traits_type, Allocator>& _r) const noexcept { return !(*this == _r); }


	string operator + (std::string_view _r) const { return string(std::move(amtrs_string_join(*this, _r))); }

	template<class Allocator>
	string operator + (const std::basic_string<char, traits_type, Allocator>& _r) const { return string(std::move(amtrs_string_join((std::string_view)*this, (std::string_view)_r))); }

	string& operator += (const string& _r)
	{
		auto*	n	= amtrs_string_join((std::string_view)*this, (std::string_view)_r);
		clear();
		mInstance	= n;
		return	*this;
	}


	string& operator += (std::string_view _r)
	{
		auto*	n	= amtrs_string_join(*this, _r);
		clear();
		mInstance	= n;
		return	*this;
	}

	string& operator += (const_pointer _r)
	{
		return	*this += std::string_view(_r);
	}

	const value_type& operator [] (std::size_t _index) const { return data()[_index]; }

	const_pointer c_str() const noexcept { return data(); }

	auto data() const noexcept -> const_pointer
	{
		static const value_type	_empty[] = {'\0'};
		return	mInstance == nullptr
				? _empty
				: mInstance->text;
	}


	auto size() const noexcept -> std::size_t { return mInstance == nullptr ? 0 : mInstance->size; }


	auto hash() const noexcept -> std::size_t
	{
		return	mInstance == nullptr
				? std::hash<std::string_view>{}(std::string_view(""))
				: std::hash<std::string_view>{}(amtrs_string_to_string_view(mInstance));
	}


	bool empty() const noexcept { return size() == 0; }
	
	size_type find(const string& _s, size_type _pos = 0) const;
	size_type find(const value_type* _c, size_type _pos, size_type _n) const;
	size_type find(const value_type* _c, size_type _pos = 0) const;
	size_type find(value_type _c, size_type _pos = 0) const { return std::string_view(*this).find_first_of(_c, _pos); }


	size_type rfind(const value_type* s, size_type pos, size_type n) const;
	size_type rfind(std::string_view s, size_type pos = 0) const;
	size_type rfind(const value_type* _s, size_type _pos = 0) const { return rfind(std::string_view(_s), _pos); }


	size_type find_first_of(const value_type* s, std::size_t pos = 0) const;
	size_type find_last_of(value_type c, size_type pos = npos) const noexcept { return ((std::string_view)*this).find_last_of(c, pos); }

	string& replace(size_type _pos, size_type _size, const value_type* _replacement, std::size_t _replacement_size); 
	string& replace(size_type _pos, size_type _size, std::string_view _replacement) { return replace(_pos, _size, _replacement.data(), _replacement.size()); } 

	string substr(std::size_t _pos, std::size_t _size = npos) const { return {((std::string_view)*this).substr(_pos, _size)}; }

	auto begin() const noexcept -> decltype(std::declval<std::string_view&>().cbegin()) { return ((std::string_view)*this).cbegin(); }
	auto end() const noexcept -> decltype(std::declval<std::string_view&>().cend()) { return ((std::string_view)*this).cend(); }
	auto cbegin() const noexcept -> decltype(std::declval<std::string_view&>().cbegin()) { return ((std::string_view)*this).cbegin(); }
	auto cend() const noexcept -> decltype(std::declval<std::string_view&>().cend()) { return ((std::string_view)*this).cend(); }

	auto& front() noexcept { return *begin(); }
	auto& back() noexcept { return *(end()-1); }
	auto& front() const noexcept { return *begin(); }
	auto& back() const noexcept { return *(end()-1); }


private:
	amtrs_string*	mInstance	= nullptr;
};


inline auto operator + (std::string_view _l, const string& _r) -> string
{
	return	string(_l, (std::string_view)_r);
}

inline auto operator + (const char* _l, const string& _r) -> string
{
	return	string((std::string_view)_l, (std::string_view)_r);
}

template<class Allocator>
inline auto operator + (const std::basic_string<char, std::char_traits<char>, Allocator>& _l, const string& _r) -> string
{
	return	string(std::string_view{_l.c_str(), _l.size()}, (std::string_view)_r);
}

template<class T>
class	string_wrapper
		: public string
{
public:
	using	super_type	= string;
	using	super_type::super_type;
	using	super_type::operator ==;
	using	super_type::operator !=;
	using	super_type::operator amtrs_string_view;
	using	super_type::operator std::string_view;

	string_wrapper(super_type&& _r)
		: super_type(_r)
	{}

	T& operator = (T&& _value)
	{
		super_type::operator = (std::move(*static_cast<super_type*>(&_value)));
		return	*static_cast<T*>(this);
	}

	template<class R>
	T& operator = (R _value)
	{
		super_type::operator = (_value);
		return	*static_cast<T*>(this);
	}

	template<class R>
	T operator + (R _value)
	{
		return	T(super_type::operator + (_value));
	}

	template<class R>
	T& operator += (R _value)
	{
		super_type::operator += (_value);
		return	*static_cast<T*>(this);
	}


	T& replace(size_type _pos, size_type _size, const value_type* _replacement, std::size_t _replacement_size)
	{
		super_type::replace(_pos, _size, _replacement, _replacement_size);
		return	*this;
	}

	T& replace(size_type _pos, size_type _size, std::string_view _replacement)
	{
		super_type::replace(_pos, _size, _replacement);
		return	*this;
	}

	T substr(std::size_t _pos, std::size_t _size = npos) const
	{
		auto	retval	= string_wrapper(super_type::substr(_pos, _size));
		return T(std::move(retval));
	}

};

template<class T>
inline auto operator + (std::string_view _l, const string_wrapper<T>& _r) -> T
{
	return	T(_l + *static_cast<const string*>(&_r));
}

template<class T>
inline auto operator + (const char* _l, const string_wrapper<T>& _r) -> T
{
	return	T(_l + *static_cast<const string*>(&_r));
}

template<class T, class Allocator>
inline auto operator + (const std::basic_string<char, std::char_traits<char>, Allocator>& _l, const string_wrapper<T>& _r) -> T
{
	return	T(_l + static_cast<const string*>(&_r));
}



inline std::size_t string::find_first_of(const char* _s, std::size_t _pos) const
{
	if (_pos <= size())
	{
		auto	sz	= traits_type::length(_s);
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



inline auto string::find(const char* _s, size_type _pos) const -> size_type
{
	auto	len		= traits_type::length(_s);
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


inline auto string::rfind(std::string_view _s, size_type _pos) const -> size_type
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

inline auto string::replace(size_type _pos, size_type _size, const char* _replacement, std::size_t _replacement_size) -> string&
{
	std::string	t(data(), size());
	*this	= t.replace(_pos, _size, _replacement, _replacement_size);
	return	*this;
}


AMTRS_NAMESPACE_END
namespace std {


template< class CharT, class Traits>
basic_ostream<CharT, Traits>& operator << (basic_ostream<CharT, Traits>& _os, const AMTRS_NAMESPACE::string& _s)
{
	return	_os << _s.c_str();
}

template<>
struct	hash<::AMTRS_NAMESPACE::string const>
{
	std::size_t operator () (::AMTRS_NAMESPACE::string const& _s) const noexcept
	{
		return	_s.hash();
	}
};


template<class T>
struct	hash<::AMTRS_NAMESPACE::string_wrapper<T> const>
{
	std::size_t operator () (::AMTRS_NAMESPACE::string_wrapper<T> const& _s) const noexcept
	{
		return	_s.hash();
	}
};


}
#endif	// __cplusplus
#endif
