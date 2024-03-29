﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__http__parser__hpp
#define	__libamtrs__net__http__parser__hpp
AMTRS_NET_HTTP_NAMESPACE_BEGIN

struct	directive_result
{
	std::string_view	name;
	std::string_view	value;
};

class	directive_iterator;

/*! http token.
 * ひとつのトークンを取得します。
 * _inからディレクティブを読み取り戻り値として返します。
 */
std::string_view http_get_token(std::string_view _in);


/*! http header directives.
 * _inからディレクティブとして認識可能な範囲を読み取り、戻り値として返します。
 * _name : ディレクティブ名
 * _value: 値(ダブルコーテーションが含まれる場合、ダブルコーテーションを含みます)
 * return: ディレクティブ以降の文字列
 * 		';(セミコロン)' で終了す場合、セミコロンの直前を返します。
 * 		これによりセミコロンで終わっているかどうかを呼び出し側が把握しやすくなります。
 */
std::pair<directive_result, std::string_view> http_get_directive(std::string_view _in);



/*! ヘッダ行からディレクティブを列挙するためのバケットを返します。
 *  _header_line : ヘッダの "HEADER: " を除いた部分を指定します。
 *  解析不能な文字列が検出された場合、そこで終了します。
 *  name と value はそれぞれ _header_line 文字列に対する string_view です。
 *  value.data() + value.size() で次の文字列の場所を知ることが出来ます。
 * ----------------------------------------------------------------------------
 * EXAMPLE:
 * 		auto		i	= enumrate_directive("attachment; filename=\"test.jpeg\" lastword!!");
 * 		char const*	n	= nullptr;
 * 		for (auto& d : i)
 *		{
 *			n = d.value.data() + d.value.size();
 *			std::cout << d.name << " = "[" << d.value << "]" << std::endl;
 *		}
 *		std::cout << "not syntax : [" << n << "]" << std::endl;
 * OUTPUT:
 * 		attachment = []
 * 		filename = ["test.jpeg"]
 *		lastword = []
 *		not syntax : [!!]
 * ----------------------------------------------------------------------------
 */
bucket<directive_iterator> enumrate_directive(std::string_view _header_line);


// ****************************************************************************
//! HTTPヘッダ行をキーと値に分割します。
// ----------------------------------------------------------------------------
//! 分割した値は前後の空白をトリミング済みになります。
// ----------------------------------------------------------------------------
template<class CharT, class Traits>
auto split_header(std::basic_string_view<CharT, Traits> _line) -> std::pair<std::basic_string_view<CharT, Traits>, std::basic_string_view<CharT, Traits>>
{
	auto	line	= trim(_line, "\r\n ");
	auto	pos		= line.find_first_of(':');
	if (pos == line.npos)
	{
		return	{};
	}
	return	{
		trim(line.substr(0, pos), " "),
		trim(line.substr(pos+1), " ")
	};
}


// ****************************************************************************
//! HTTPレスポンスヘッダ／リクエストヘッダの解析を行います。
// ----------------------------------------------------------------------------
template<class CharT>
class	basic_parser
{
	static constexpr size_t	default_buffer_size	= 1400;
	static constexpr size_t	min_buffer_size		= 1400;
public:
	using	char_type	= CharT;
	using	value_type	= CharT;
	using	size_type	= size_t;

	static constexpr size_type	nlen	= static_cast<size_t>(-1);


	basic_parser(bool _https = false)
		: mHttps(_https)
	{}


	//! Contents-Length が読み込まれている場合はサイズを返す。
	//! まだ読み込まれていない場合は nlen を返す。
	size_type contents_length() const noexcept { return mContentsLength; }

	transfer_encoding_type transfer_encoding() const noexcept { return mTransferEncofing; }

	bool empty() const noexcept { return mDataFinish == true; }

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
		mHeaderParsed	= true;

		// HEADER
		while (_in.good() && !mHttpsContent)
		{
			// バッファの最後まで使い切った
			if ((mBuffer.size() - mTail) < min_buffer_size)
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
			mTail += _in.gcount();
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
							auto	num	= (c >= '0' && c <= '9') ? (c - '0')
										: 0;
							mContentsLength = mContentsLength * 10 + num;
						}
					}

					if (starts_with(line, (std::string_view)"Transfer-Encoding:"))
					{
						auto	pos	= line.find_first_of(':');
						auto	val	= trim(line.substr(pos+1), " \t\r\n");
						mTransferEncofing	= val == "chunked" ? transfer_encoding_type::chunked
											: val == "compress" ? transfer_encoding_type::compress
											: val == "deflate" ? transfer_encoding_type::deflate
											: val == "gzip" ? transfer_encoding_type::gzip
											: transfer_encoding_type::identity;
					}
				}
				else
				{
					break;
				}
			}
		}

		shift();
		return	headerCount;
	}

	// ========================================================================
	//! チャンクサイズをパースする
	// ------------------------------------------------------------------------
	//! 戻り値は読みとったデータのサイズ。
	//! nlen を返した場合はチャンクサイズを解析するのに十分なデータサイズが
	//! ないことを示す。
	// ------------------------------------------------------------------------
	size_t read_chunk_size(size_t& _chunksize, char_type* _buffer, size_type _size)
	{
		auto		tl	= std::string_view(_buffer, _size);
		auto		lp	= tl.find_first_of("\r\n");
		if (lp == std::string_view::npos || (lp + 1 > tl.size()))
		{
			return	nlen;
		}

		// 改行コードが見つかったのでコールバックに通知
		std::string_view	line(std::string_view(_buffer, lp));
		size_t				chsz(0);
		for (auto c : trim(line, "\r\n"))
		{
			chsz = chsz * 16;
			if (c >= '0' && c <= '9')
			{
				chsz += c - '0';
			}
			else if (c >= 'a' && c <= 'f')
			{
				chsz += c - 'a' + 10;
			}
			else if (c >= 'A' && c <= 'F')
			{
				chsz += c - 'A' + 10;
			}
			else
			{
				AMTRS_WARN_LOG("Unkown chunked charactor 0x%02x", (unsigned int)c);
				throw	std::runtime_error("Unkown chunked charactor");
			}
		}
		_chunksize	= chsz;
		return	lp + 2;
	}


	// ========================================================================
	//! コンテンツ部分を読み取ります。
	// ------------------------------------------------------------------------
	//! 事前に header を解析していない場合は自動的にヘッダを読み取り、
	//! ヘッダ部分をスキップします。
	// ------------------------------------------------------------------------
	template<class In>
	size_t read(char_type* _buffer, In& _in, size_type _size)
	{
		size_t		retval	= 0;
		char_type*	out		= _buffer;
		size_t		reqSize	= _size;

		if (!mHeaderParsed)
		{
			header<In>(_in, [](char_type const* _line, size_t _size) -> bool
			{
				return	true;
			});
		}

		if (mContentsLength != nlen)
		{
			if (mContentsLength == mContentsTransfer)
			{
				mDataFinish	= true;
				return	0;
			}
			reqSize	= std::min<size_type>(mContentsLength - mContentsTransfer, reqSize);
		}

		if (transfer_encoding() == transfer_encoding_type::chunked)
		{
			if (mChunkSize == mChunkRead)
			{
				for (;;)
				{
					size_t	chunkSize	= 0;
					size_t	readSize	= read_chunk_size(chunkSize, mBuffer.data() + mTop, mTail - mTop);
					if (readSize == nlen)
					{
						// チャンクを読み取るだけのバッファが足りない
						if (!shift())
						{
							// バッファを拡張する
							mBuffer.resize(mBuffer.size() + default_buffer_size);
						}
						_in.read(mBuffer.data() + mTail, mBuffer.size() - mTail);
						mTail += _in.gcount();
					}
					else
					{
						mChunkSize		=  chunkSize; // \r + \n
						mChunkRead		=  0;
						mTop			+= readSize;
						if (mChunkSize == 0)
						{
							mDataFinish	= true;
						}
						break;
					}
				}
			}
			reqSize	= std::min(reqSize, (mChunkSize - mChunkRead));
		}


		// まずはバッファに残っているデータを出力
		auto	copySize	= std::min(mTail - mTop, reqSize);	// コピーすべき量
		if (copySize > 0)
		{
			reqSize				-= copySize;
			retval				+= copySize;
			mChunkRead			+= copySize;
			std::copy_n(mBuffer.data() + mTop, copySize, out);
			out					+= copySize;
			mTop				+= copySize;
			mContentsTransfer	+= copySize;
			if (mTop == mTail)
			{
				mTop	= 0;
				mTail	= 0;
			}
		}

		// まだコピーすべきデータが残っている場合はストリームから読み取りながら転送する
		while ((reqSize > 0) && _in.good())
		{
			_in.read(out, reqSize);
			auto	rs	= _in.gcount();
			reqSize				-= rs;
			retval				+= rs;
			out					+= rs;
			mChunkRead			+= rs;
			mContentsTransfer	+= rs;
		}

		if (transfer_encoding() == transfer_encoding_type::chunked)
		{
			if (mChunkSize == mChunkRead)
			{
				// Line end skip.
				char	le[2];
				int		r	= 0;

				while (mTop < mTail && r < 2)
				{
					le[r++] = mBuffer.data()[mTop++];
				}
				if (mTop == mTail)
				{
					mTop	= 0;
					mTail	= 0;
				}
				if (r < 2)
				{
					size_t	rs;
					do
					{
						_in.read(le + r, 2 - r);
						rs = _in.gcount();
						r += (int)rs;
					} while (r != 2 && rs > 0);
				}
				if (mChunkSize == 0)
				{
					mDataFinish	= true;
				}
			}
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


	std::vector<char>		mBuffer;
	size_t					mTop				= 0;
	size_t					mTail				= 0;
	size_t					mContentsLength		= nlen;
	size_t					mContentsTransfer	= 0;
	size_t					mChunkSize			= 0;
	size_t					mChunkRead			= 0;
	transfer_encoding_type	mTransferEncofing	= transfer_encoding_type::identity;

	bool					mHeaderParsed		= false;
	bool					mDataFinish			= false;

	bool					mHttps				= true;
	bool					mHttpsContent		= false;
};

using	parser	= basic_parser<char>;




class	directive_iterator
{
public:
	directive_iterator() = default;
	directive_iterator(directive_iterator const&) = default;
	directive_iterator& operator = (directive_iterator const&) = default;
	directive_iterator(directive_result _d, std::string_view _r)
		: mDirective(std::move(_d))
		, mRemain(std::move(_r))
	{}

	directive_result const* operator -> () const noexcept
	{
		return	&mDirective;
	}

	directive_result const& operator * () const noexcept
	{
		return	mDirective;
	}

	bool operator == (directive_iterator const& _r) const noexcept
	{
		return	mRemain == _r.mRemain && mDirective.name == _r.mDirective.name;
	}

	bool operator != (directive_iterator const& _r) const noexcept
	{
		return	mRemain != _r.mRemain || mDirective.name != _r.mDirective.name;
	}

	directive_iterator& operator ++ ();

protected:
	directive_result	mDirective;
	std::string_view	mRemain;
};


AMTRS_NET_HTTP_NAMESPACE_END
#endif
