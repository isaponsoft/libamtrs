/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem-lar.hpp>
#include <amtrs/io-zlib.hpp>
using namespace amtrs;

size_t read_lar_size_t(LAR_SIZE_T& _n, amtrs_stream_read_seekable& _i)
{
	size_t		len	= 0;
	LAR_SIZE_T	n	= 0;
	for (;;)
	{
		uint8_t	v;
		io::read(&v, _i, sizeof(uint8_t));
		++len;
		n	= (n << 7) + (v & 0x7f);
		if (v & 0x80)
		{
			break;
		}

		if (len >= sizeof(LAR_SIZE_T))
		{
			return	-1;
		}
	}
	_n	= n;
	return	len;
}


size_t write_lar_size_t(amtrs_stream_write_seekable* _o, LAR_SIZE_T _n)
{
	uint8_t	buff[sizeof(LAR_SIZE_T)];
	size_t	len	= 0;
	do
	{
		auto	n	= _n >> 7;
		buff[sizeof(buff) - (++len)]	= (_n & 0x7f);
		_n	= n;
	} while (_n > 0);

	if (_o)
	{
		buff[sizeof(buff) - 1] |= 0x80;
		_o->write(&buff[sizeof(buff) - (len)], len);
	}
	return	len;
}

AMTRS_LAR_NAMESPACE_BEGIN




lar::lar()
{
}

lar::~lar()
{
	if (mEntries)
	{
		for (auto i = 0; i < size(); ++i)
		{
			if (mEntries[i])
			{
				delete	mEntries[i];
			}
		}
		delete[]	mEntries;
	}
}

size_t lar::compute_entry_size() const noexcept
{
	size_t	s	= 0;
	for (auto& e : *this)
	{
		size_t	es	= 0;
		es	+= write_lar_size_t(nullptr, e.compress_size);
		es	+= write_lar_size_t(nullptr, e.original_size);
		es	+= write_lar_size_t(nullptr, e.name_size);
		es	+= sizeof(e.compress);
		es	+= e.name_size;
		s   += es;
	}
	return	s;
}


size_t lar::compute_value_size() const noexcept
{
	size_t	s	= 0;
	for (auto& e : *this)
	{
		s	+= e.compress_size;
	}
	return	s;
}

lar::iterator lar::add_entry(std::string_view _name)
{
	if (auto e = find(_name); e != end())
	{
		return	e;
	}

	// list extends.
	entry**	newlist		= new entry*[mEntryNum + 1];
	if (mEntries)
	{
		std::copy_n(mEntries, mEntryNum, newlist);
		delete[]	mEntries;
	}
	mEntries	= newlist;

	entry*	e			= new entry();
	mEntries[mEntryNum]	= e;
	mEntryNum			= mEntryNum + 1;

	e->name(_name);

	e->compress				= compresstype::lar_raw;
	e->original_size		= 0;
	e->compress_size		= 0;

	return	iterator(newlist + mEntryNum - 1);
}


lar::iterator lar::find(std::string_view _name) noexcept
{
	for (auto it = begin(); it != end(); ++it)
	{
		if ( _name == it->name())
		{
			return	it;
		}
	}
	return	end();
}


lar::const_iterator lar::find(std::string_view _name) const noexcept
{
	for (auto it = begin(); it != end(); ++it)
	{
		if ( _name == it->name())
		{
			return	it;
		}
	}
	return	end();
}


void lar::entry::name(std::string_view _name)
{
	if (mName) { delete[] mName; mName = nullptr; }
	mName = new char [_name.size() + 1];
	std::copy_n(_name.data(), _name.size(), mName);
	mName[_name.size()]	= 0;
	name_size			= _name.size();
}


void lar::entry::data_alloc()
{
	if (mData)
	{
		delete[]	mData;
		mData	= nullptr;
	}
	mData			= new char [compress_size];
}

struct	alloc
{
	char* allocate(size_t n)
	{
		return	new char[n];
	}

	void deallocate(void* m, size_t n)
	{
		delete[] reinterpret_cast<char*>(m);
	}
};

void lar::entry::compress_to(compresstype _type)
{
	if (compress == _type)
	{
		return;
	}
	switch (_type)
	{
		case lar_raw :
		{
			if (compress == lar_deflate)
			{
				char*	originalbuff	= new char[original_size];
				z_stream	z;
				std::memset(&z, 0, sizeof(z));
				::inflateInit2(&z, -MAX_WBITS);
				z.next_in	= (Bytef*)mData;
				z.avail_in 	= compress_size;
				z.next_out	= (Bytef*)originalbuff;
				z.avail_out	= original_size;
				int	r	= inflate(&z, Z_FINISH);
				if (r != Z_STREAM_END || z.avail_out > 0)
				{
					delete[]	originalbuff;
					return;
				}
				delete[]	mData;
				compress		= lar_raw;
				compress_size	= original_size;
				mData			= originalbuff;
				inflateEnd(&z);
			}
			break;
		}

		case lar_deflate :
		{
			size_t	compresssize	= original_size;
			char*	compressbuff	= new char[compresssize];
			z_stream	z;
			std::memset(&z, 0, sizeof(z));
			::deflateInit2(&z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 9, Z_DEFAULT_STRATEGY);
			z.next_in	= (Bytef*)mData;
			z.avail_in 	= original_size;
			z.next_out	= (Bytef*)compressbuff;
			z.avail_out	= compresssize;
			int	r	= deflate(&z, Z_FINISH);
			if (r == Z_STREAM_END && z.avail_out > 0)
			{
				delete[]	mData;
				compress		= _type;
				compress_size	= compresssize - z.avail_out;
				mData			= compressbuff;
			}
			else
			{
				// 圧縮出来なかったのでRAWのまま
				delete[]	compressbuff;
			}
			deflateEnd(&z);
			break;
		}
	}
}


AMTRS_LAR_NAMESPACE_END

AMTRS_IO_NAMESPACE_BEGIN

bool read(AMTRS_FILESYSTEM_NAMESPACE::lar& _ar, amtrs_stream_read_seekable& _i, bool _headeronly)
{
	using namespace	AMTRS_FILESYSTEM_NAMESPACE;
	size_t			filesize	= io::size(_i);
	LAR_FILEHEADER	h;
	if (_i.read(&h, sizeof(h)) != sizeof(h))
	{
		return	false;
	}
	if (h.signature[0] != 'L' || h.signature[1]	!= 'A' || h.signature[2] != 'R' || h.signature[3] != '1')
	{
		return	false;
	}
	if (filesize && filesize < h.header_size)
	{
		return	false;
	}

	LAR_SIZE_T	entry_size;
	LAR_SIZE_T	value_size;
	LAR_SIZE_T	file_num;
	read_lar_size_t(entry_size, _i);
	read_lar_size_t(value_size, _i);
	read_lar_size_t(file_num, _i);

#if	0
	std::cout << "entry size : " << std::dec << entry_size << std::endl;
	std::cout << "value size : " << std::dec << value_size << std::endl;
	std::cout << "file count : " << std::dec << file_num << std::endl;
#endif
	
	std::vector<lar::entry*>	entries;
	entries.resize(file_num);

	for (uint32_t fnum = 0; fnum < file_num; ++fnum)
	{
		LAR_ENTRY	e;

		read_lar_size_t(e.compress_size, _i);
		read_lar_size_t(e.original_size, _i);
		read_lar_size_t(e.name_size, _i);
		io::read(&e.compress, _i, sizeof(uint8_t));

		char*		buff	= new char[e.name_size + 1];
		_i.read(buff, e.name_size);
		buff[e.name_size]	= 0;
		entries[fnum]	= &*_ar.add_entry({buff, e.name_size});
		entries[fnum]->compress_size	= e.compress_size;
		entries[fnum]->original_size	= e.original_size;
		entries[fnum]->compress			= e.compress;
		delete[]	buff;
	}

	if (_headeronly)
	{
		return	true;
	}

	for (uint32_t fnum = 0; fnum < file_num; ++fnum)
	{
		auto*	e		= entries[fnum];
		e->data_alloc();
		auto	rs		= _i.read(e->data(), e->compress_size);
	}

	return	true;
}


bool write(amtrs_stream_write_seekable& _o, AMTRS_FILESYSTEM_NAMESPACE::lar& _s)
{
	char	z[16]	= {0};
	auto	cp		= _o.tell();
	auto	es		= _s.compute_entry_size();
	auto	vs		= _s.compute_value_size();
	auto	hs		= sizeof(LAR_FILEHEADER);
	hs	+= write_lar_size_t(nullptr, es);
	hs	+= write_lar_size_t(nullptr, vs);
	hs	+= write_lar_size_t(nullptr, _s.size());
	auto	ts		= hs + es + vs;

	LAR_FILEHEADER	h;
	h.signature[0]	= 'L';
	h.signature[1]	= 'A';
	h.signature[2]	= 'R';
	h.signature[3]	= '1';
	h.header_size	= hs;
	_o.write(&h, sizeof(h));

#if	0
	std::cout << "total size : " << std::dec << ts << std::endl;
	std::cout << "header size: " << std::dec << hs << std::endl;
	std::cout << "entry size : " << std::dec << es << std::endl;
	std::cout << "value size : " << std::dec << vs << std::endl;
	std::cout << "file count : " << std::dec << _s.size() << std::endl;
#endif

	// entry size
	write_lar_size_t(&_o, es);

	// value_size
	write_lar_size_t(&_o, vs);

	// file count
	write_lar_size_t(&_o, _s.size());

	// entry table
	uint32_t	address	= sizeof(LAR_FILEHEADER) + es + _s.compute_padd(es, 16);
	for (auto& e : _s)
	{
#if	0
		std::cout << e.name() << " " << e.original_size << " => " << e.compress_size << "(" << (100.0 * double(e.compress_size) / double(e.original_size)) << "%)" << std::endl;
#endif
		write_lar_size_t(&_o, e.compress_size);
		write_lar_size_t(&_o, e.original_size);
		write_lar_size_t(&_o, e.name_size);
		io::write(_o, &e.compress, sizeof(uint8_t));
		_o.write(e.name(), e.name_size);
	}

	// file data
	for (auto& e : _s)
	{
		_o.write(e.data(), e.compress_size);
	}

	return	true;
}



AMTRS_IO_NAMESPACE_END
