/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__vstream__hpp
#define	__libamtrs__io__vstream__hpp
#include "stream_in-fwd.hpp"
AMTRS_IO_NAMESPACE_BEGIN

template<class CharT>
class	vstream
{
public:
	using	char_type	= CharT;
	using	size_type	= size_t;
	using	iostate		= std::ios_base::iostate;
	using	off_type	= typename std::basic_iostream<CharT>::off_type;
	using	pos_type	= typename std::basic_iostream<CharT>::pos_type;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	vstream() = default;
	vstream(vstream const&) = default;
	vstream(vstream&&) = default;
	vstream& operator = (vstream const&) = default;
	vstream& operator = (vstream&&) = default;
	template<class S> vstream(S&& _stream) : mStream(std::forward<S>(_stream)) {}



	std::size_t gcount() const noexcept { return (std::size_t)mStream->on_gcount(); }
	std::size_t pcount() const noexcept { return (std::size_t)mStream->on_pcount(); }

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == std::ios::goodbit; }
	bool eof()  const noexcept { return rdstate() & std::ios::eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & std::ios::failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & std::ios::badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStream->on_rdstate(); }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept { mStream->on_clear(_state); }

	bool empty() const noexcept
	{
		return	mStream.empty();
	}

	vstream& read(char_type* _buff, size_t _n)
	{
		mStream->on_read(_buff, _n);
		return	*this;
	}

	vstream& write(char_type const* _buff, size_t _n)
	{
		mStream->on_write(_buff, _n);
		return	*this;
	}

	class	vstream_base
			: public ref_object
	{
	public:
		using	char_type	= CharT;
		using	size_type	= size_t;
		using	iostate		= std::ios_base::iostate;
		using	off_type	= typename std::basic_iostream<CharT>::off_type;
		using	pos_type	= typename std::basic_iostream<CharT>::pos_type;

		virtual void on_read(char_type* _buff, size_type _n) = 0;
		virtual void on_write(char_type const* _buff, size_type _n) = 0;
		virtual size_type on_gcount() const noexcept = 0;
		virtual size_type on_pcount() const noexcept = 0;
		virtual iostate on_rdstate() const noexcept = 0;
		virtual void on_setstate(iostate _state) noexcept = 0;
		virtual void on_clear(iostate _state = goodbit) noexcept = 0;
		virtual pos_type on_tellg() = 0;
		virtual void on_seekg(off_type _off, std::ios_base::seekdir _dir) = 0;
	};

protected:
	ref<vstream_base>	mStream;
};

struct	check_stream_method
{
	template<class S>
	static auto pcount(S&& _stream) -> decltype(std::declval<S&>().pcount(), std::true_type{});
	static auto pcount(...) -> std::false_type;

	template<class S>
	static auto tellg(S&& _stream) -> decltype(std::declval<S&>().tellg(), std::true_type{});
	static auto tellg(...) -> std::false_type;

	template<class S>
	static auto seekg(S&& _stream) -> decltype(std::declval<S&>().seekg(0, std::ios::beg), std::true_type{});
	static auto seekg(...) -> std::false_type;

	template<class S>
	static auto read(S&& _stream) -> decltype(std::declval<S&>().read(nullptr, 0), std::true_type{});
	static auto read(...) -> std::false_type;

	template<class S>
	static auto write(S&& _stream) -> decltype(std::declval<S&>().write(nullptr, 0), std::true_type{});
	static auto write(...) -> std::false_type;
};


template<class S>
auto make_vstream(S&& _stream) -> vstream<typename std::remove_reference<S>::type::char_type>
{
	using	char_type	= typename std::remove_reference<S>::type::char_type;
	using	size_type	= typename std::remove_reference<S>::type::size_type;
	using	iostate		= typename std::remove_reference<S>::type::iostate;
	using	off_type	= typename std::remove_reference<S>::type::off_type;
	using	pos_type	= typename std::remove_reference<S>::type::pos_type;

	struct	vs : vstream<char_type>::vstream_base
	{
		vs(S&& _stream)
			: stream(std::move(_stream))
		{}

		~vs()
		{}

		virtual void on_read(char_type* _buff, size_type _n) override
		{
			stream.read(_buff, _n);
		}

		virtual void on_write(char_type const* _buff, size_type _n) override
		{
			if constexpr (std::is_same<decltype(check_stream_method::write(stream)), std::true_type>::value)
			{
				stream.write(_buff, _n);
			}
		}

		virtual size_type on_gcount() const noexcept override
		{
			return	stream.gcount();
		}

		virtual size_type on_pcount() const noexcept override
		{
			if constexpr (std::is_same<decltype(check_stream_method::pcount(stream)), std::true_type>::value)
			{
				return	stream.pcount();
			}
			else
			{
				return	0;
			}
		}

		virtual iostate on_rdstate() const noexcept override
		{
			return	stream.rdstate();
		}

		virtual void on_setstate(iostate _state) noexcept override
		{
			stream.setstate(_state);
		}

		virtual void on_clear(iostate _state) noexcept override
		{
			stream.clear(_state);
		}

		virtual pos_type on_tellg() override
		{
			using	tellg_type	= decltype(check_stream_method::tellg(stream));
			if constexpr (std::is_same<tellg_type, std::true_type>::value)
			{
				return	stream.tellg();
			}
			else
			{
				return	0;
			}
		}

		virtual void on_seekg(off_type _off, std::ios_base::seekdir _dir) override
		{
			using	tellg_type	= decltype(check_stream_method::seekg(stream));
			if constexpr (std::is_same<tellg_type, std::true_type>::value)
			{
				stream.seekg(_off, _dir);
			}
		}

		S	stream;
	};
	return	new vs(std::move(_stream));
}


AMTRS_IO_NAMESPACE_END
#endif
