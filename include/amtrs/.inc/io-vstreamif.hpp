/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__vstreamif__hpp
#define	__libamtrs__io__vstreamif__hpp
AMTRS_IO_NAMESPACE_BEGIN


template<class Elm, class Traits = std::char_traits<Elm>>
class	basic_vstreamif;

using	vstreamif	= basic_vstreamif<char>;
using	wvstreamif	= basic_vstreamif<wchar_t>;


template<class Elm, class Traits>
class	basic_vstreamif
		: public streamif_base
{
public:
	class	vstreambase;
	using	char_type	= Elm;
	using	size_type	= std::streamsize;
	using	iostate		= streamif_base::iostate;
	using	fpos_type	= std::fpos<typename Traits::state_type>;

	basic_vstreamif() = default;
	basic_vstreamif(basic_vstreamif const&) = default;
	basic_vstreamif(basic_vstreamif&&) = default;
	basic_vstreamif& operator = (basic_vstreamif const&) = default;
	basic_vstreamif& operator = (basic_vstreamif&&) = default;

	basic_vstreamif(ref<vstreambase> _i)
		: mInstance(std::move(_i))
	{}

	bool empty() const noexcept { return mInstance.empty(); }

	explicit operator bool() const { return !empty() && good(); }

	bool good() const noexcept { return rdstate() == streamif_base::goodbit; }
	bool eof()  const noexcept { return rdstate() & streamif_base::eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & streamif_base::failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & streamif_base::badbit  ? true : false; }
	iostate rdstate() const noexcept { return mInstance->rdstate(); }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept { mInstance->clear(_state); }

	basic_vstreamif& read(char_type* _data, size_type _size) { mInstance->read(_data, _size); return *this; }
	basic_vstreamif& write(char_type const* _data, size_type _size) { mInstance->write(_data, _size); return *this; }
	void seek(fpos_type _pos, streamif_base::seekdir _dir) { return mInstance->seek(_pos, _dir); }
	basic_vstreamif& seek(fpos_type _pos) { seek(_pos, beg); return *this; }
	fpos_type tell() { return mInstance->tell(); }
	size_type size() { return mInstance->size(); }

	size_type gcount() const noexcept { return mInstance->gcount(); }
	size_type pcount() const noexcept { return mInstance->pcount(); }

	class	vstreambase : public ref_object
	{
	public:
		virtual void read(char_type* _data, size_type _size) = 0;
		virtual void write(char_type const* _data, size_type _size) = 0;
		virtual void seek(fpos_type _pos, streamif_base::seekdir _dir) = 0;
		virtual fpos_type tell() = 0;
		virtual size_type size() = 0;
		virtual iostate rdstate() const noexcept = 0;
		virtual void clear(iostate _state) noexcept = 0;
		virtual size_type gcount() const noexcept = 0;
		virtual size_type pcount() const noexcept = 0;
	};
protected:
	ref<vstreambase>	mInstance;
};




template<class Source, class Elm, class Traits>
auto make_vstreamif(basic_streamif<Source, Elm, Traits> _s) -> basic_vstreamif<Elm>
{
	using	stream_type		= basic_streamif<Source, Elm, Traits>;
	using	vstream_type	= basic_vstreamif<Elm>;

	class	pipe
			: public basic_vstreamif<Elm>::vstreambase
	{
	public:
		using	char_type	= typename stream_type::char_type;
		using	size_type	= typename stream_type::size_type;
		using	iostate		= typename stream_type::iostate;
		using	fpos_type	= typename stream_type::fpos_type;

		pipe(stream_type _o)
			: org(std::move(_o))
		{}

		virtual void read(char_type* _data, size_type _size) override
		{
			org.read(_data, _size);
		}

		virtual void write(char_type const* _data, size_type _size) override
		{
			org.write(_data, _size);
		}

		virtual void seek(fpos_type _pos, streamif_base::seekdir _dir) override
		{
			org.seek(_pos, _dir);
		}

		virtual fpos_type tell() override
		{
			return	org.tell();
		}

		virtual size_type size() override
		{
			return	org.size();
		}

		virtual iostate rdstate() const noexcept override
		{
			return	org.rdstate();
		}

		virtual void clear(iostate _state) noexcept override
		{
			org.clear(_state);
		}

		virtual size_type gcount() const noexcept override
		{
			return	org.gcount();
		}

		virtual size_type pcount() const noexcept override
		{
			return	org.pcount();
		}

		stream_type	org;
	};
	return	vstream_type(new pipe(std::move(_s)));
}

template<class... Source>
auto make_vstreamif(basic_vstreamif<Source...> _value) -> basic_vstreamif<Source...>
{
	return	_value;
}

template<class... Source>
auto make_streamif(basic_vstreamif<Source...> _value) -> basic_vstreamif<Source...>
{
	return	_value;
}


AMTRS_IO_NAMESPACE_END
#endif
