/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__functions__hpp
#define	__libamtrs__io__functions__hpp
AMTRS_IO_NAMESPACE_BEGIN

template<class...>
struct	io_traits;			// prototype. 

template<class T>
struct	__chartype_value_type
{
private:
	template<class C> static auto _match_type(C& c) -> typename C::value_type;
	                  static auto _match_type(...)  -> void;
public:
	using	type	= decltype(_match_type(std::declval<T&>()));
};
template<class T>
struct	__chartype_char_type
{
private:
	template<class C> static auto _match_type(C& c) -> typename C::char_type;
	                  static auto _match_type(...)  -> typename __chartype_value_type<T>::type;
public:
	using	type	= decltype(_match_type(std::declval<T&>()));
};


template<class T>
struct	__sizetype_default
{
	using	type	= std::streamsize;
};
template<class T>
struct	__sizetype_size_type
{
private:
	template<class C> static auto _match_type(C& c) -> typename C::size_type;
	                  static auto _match_type(...)  -> typename __sizetype_default<T>::type;
public:
	using	type	= decltype(_match_type(std::declval<T&>()));
};



template<class T>
struct	__iotest
{
	using	size_type	= typename __sizetype_size_type<T>::type;
	using	char_type	= typename __chartype_char_type<T>::type;
private:
	template<class C> static auto _read_rs(C& c) -> decltype(static_cast<size_type>(c.read(std::declval<char_type*>(), std::declval<size_type>())), std::true_type{});
	                  static auto _read_rs(... ) -> std::false_type;
	template<class C> static auto _read_gs(C& c) -> decltype(static_cast<size_type>(c.read(std::declval<char_type*>(), std::declval<size_type>()).gcount()), std::true_type{});
	                  static auto _read_gs(... ) -> std::false_type;


	template<class C> static auto _write_rs(C& c) -> decltype(static_cast<size_type>(c.write(std::declval<char_type*>(), std::declval<size_type>())), std::true_type{});
	                  static auto _write_rs(... ) -> std::false_type;
	template<class C> static auto _write_ps(C& c) -> decltype(static_cast<size_type>(c.write(std::declval<char_type*>(), std::declval<size_type>()).pcount()), std::true_type{});
	                  static auto _write_ps(... ) -> std::false_type;
	template<class C> static auto _write_only(C& c) -> decltype(c.write(std::declval<char_type*>(), std::declval<size_type>()), std::true_type{});
	                  static auto _write_only(... ) -> std::false_type;


	template<class C> static auto _size(C& c) -> decltype(static_cast<size_type>(c.size()), std::true_type{});
	                  static auto _size(... ) -> std::false_type;

	template<class C> static auto _seek(C& c) -> decltype(c.seek(0, std::ios::beg), std::true_type{});
	                  static auto _seek(... ) -> std::false_type;
	template<class C> static auto _tell(C& c) -> decltype(static_cast<size_type>(c.tell()), std::true_type{});
	                  static auto _tell(... ) -> std::false_type;
	template<class C> static auto _seekg(C& c) -> decltype(c.seekg(0, std::ios::beg), std::true_type{});
	                  static auto _seekg(... ) -> std::false_type;
	template<class C> static auto _tellg(C& c) -> decltype(static_cast<size_type>(c.tellg()), std::true_type{});
	                  static auto _tellg(... ) -> std::false_type;
	template<class C> static auto _seekp(C& c) -> decltype(c.seekp(0, std::ios::beg), std::true_type{});
	                  static auto _seekp(... ) -> std::false_type;
	template<class C> static auto _tellp(C& c) -> decltype(static_cast<size_type>(c.tellp()), std::true_type{});
	                  static auto _tellp(... ) -> std::false_type;
public:
	static constexpr decltype(_read_rs(std::declval<T&>()))		has_read_return_size	= decltype(_read_rs(std::declval<T&>())){};
	static constexpr decltype(_read_gs(std::declval<T&>()))		has_read_and_gcount		= decltype(_read_gs(std::declval<T&>())){};

	static constexpr decltype(_write_rs(std::declval<T&>()))	has_write_return_size	= decltype(_write_rs(std::declval<T&>())){};
	static constexpr decltype(_write_ps(std::declval<T&>()))	has_write_and_pcount	= decltype(_write_ps(std::declval<T&>())){};
	static constexpr decltype(_write_only(std::declval<T&>()))	has_write_only			= decltype(_write_only(std::declval<T&>())){};

	static constexpr decltype(_size(std::declval<T&>()))		has_size				= decltype(_size(std::declval<T&>())){};

	static constexpr decltype(_seek(std::declval<T&>()))		has_seek				= decltype(_seek(std::declval<T&>())){};
	static constexpr decltype(_tell(std::declval<T&>()))		has_tell				= decltype(_tell(std::declval<T&>())){};
	static constexpr decltype(_seekg(std::declval<T&>()))		has_seekg				= decltype(_seekg(std::declval<T&>())){};
	static constexpr decltype(_tellg(std::declval<T&>()))		has_tellg				= decltype(_tellg(std::declval<T&>())){};
	static constexpr decltype(_seekp(std::declval<T&>()))		has_seekp				= decltype(_seekp(std::declval<T&>())){};
	static constexpr decltype(_tellp(std::declval<T&>()))		has_tellp				= decltype(_tellp(std::declval<T&>())){};

};



// read basic.
template<class CharT, class In>
auto read(CharT* _buff, In& _in, size_t _size)
{
	using	intype	= typename std::remove_reference<In>::type;
	// support read(CharT*, size_t) -> size_type
	if constexpr (__iotest<intype>::has_read_return_size)
	{
		return	_in.read(_buff, _size);
	}
	// support read(CharT*, size_t).gcount()
	else if constexpr (__iotest<intype>::has_read_and_gcount)
	{
		return	_in.read(_buff, _size).gcount();
	}
	// other
	else
	{
		return	io_traits<intype>{}.read(_buff, _in, _size);
	}
}



// T read<T>(in, size) -> read(buff, in, size)
template<class T, class In>
T read(In& _in, size_t _size)
{
	T		retval;
	retval.resize(_size);
	auto	rs	= read(retval.data(), _in, _size);
	if (rs == 0)
	{
		return	T{};
	}
	if (rs != _size)
	{
		retval.resize(rs);
	}
	return	retval;
}



// T read(buff[N], in) -> read(buff, in, N)
template<class CharT, class T, size_t N>
auto read(CharT (&_data)[N], T& _in)
{
	CharT*	data	= static_cast<CharT*>(_data);
	return	read(data, _in, N);
}



template<endian E, class T, class In>
bool read(T& _buff, In& _in)
{
	auto	s	= read((typename In::char_type*)&_buff, _in, sizeof(T));
	if (s != sizeof(T))
	{
		return	false;
	}
	_buff	= endian_util::decode<E>(_buff);
	return	true;
}




// 読み込みを行い、_csumと一致するか調べる
template<class In, size_t CS>
bool read_checksum(In& _in, typename In::char_type const (& _csum)[CS])
{
	char	buff[CS];
	auto	rs	= read(buff, _in, CS);
	return	rs == CS
			? (bool)(!memcmp(buff, _csum, CS))
			: false;
}



template<class CharT, class Out>
auto write(Out& _out, CharT const* _data, size_t _size)
{
	using	type	= typename std::remove_reference<Out>::type;
	// support write(CharT*, size_t) -> size_type
	if constexpr (__iotest<type>::has_write_return_size)
	{
		return	_out.write(_data, _size);
	}
	// support write(CharT*, size_t).pcount()
	else if constexpr (__iotest<type>::has_write_and_pcount)
	{
		return	_out.write(_data, _size).pcount();
	}
	// support write(CharT*, size_t)
	else if constexpr (__iotest<type>::has_write_only)
	{
		_out.write(_data, _size);
		return	_size;
	}
	// other
	else
	{
		return	io_traits<type>{}.write(_out, _data, _size);
	}
}



template<class CharT, size_t N, class T>
auto write(T& _out, CharT const (&_data)[N])
{
	return	write(_out, _data, N);
}


template<class T, class I>
auto write(T& _out, I const& _data)
{
	return	write(_out, _data.data(), _data.size());
}


template<class T, class Pos, class Dir>
bool seek(T&& _s, Pos _pos, Dir _dir)
{
	using	type	= typename std::remove_reference<T>::type;
	// support size() function.
	if constexpr (__iotest<type>::has_seek)
	{
		_s.seek(_pos, _dir);
		return	_s.good();
	}
	// support tellg() and seekg() functions.
	else if constexpr (__iotest<type>::has_seekg && __iotest<type>::has_seekp)
	{
		_s.seekg(_pos, _dir);
		_s.seekp(_pos, _dir);
		return	_s.good();
	}
	else
	{
		static_assert("seek unsupported");
		return	false;
	}
}



template<class In>
size_t size(In& _in)
{
	using	type	= typename std::remove_reference<In>::type;
	// support size() function.
	if constexpr (__iotest<type>::has_size)
	{
		return	_in.size();
	}
	// support tellg() and seekg() functions.
	else if constexpr (__iotest<type>::has_seekg && __iotest<type>::has_tellg)
	{
		auto	current	= _in.tellg();

		_in.seekg(0, std::ios::end);
		auto	endPos	= _in.tellg();

		_in.seekg(0, std::ios::beg);
		auto	begPos	= _in.tellg();

		_in.seekg(current, std::ios::beg);
		return	static_cast<size_t>(endPos - begPos);
	}
	else
	{
		static_assert("size unsupported");
		return	false;
	}
}



// copy to "_dest" from "_src".
template<class D, class S>
auto copy(D& _dest, S& _src, size_t _size)
{
	size_t	len	= 0;
	while (_dest && _src && len < _size)
	{
		char	buff[1024];
		auto	sz	= read(buff, _src, std::min<size_t>(_size - len, sizeof(buff)));
		while (sz > 0 && _dest)
		{
			auto	ws	= write(_dest, buff, sz);
			len	+= ws;
			if (!_src)
			{
				break;
			}
		}
	}
	return	len;
}






AMTRS_IO_NAMESPACE_END
#endif
