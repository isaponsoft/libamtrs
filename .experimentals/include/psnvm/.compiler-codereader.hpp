/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__compiler__codereader__hpp
#define	__psnvm__compiler__codereader__hpp
#include ".vm-vm.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! コードを読み込むための構造体
// ----------------------------------------------------------------------------
struct	codereader
{
	struct	sourcecode
	{
		vmstring		filename;
		vmstring		code;
		int				line		= 1;
	};

	struct	positiondata
	{
		char const*	cursor	= nullptr;
		int			line	= 1;

		char const* data() const noexcept { return cursor; }
	};


	enum read_types
	{
		unkown_type		= 0,
		identify_type	= 1<<0,
		macroname_type	= 1<<1,
		string_type		= 1<<2,
		integer_type	= 1<<3,
		numeric_type	= 1<<4,
	};



	codereader() = default;
	codereader(codereader const&) = default;
	codereader(codereader&) = default;
	codereader(vmstring _filename, vmstring _code, int _line = 1)
		: mFilename(_filename)
		, mSsource(_code)
		, mCursor(mSsource.data())
		, mEnd(&*mSsource.end())
		, mLine(_line)
	{}

	auto filename() const noexcept { return mFilename; }
	auto line() const noexcept { return mLine; }

	auto c_str() const noexcept { return mCursor; } 
	auto data() const noexcept { return mCursor; } 
	auto end() const noexcept { return mEnd; } 
	auto size() const noexcept { return (size_t)(mEnd - data()); }
	auto front() const noexcept { return *data(); }
	auto back() const noexcept { return *(data() + size() - 1); }

	codereader substr(size_t _pos, size_t _size) { codereader r(*this); r.mCursor += _pos; r.mEnd = r.mCursor + _size; return r; }

	// ========================================================================
	//! 読み込みの残りが空っぽか調べます。
	// ------------------------------------------------------------------------
	bool empty() const noexcept { return data() == mEnd; }


	// ========================================================================
	//! 空白と改行をスキップします。
	// ------------------------------------------------------------------------
	//! \return
	//!		スキップした結果、まだデータが残っている場合は true を返します。
	//!		つまり (!empty()) と同じ結果を返します。
	// ------------------------------------------------------------------------
	bool skip_blunk();


	// ========================================================================
	//! 空白をスキップします。
	// ------------------------------------------------------------------------
	//! \return
	//!		スキップした結果、まだデータが残っている場合は true を返します。
	//!		つまり (!empty()) と同じ結果を返します。
	// ------------------------------------------------------------------------
	bool skip_space();


	codereader& skip_return() { while (!empty() && (front() == '\r' || front() == '\n')) { next(); } return *this; }

	// ========================================================================
	//! 指定した種類のデータを読み取ります。
	// ------------------------------------------------------------------------
	std::pair<std::string_view, read_types> read(unsigned int _types);


	// ========================================================================
	//! 1行を読み取ります。
	// ------------------------------------------------------------------------
	std::string_view read_line();


	// ========================================================================
	//! 指定したトークンとマッチするか調べます。
	// ------------------------------------------------------------------------
	//! マッチした場合は読み取り位置を進めます。
	// ------------------------------------------------------------------------
	std::string_view match_starts(std::initializer_list<std::string_view> _tokens);


	// ========================================================================
	//! 読み取り位置を進めます。
	// ------------------------------------------------------------------------
	codereader& next() { operator ++ (); return *this; }


	// ========================================================================
	//! 読み取り位置を進めます。
	// ------------------------------------------------------------------------
	codereader& next(unsigned int _off) { operator += (_off); return *this; }

	// ========================================================================
	//! 読み取り位置を戻します。
	// ------------------------------------------------------------------------
	codereader& prev(unsigned int _off) { while (_off > 0) { --_off; --mCursor; if (*mCursor == '\n') { --mLine; } } return *this; }


	// ========================================================================
	//! 現在の位置を取得します。
	// ------------------------------------------------------------------------
	positiondata position() const noexcept
	{
		return	{mCursor, mLine};
	}


	// ========================================================================
	//! 現在の位置を変更します。
	// ------------------------------------------------------------------------
	codereader& position(positiondata const& _pos) noexcept
	{
		mCursor	= _pos.cursor;
		mLine	= _pos.line;
		return	*this;
	}


	auto& operator * () const noexcept { return *data(); }
	auto& operator [] (int _i) const noexcept { return data()[_i]; }
	codereader& operator ++ () noexcept { if (*mCursor == '\n') { ++mLine; } ++mCursor; return *this;  }
	codereader& operator += (unsigned int _off) noexcept { while (_off > 0) { operator ++(); --_off; } return *this;  }

	operator std::string_view() const noexcept { return {data(), size()}; }

	// ========================================================================
	//! 現在の位置から RAW String の読み込みを行います。
	// ------------------------------------------------------------------------
	//! RAW stringではない場合は empty() を返します。
	//!	return
	//!		first	文字列全体を示す string_view
	//!		second	delimiterを除いた文字列を示す string_view
	// ------------------------------------------------------------------------
	auto read_raw_string() noexcept -> std::pair<std::string_view, std::string_view>;

	// ========================================================================
	//! 現在の位置からヒアドキュメントの読み込みを行います。
	// ------------------------------------------------------------------------
	//! ヒアドキュメントではない場合は empty() を返します。
	// ------------------------------------------------------------------------
	auto read_hiredociment() noexcept -> std::string_view;

private:
	vmstring	mSsource;
	vmstring	mFilename;
	char const*	mCursor	= nullptr;
	char const*	mEnd	= nullptr;
	int			mLine	= 1;
};



PASSION_NAMESPACE_END
namespace std {

template<class CharT, class Traits>
basic_ostream<CharT, Traits>& operator << (basic_ostream<CharT, Traits>& _os, PASSION_NAMESPACE::codereader const& _s)
{
	return	_os << (std::string_view)_s;
}

}
#endif
