/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__vstreamif__hpp
#define	__libamtrs__io__vstreamif__hpp
AMTRS_IO_NAMESPACE_BEGIN




class	vstreamif
		: public streamif_base
{
public:
	class	vstreambase;
	using	size_type	= std::streamsize;
	using	iostate		= streamif_base::iostate;
	using	fpos_type	= std::fpos<typename std::char_traits<char>::state_type>;
	using	pos_type	= fpos_type;
	using	char_type	= char;

	vstreamif() = default;
	vstreamif(vstreamif const&) = default;
	vstreamif(vstreamif&&) = default;
	vstreamif& operator = (vstreamif const&) = default;
	vstreamif& operator = (vstreamif&&) = default;

	vstreamif(ref<vstreambase> _i)
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

	vstreamif& read(void* _data, size_type _size) { mInstance->read(_data, _size); return *this; }
	vstreamif& write(void const* _data, size_type _size) { mInstance->write(_data, _size); return *this; }
	void seek(fpos_type _pos, streamif_base::seekdir _dir) { return mInstance->seek(_pos, _dir); }
	vstreamif& seek(fpos_type _pos) { seek(_pos, beg); return *this; }
	fpos_type tell() { return mInstance->tell(); }
	size_type size() { return mInstance->size(); }

	size_type gcount() const noexcept { return mInstance->gcount(); }
	size_type pcount() const noexcept { return mInstance->pcount(); }

	class	vstreambase : public ref_object
	{
	public:
		virtual void read(void* _data, size_type _size) = 0;
		virtual void write(void const* _data, size_type _size) = 0;
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




template<class Stream>
auto make_vstreamif(Stream&& _s) -> vstreamif
{
	auto	stream	= make_streamif(std::move(_s));

	using	stream_type		= decltype(stream);
	using	vstream_type	= vstreamif;

	class	pipe
			: public vstreamif::vstreambase
	{
	public:
		using	size_type	= typename stream_type::size_type;
		using	iostate		= typename stream_type::iostate;
		using	fpos_type	= typename stream_type::fpos_type;

		pipe(stream_type&& _o)
			: org(std::move(_o))
		{}

		virtual void read(void* _data, size_type _size) override
		{
			org.read(_data, _size);
		}

		virtual void write(void const* _data, size_type _size) override
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
	return	vstream_type(new pipe(std::move(stream)));
}

inline auto make_vstreamif(vstreamif _value) -> vstreamif
{
	return	_value;
}

inline auto make_streamif(vstreamif _value) -> vstreamif
{
	return	_value;
}


AMTRS_IO_NAMESPACE_END
#endif
