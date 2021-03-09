/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__libamtrs__scriptutil__hpp
#define	__libamtrs__libamtrs__scriptutil__hpp
#include "filesystem.hpp"
#include <deque>
#include <functional>
#include <string>
#define	AMTRS_SCRIPTUTIL_NAMESPACE			AMTRS_NAMESPACE::ssu
#define	AMTRS_SCRIPTUTIL_NAMESPACE_BEGIN	namespace AMTRS_SCRIPTUTIL_NAMESPACE {
#define	AMTRS_SCRIPTUTIL_NAMESPACE_END		}
AMTRS_SCRIPTUTIL_NAMESPACE_BEGIN

template<class T>
using	span_callback	= void(T*, size_t);

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
 * windows の場合はファイル名に .bat を付与します。
 * unix に似たOSではファイル名に .sh を付与します。
 */
std::string add_bat(std::string_view _filename);


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
 * シェルスクリプトまたはバッチを実行し設定された環境変数を列挙します。
 * _envscript
 *		環境設定用スクリプト
 * _workfile
 *		環境設定用スクリプトを実行するための一時ファイルのパス
 *		enumenvsは一時ファイルに環境変数取得用のスクリプトを作成します。
 */
bool enumenvs(std::vector<std::pair<std::string, std::string>>& _dest, std::string const& _envscript, std::string const& _workfile);


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

/*!
 * ファイルをダウンロードします。
 * _savename を省略した場合はカレントディレクトリに保存されます。
 * _savename がディレクトリの場合はディレクトリの中に保存されます。
 * _savedname ダウンロードしたファイルを保存した時のファイル名。
 */
bool download(std::string const& _url, std::string const& _savename, std::string* _savedname = nullptr);


struct	extract_message
{
	int			msgid;
	std::string	name;
	size_t		size;
};



/*!
 * アーカイブファイルを展開します。
 */
struct	extract_params
{
	using		callback	= int(extract_message& m);
	std::string				sourcename;					// 展開中のソースデータの名前(progress bar info)
	bool					skiproot	= false;		// 単一のルートディレクトリを持つ場合はルートディレクトリを排除します。
	std::function<callback>	msgproc;					// 展開時のメッセージプロシージャ
};

bool extract(io::vstreamif _in, std::string const& _saveto, extract_params _info = {});
bool extract(std::string const& _file, std::string const& _saveto, extract_params _info = {});


/*!
 * Visual Studio の環境設定を行います。
 * Windows 以外の場合は無視され、常に true を返します。
 */
bool visualstudio_setup();

/*!
 * c++ の開発環境変数を設定します。
 * Windows の場合は visualstudio_setup() を呼び出します。
 */
bool cpp_setup();


/*!
 * ファイルの一覧を取得します。
 */
bool ls(std::string const& _patturn = {});


bool cat(std::string_view _file);
bool cat(std::initializer_list<std::string_view> _files);


AMTRS_SCRIPTUTIL_NAMESPACE_END
#endif
