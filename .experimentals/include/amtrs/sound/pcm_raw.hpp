/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__sound__pcm_raw__hpp
#define	__libamtrs__sound__pcm_raw__hpp
#include "pcm.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! PCM RAW形式の再生を行います。
// ----------------------------------------------------------------------------
template<class BufferT>
class	basic_pcm_raw
		: public pcm
{
public:
	virtual ~basic_pcm_raw(){}

	template<class... Args>
	static ref<basic_pcm_raw> create(int _channels, int _bits_per_sample, int64_t _samples_per_sec, Args&&... _args)
	{
		return	new basic_pcm_raw(_channels, _bits_per_sample, _samples_per_sec, std::forward<Args>(_args)...);
	}

protected:
	template<class... Args>
	basic_pcm_raw(int _channels, int _bits_per_sample, int64_t _samples_per_sec, Args&&... _args)
		: pcm(_channels, _bits_per_sample, _samples_per_sec)
		, mPCMData(std::forward<Args>(_args)...)
	{}


private:
	virtual bool on_read(std::size_t* _written, void* _dest, std::size_t _requestSize) override
	{
		bool		retval		= true;
		std::size_t	copySize	= _requestSize;
		std::size_t	capacity	= mPCMData.size() - mOffset;
		if (copySize > capacity)
		{
			copySize	= capacity;
			retval		= false;
		}

		std::memcpy(static_cast<char*>(_dest), mPCMData.data() + mOffset, copySize);
		mOffset		+= copySize;
		*_written	=  copySize;
		return	retval;
	}

	BufferT			mPCMData;
	std::size_t		mOffset	= 0;
};


using	pcm_raw		= basic_pcm_raw<std::vector<char>>;



AMTRS_NAMESPACE_END
#endif
