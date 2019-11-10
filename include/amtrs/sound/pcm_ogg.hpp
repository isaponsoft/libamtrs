#ifndef	__libamtrs__sound__pcm_ogg__hpp
#define	__libamtrs__sound__pcm_ogg__hpp
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "../iostream/@"
#include "pcm_raw.hpp"

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")
#endif

AMTRS_NAMESPACE_BEGIN

class	pcm_ogg
		: public pcm_raw
{
public:
	template<class StreamT>
	static ref<pcm_ogg> create(StreamT&& _in);


protected:
	template<class... Args>
	pcm_ogg(int _channels, int _bits_per_sample, int64_t _samples_per_sec, Args&&... _args)
		: pcm_raw(_channels, _bits_per_sample, _samples_per_sec, std::forward<Args>(_args)...)
	{}
};



template<class StreamT>
inline ref<pcm_ogg> pcm_ogg::create(StreamT&& _in)
{
	auto			in	= ios::make_rstream(std::forward<StreamT>(_in));

	ov_callbacks	callbacks;

	callbacks.read_func		= [](void* _buffer, std::size_t _size_item, std::size_t _count, void* _data) -> std::size_t
	{
		static_cast<decltype(in)*>(_data)->clear();
		auto	ret	= static_cast<decltype(in)*>(_data)->read(static_cast<char*>(_buffer), _size_item * _count).gcount();
		return	ret;
	};
	callbacks.seek_func		= [](void* _data, ogg_int64_t _offset, int _flag) -> int
	{
#if	1
		// Android x86 の libogg がクラッシュするので seek しないようにする。
		return	-1;
#else
		static_cast<decltype(in)*>(_data)->clear();
		static_cast<decltype(in)*>(_data)->seekg
		(
			static_cast<typename decltype(in)::pos_type>(_offset),
			_flag == SEEK_END ? std::ios::end :
			_flag == SEEK_SET ? std::ios::beg :
			std::ios::cur
		);
		return	0;//static_cast<int>(_offset);
#endif
	};
	callbacks.close_func	= [](void* _data) -> int
	{
		return	0;
	};
	callbacks.tell_func		= [](void* _data) -> long
	{
		return	(long)static_cast<decltype(in)*>(_data)->tellg();
	};


	OggVorbis_File	ovf;
	if (ov_open_callbacks(&in, &ovf, 0, 0, callbacks))
	{
		throw	std::runtime_error("ogg initialize error");
	}

	vorbis_info*	info	= ov_info(&ovf, -1);
	in.clear();

	int					bitIndex = 0;
	std::vector<char>	pcm;
	for (;;)
	{
		std::size_t	s	= pcm.size();
		char		b[4096];
		std::size_t	r	= ov_read(
				&ovf,
				b,
				sizeof(b),
				0,		// 0:little endian, 1:big endian
				2,		// 2:word size
				1,		// 0:unsined, 1:signed
				&bitIndex);
		if (!r)
		{
			break;
		}
		pcm.resize(s + r);
		memcpy(pcm.data() + s, b, r);
	}
	//AMTRS_DEBUG_LOG("ogg channel=%d,rate=%d, size=%zd", (int)info->channels, (int)info->rate, pcm.size())

	ref<pcm_ogg> 	retval	= new pcm_ogg(info->channels, 16, (int64_t)info->rate, std::move(pcm));
	ov_clear(&ovf);
	return	retval;
}

AMTRS_NAMESPACE_END
#endif
