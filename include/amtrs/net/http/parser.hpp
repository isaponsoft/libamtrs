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
DISCLAIMED. In NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER In CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING In ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__net__http__parser__hpp
#define	__libamtrs__net__http__parser__hpp
#include "../../string/trim.hpp"
#include "def.hpp"
AMTRS_NET_HTTP_NAMESPACE_BEGIN



// ****************************************************************************
//! HTTPレスポンスヘッダ／リクエストヘッダの解析を行います。
// ----------------------------------------------------------------------------
template<class CharT>
class	basic_parser
{
	static constexpr size_t	default_buffer_size	= 200;
public:
	using	char_type	= CharT;
	using	value_type	= CharT;
	using	size_type	= size_t;

	static constexpr size_type	nlen	= static_cast<size_t>(-1);

	basic_parser(bool _https)
		: mHttps(_https)
	{}


	//! Contents-Length が読み込まれている場合はサイズを返す。
	//! まだ読み込まれていない場合は nlen を返す。
	size_type contents_length() const noexcept { return mContentsLength; }

	//! read で読み取り可能なコンテンツの残り残量を返します。
	size_type contents_rest() const noexcept { return mContentsRest; }


	// ****************************************************************************
	//! ヘッダの解析を行い、１行ずつコールバックを呼び出します。
	// ----------------------------------------------------------------------------
	//! _callback
	//!		bool(char const* _header, size_t _size);
	//!		_header			: ヘッダを示すデータの先頭。
	//!		_size			: _headerに格納されているデータのサイズ
	//!
	//! コールバックから抜けると _header の示すデータは無効となります。
	//!	コールバックが false を返すと処理を中断します。
	// ----------------------------------------------------------------------------
	template<class In, class Callback>
	size_t header(In& _in, Callback&& _callback)
	{
		size_t	headerCount	= 0;

		// HEADER
		while (_in.good() && !mHttpsContent)
		{
			// バッファの最後まで使い切った
			if (mTail == mBuffer.size())
			{
				// 前に詰める
				if (!shift())
				{
					// バッファを拡張する
					mBuffer.resize(mBuffer.size() + default_buffer_size);
				}
			}


			// バッファの最後尾に読み込む
			_in.read(mBuffer.data() + mTail, mBuffer.size() - mTail);
			mTail	+= _in.gcount();
			while ((mTail - mTop) > 0)
			{
				// 新しく読み取った部分に改行コードが含まれるかチェック
				auto		lp	= std::string_view(mBuffer.data() + mTop, mTail - mTop).find_first_of("\n");
				if (lp != std::string_view::npos)
				{
					// 改行コードが見つかったのでコールバックに通知
					std::string_view	line(mBuffer.data() + mTop, lp + 1);
					mTop		+= line.size();
					if (line == "\n" || line == "\r\n")
					{
						if (!mHttps)
						{
							return	headerCount;
						}
						mHttpsContent	= true;
						break;
					}
					if (!_callback(line.data(), line.size()))
					{
						return	headerCount;
					}
					++headerCount;

					if (starts_with(line, (std::string_view)"Content-Length:"))
					{
						auto	pos	= line.find_first_of(':');
						auto	val	= trim(line.substr(pos+1), " \t\r\n");
						mContentsLength	= 0;
						for (auto c : val)
						{
							mContentsLength = mContentsLength * 10 + (c - '0');
						}
						mContentsRest	= mContentsLength;
					}
				}
				else
				{
					break;
				}
			}
		}

		shift();
		if (mHttpsContent)
		{
			for (;;)
			{
				auto		lp	= std::string_view(mBuffer.data() + mTop, mTail - mTop).find_first_of("\n");
				if (lp != std::string_view::npos)
				{
					// 改行コードが見つかったのでコールバックに通知
					std::string_view	line(std::string_view(mBuffer.data() + mTop, lp + 1));
					mTop			+= line.size();
					mContentsLength	=  0;
					for (auto c : trim(line, "\r\n"))
					{
						mContentsLength = mContentsLength * 16;
						if (c >= '0' && c <= '9')
						{
							mContentsLength += c - '0';
						}
						else if (c >= 'a' && c <= 'f')
						{
							mContentsLength += c - 'a' + 10;
						}
						else if (c >= 'A' && c <= 'F')
						{
							mContentsLength += c - 'A' + 10;
						}
					}
					mContentsRest	= mContentsLength;
					break;
				}
				else
				{
					_in.read(mBuffer.data() + mTail, mBuffer.size() - mTail);
					mTail	+= _in.gcount();
				}
			}
			shift();
		}

		return	headerCount;
	}



	// ========================================================================
	//! コンテンツ部分を読み取ります。
	// ------------------------------------------------------------------------
	//! 事前に header を解析していない場合は自動的にヘッダを読み取り、
	//! ヘッダ部分をスキップします。
	// ------------------------------------------------------------------------
	template<class In>
	size_t read(char_type* _buffer, In& _in, size_type _size = nlen)
	{
		if (mContentsLength	== nlen)
		{
			header<In>(_in, [](char_type const* _line, size_t _size) -> bool
			{
				return	true;
			});
		}

		size_t		retval	= 0;
		char_type*	out		= _buffer;
		size_t		reqSize	= (_size == nlen) ? mContentsRest : std::min(_size, _size);

		// まずはバッファに残っているデータを出力
		auto	maxSize		= std::min(mContentsRest, mTail - mTop);	// 現在返すことができるデータの最大量
		auto	copySize	= std::min(maxSize, reqSize);				// コピーすべき量
		if (copySize > 0)
		{
			mContentsRest		-= copySize;
			reqSize				-= copySize;
			maxSize				-= copySize;
			retval				+= copySize;
			std::copy_n(mBuffer.data() + mTop, copySize, out);
			out					+= copySize;
			mTop				+= copySize;
			if (mTop == mTail)
			{
				mTop	= 0;
				mTail	= 0;
			}
		}

		// まだコピーすべきデータが残っている場合はストリームから読み取りながら転送する
		while ((mContentsRest > 0) && (reqSize > 0) && _in.good())
		{
			auto	copySize	= std::min(mContentsRest, reqSize);				// コピーすべき量
			_in.read(out, copySize);
			copySize	= _in.gcount();
			mContentsRest		-= copySize;
			reqSize				-= copySize;
			maxSize				-= copySize;
			retval				+= copySize;
			out					+= copySize;
		}

		return	retval;
	}


	

private:

	bool shift()
	{
		if (mTop <= 0)
		{
			return	false;
		}

		// 前に詰める
		auto*	d	= mBuffer.data();
		auto*	s	= mBuffer.data() + mTop;
		auto	c	= mTail - mTop;
		for (size_t i = 0; i < c; ++i)
		{
			d[i] = s[i];
		}
		mTail	-= mTop;
		mTop	=  0;
		return	true;
	}


	std::vector<char>	mBuffer;
	size_t				mTop			= 0;
	size_t				mTail			= 0;
	size_t				mContentsLength	= nlen;
	size_t				mContentsRest	= 0;
	bool				mHttps			= true;
	bool				mHttpsContent	= false;
};

using	parser	= basic_parser<char>;


AMTRS_NET_HTTP_NAMESPACE_END
#endif
