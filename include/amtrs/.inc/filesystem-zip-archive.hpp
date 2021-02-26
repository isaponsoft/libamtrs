/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__zlp__zip_archive__hpp
#define __libamtrs__filesystem__zlp__zip_archive__hpp
AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN



template<class In>
uintmax_t search_end_of_central(end_of_centraldirentry* _buff, In& _in, uintmax_t _fsize)
{
	static constexpr size_t	buffer_size	= 0x400;

	size_type	backRead	= sizeof(eocd);
	size_type	maxBack		= _fsize;
	uintmax_t	centralPos	= static_cast<uintmax_t>(-1);
	char		buff[buffer_size + 4];
	while (backRead < maxBack)
	{
		size_type	readSize;
		size_type	readPos;

		if (backRead + buffer_size > maxBack)
		{
			backRead	=  maxBack;
		}
		else
		{
			backRead	+= buffer_size;
		}

		readPos		= _fsize-backRead;
		readSize	= std::min((buffer_size+4), (_fsize-readPos));
		if (!io::seek(_in, readPos, std::ios::beg))
		{
			break;
		}
		if (io::read(buff, _in, readSize) != readSize)
		{
			break;
		}

		// EOCDが含まれているか調べる
		for (intmax_t i = (intmax_t)readSize - 3; (--i) > 0; )
		{
			if (!std::memcmp(buff + i, eocd, sizeof(eocd)))
			{
				centralPos	= readPos + i;

				// セントラルディレクトリを読み取る
				if (_buff)
				{
					if (!io::read(*_buff, view<char>(buff + i, sizeof(buff) - i)))
					{
						return	static_cast<uintmax_t>(-1);
					}
				}
				return	centralPos;
			}
		}
	}
	return	static_cast<uintmax_t>(-1);
}



//! テンプレート引数INで指定するioを使用して zip アーカイブのファイルイテレータを作成します。
template<class In>
class	zip_archive
{
public:
	using	filesize_type	= uintmax_t;
	using	size_type		= size_t;
	using	value_type		= zip_entry;
	using	filename_type	= typename value_type::filename_type;

	using	iterator		= zip_iterator<In>;
	static constexpr filesize_type	npos	= static_cast<filesize_type>(-1);


	template<class... Args>
	zip_archive(Args&&... _args)
		: mInput(std::forward<Args>(_args)...)
	{
		end_of_centraldirentry	eoc;
		mArchiveSize		= io::size(mInput);
		mEOCPosition		= search_end_of_central(&eoc, mInput, mArchiveSize);
		mDataPosition		= mEOCPosition - eoc.sizeOfCentralDir - eoc.offsetOfCentralDir;
		mFirstCentralPos	= eoc.offsetOfCentralDir;
	}

	zip_archive(const zip_archive&) = default;
	zip_archive(zip_archive&&) = default;
	zip_archive& operator = (const zip_archive&) = default;
	zip_archive& operator = (zip_archive&&) = default;

	iterator begin() noexcept
	{
		return	iterator(this, mDataPosition + mFirstCentralPos);
	}

	iterator end() noexcept
	{
		return	iterator();
	}


	// ========================================================================
	//! エントリーで指定するファイルをオープンする。
	// ------------------------------------------------------------------------
	//! 戻り値はコンストラクタで設定したINオブジェクトへの参照です。open中に
	//! zip_iterator などを操作すると正しく読み込めなくなる可能性があります。
	// ------------------------------------------------------------------------
	auto open(const value_type& _entry)
	{
		mInput.clear();
		filesystem::zip::localfilehead	lfh;
		io::seek(mInput, mDataPosition + _entry.mFilepos, std::ios::beg);
		if (!io::read(lfh, mInput))
		{
			return	io::make_limit(mInput, 0);
		}
		io::seek(mInput, lfh.fileInfo.filenameSize + lfh.fileInfo.extraFieldSize, std::ios::cur);
		return	io::make_limit(mInput, _entry.compressedSize);
	}

	auto& rdata() noexcept
	{
		return	mInput;
	}

private:
	bool read_entry(zip_entry* _entry, filesystem::zip::centraldirentry& _central, filesize_type _cdp)
	{

		io::seek(mInput, mDataPosition + _cdp, std::ios::beg);
		if (!io::read(_central, mInput))
		{
			return	false;
		}
		if (_entry)
		{
			*static_cast<entry_data*>(_entry)		= _central.fileInfo;

			_entry->mFilename	= filename_type(_central.fileInfo.filenameSize);
			io::read(_entry->mFilename.data(), mInput, _central.fileInfo.filenameSize);
#if	0
			shared_buffer<char>	extra		= shared_buffer<char>(_central.fileInfo.extraFieldSize);
			io::read(extra.data(), mInput, extra.size());

			shared_buffer<char>	comment		= shared_buffer<char>(_central.fileInfo.commentSize);
			io::read(comment.data(), mInput, comment.size());
#endif
		}
		return	true;
	}



	In				mInput;
	filesize_type	mArchiveSize		= 0;
	filesize_type	mEOCPosition		= 0;
	filesize_type	mDataPosition		= 0;
	filesize_type	mFirstCentralPos	= 0;

	template<class>
	friend	class	zip_iterator;
};


template<class In>
auto open(In& _in)
{
	return	zip_archive<In&>(_in);
}

template<class In>
auto open(In&& _in)
{
	return	zip_archive<In>(std::move(_in));
}


AMTRS_FILESYSTEM_ZIP_NAMESPACE_END
#endif
