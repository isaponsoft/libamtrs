/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__apgen__apgen__hpp
#define	__apgen__apgen__hpp
#include <list>
#include <map>
#include <amtrs/filesystem.hpp>
#include <amtrs/io.hpp>
#include <amtrs/net.hpp>
#include <amtrs/string.hpp>
#include <amtrs/system.hpp>
#include <amtrs/scriptutil.hpp>

extern std::string	gAmtrsHome;

namespace apgen {

//! apgenのステータス
struct	status
{
	mutable std::map<std::string, std::string>	values;
};



//! 指定したバッファへURLのデータをダウンロードします。
bool download(std::string& _dest, std::string const& _url);

//! URLのデータをダウンロードし、ファイルに書き込みます。
bool download(std::string const& _url);


//! バッファの内容を展開し、ファイルに書き込みます。
//! 戻り値は最初のエントリがディレクトリだった場合、ディレクトリ名を返します。
std::string unzip(void const* _buff, size_t _size, std::string const& _dir = {});


std::string unzip(amtrs::io::vstreamif& _in);


//! ファイルを上書きします。
//! ただし内容に変化がなければ何もしません。
bool update(std::string const& _target, std::string const& _data);


void error(std::string const& _msg);
void report(std::string const& _report);


std::string replace_macro(std::string const& _txt, apgen::status const& _status);

std::string process(status& _status, std::string const& _filename, std::string const& _txt);
std::string process(bool& _error, status& _status, std::string const& _filename, std::string const& _txt);


void load_conf(status& _status, std::string const& _apgenconf, bool& _ec);


struct	option
{
	std::string										genType;		//!< -G "xxx"
	std::string										genFile;		//!< APGen.txt
	std::list<std::pair<std::string, std::string>>	args;
	int												loglevel	= 3;
};


class	generator_base
{
public:
	virtual ~generator_base() {}
	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) = 0;
	virtual int execute(status const& _state, option const& _opt) = 0;
};


struct	autolink
{
	using	generator_type	= generator_base*(option const&);

	struct	gen
	{
		gen(std::string_view _name, generator_type _p)
			: name(_name)
			, proc(_p)
		{}

		std::string_view		name;
		generator_type*			proc	= nullptr;
		mutable gen const*		next	= nullptr;
	};

	autolink(std::string_view _name, generator_type _p)
	{
		auto	g	= new gen(_name, _p);
		g->next	= _link();
		_link()	= g;
	}

	static gen const*& _link()
	{
		static gen const*	_;
		return	_;
	}
};

}
#endif
