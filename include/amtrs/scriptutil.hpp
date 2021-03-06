/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__libamtrs__scriptutil__hpp
#define	__libamtrs__libamtrs__scriptutil__hpp
#include "amtrs.hpp"
#include <deque>
#include <string>
#define	AMTRS_SCRIPTUTIL_NAMESPACE			AMTRS_NAMESPACE::ssu
#define	AMTRS_SCRIPTUTIL_NAMESPACE_BEGIN	namespace AMTRS_SCRIPTUTIL_NAMESPACE {
#define	AMTRS_SCRIPTUTIL_NAMESPACE_END		}
AMTRS_SCRIPTUTIL_NAMESPACE_BEGIN


enum class update_result : int
{
	err		= 0,
	modify	= 1,
	skip	= 2,
};
static constexpr update_result	update_err		= update_result::err;
static constexpr update_result	update_modify	= update_result::modify;
static constexpr update_result	update_skip		= update_result::skip;


inline bool is_good(update_result _r) noexcept { return _r == update_modify || _r == update_skip; }
inline bool is_bad(update_result _r) noexcept { return !is_good(_r); }


struct	global
{
	std::deque<std::string>	argv;
};	


template<class T>
using	array	= std::deque<T>;


//!
std::string array_shift(std::deque<std::string>& _array);


/*!
 * _text の内容を _targetfile へ書き込みます。
 * ただし、この関数は最初に _targetfile を読み取り、_texeの内容と
 * 違いが見つからない場合は書き込みをスキップします。
 * また、ディレクトリが存在しない場合は自動的にディレクトリを作成します。
 *
 * _noupdate
 *		trueの時は実際の書き込みを行わいで終了します。
 * 
 *
 * return
 *		書き込みに失敗した場合は 0 
 *		書き込みに成功した場合は 1
 *		書き込みをスキップした場合は 2
 */
update_result update(std::string_view _targetfile, std::string_view _text, bool _noupdate = false);


/*!
 * windows の場合はファイル名に .exe を付与します。
 * それ以外の場合は何もしません。
 */
std::string add_exe(std::string_view _filename);


/*!
 * ディレクトリセパレターを / に置き換えます。
 */
std::string slashdir(std::string_view _filename);

/*!
 * ディレクトリセパレターを \ に置き換えます。
 * _escape が true の時は \\ でエスケープします。
 */
std::string backslashdir(std::string_view _filename, bool _escape);


/*!
 * コマンドを実行し、結果を文字列として受け取ります。
 * return
 * 	>= 0	コマンドの終了コード
 *  <  0	エラー(そもそも実行できなかった)
 */
int exec(std::string& _out, std::string& _err, std::string const& _command);


/*!
 * コマンドを実行し、結果を文字列として受け取ります。
 * stderr はそのままパイプされます。
 * return
 * 	>= 0	コマンドの終了コード
 *  <  0	エラー(そもそも実行できなかった)
 */
int exec(std::string& _out, std::string const& _command);


/*!
 * コマンドを実行します。
 * return
 * 	>= 0	コマンドの終了コード
 *  <  0	エラー(そもそも実行できなかった)
 */
int exec(std::string const& _command);


AMTRS_SCRIPTUTIL_NAMESPACE_END
#endif
