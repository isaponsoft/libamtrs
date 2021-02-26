/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <string>
#include <amtrs/filesystem/@>
#include <amtrs/string/join.hpp>
#include <amtrs/string/trim.hpp>
#include <amtrs/system/env.hpp>
#include <psnvm/.error.hpp>
#include <psnvm/.script-opecode.hpp>
#include <psnvm/.preprocessor.hpp>
using namespace amtrs;
namespace fs	= amtrs::filesystem;
PASSION_NAMESPACE_BEGIN namespace preprocessor {



// ========================================================================
//! 引数無しのマクロを追加します。
// ------------------------------------------------------------------------
void setting::define(vmstring _name, vmstring _text, vmstring _filename, int _line)
{
	define(std::move(_name), macro{
		.filename	= std::move(_filename),
		.line		= _line,
		.text		= std::move(_text)
	});
}


// ========================================================================
//! マクロを追加します。
// ------------------------------------------------------------------------
void setting::define(vmstring _name, macro _mcr)
{
	bool	has	= _mcr.has_args;
	defines[_name].macros.push_back(std::move(_mcr));
	if (has)
	{
		defines[_name].has_args	= true;
	}
	else
	{
		defines[_name].has_noarg	= true;
	}
}


// ****************************************************************************
//! プリプロセッサ
// ----------------------------------------------------------------------------
//! c/c++のプリプロセッサに似ているが、単語単位でプリプロセス処理を行う。
// ----------------------------------------------------------------------------
class	prepro
{
public:
	using	processor_type	= bool (prepro::*)(codereader& s);
	using	macrooverload	= setting::macrooverload;

	prepro(reportor* _report = nullptr)
		: mRepotor(_report)
	{}


	// ========================================================================
	//! マクロを追加します。
	// ------------------------------------------------------------------------
	void define(vmstring _name, macro _mcr)
	{
		bool	has	= _mcr.has_args;
		mDefines[_name].macros.push_back(std::move(_mcr));
		if (has)
		{
			mDefines[_name].has_args	= true;
		}
		else
		{
			mDefines[_name].has_noarg	= true;
		}
	}

	void define(vmstring _name, vmstring _text, vmstring _filename, int _line)
	{
		define(std::move(_name), macro{
			.filename	= std::move(_filename),
			.line		= _line,
			.text		= std::move(_text)
		});
	}

	void undef(std::string_view _name)
	{
		auto	it	= mDefines.find(_name);
		if (it != mDefines.end())
		{
			mDefines.erase(it);
		}
	}


	// ========================================================================
	//! 結果を受け取ります。
	// ------------------------------------------------------------------------
	vmstring flush()
	{
		vmstring	retval	= join<vmstring>(mOutbuffer.begin(), mOutbuffer.end(), "");
		mOutbuffer.clear();
		return	retval;
	}



	// ========================================================================
	//! 生成したコードを追加します。
	// ------------------------------------------------------------------------
	void push(vmstring _code)
	{
		mOutbuffer.push_back(std::move(_code));
	}


	// ========================================================================
	//! 現在のソース位置を記録します。
	// ------------------------------------------------------------------------
	void recode_position(codereader const& _source)
	{
		if (mCurrentFilename == _source.filename())
		{
			mOutbuffer.push_back(format<vmstring>("#line %d\n", _source.line()));
		}
		else
		{
			mCurrentFilename	= _source.filename();
			mOutbuffer.push_back(format<vmstring>("#line %d \"%s\"\n", _source.line(), mCurrentFilename.c_str()));
		}
		mCurrentLine	= _source.line();
	}


	// ========================================================================
	//! 結果を受け取ります。
	// ------------------------------------------------------------------------
	reportor& report() const noexcept { return *mRepotor; }


	// ========================================================================
	//! エラーがないか調べる。
	// ------------------------------------------------------------------------
	bool good() const noexcept { return report().empty(); }


	struct	params
	{
		std::pair<std::string_view, std::string_view> const*	start;
		std::size_t												size;
	};




	reportor*										mRepotor;

	vmstring										mCurrentFilename;
	int												mCurrentLine	= 1;

	std::list<vmstring>								mOutbuffer;

	std::unordered_map<vmstring, macrooverload>		mDefines;
	std::vector<vmstring>							mIncludePaths;



	struct	expression_value
	{
		enum type
		{
			integer_value	= 1,
			real_value		= 2,
		};
		int			type;
		union
		{
			intmax_t	i;
		//	double		r;		// 拡張用、現在は未使用
		};
	};

	void push(codereader::positiondata _start, codereader::positiondata _end)
	{
		push({_start.data(), (size_t)(_end.data() - _start.data())});
	}


	// #define を処理する
	bool directive_define(std::string_view directive, codereader s, codereader&)
	{
		if (directive != "define")
		{
			return	false;
		}

		// マクロ名を読み取る
		auto	name	= s.read(codereader::macroname_type).first;
		if (name.empty())
		{
			// 名前がない
			report().push({pp_define_has_not_name, s});
			return	false;
		}
		s.skip_blunk();

		std::vector<vmstring>	params;
		bool					hasParams	= false;

		// ( があればパラメータ解析
		if (!s.empty() && *s == '(')
		{
			hasParams	= true;

			++s;
			if (!s.skip_space())
			{
				// エラー閉じるカッコがない
				report().push({pp_define_unclosed_parameter, s, name});
				return	false;
			}

			if (*s != ')')
			{
				for (;;)
				{
					if (!s.skip_blunk())
					{
						// エラー閉じるカッコがない
						report().push({pp_define_unclosed_parameter, s, name});
						return	false;
					}

					char const*	begin	= s.data();
					auto [token, type] = s.read(codereader::identify_type);
					if (type != codereader::identify_type)
					{
						// エラー:名前以外のものがある
						report().push({pp_define_unkown_parameter_name, s, name, vmstring(s.data(), 1)});
						return	false;
					}
					params.push_back({begin, s.data()});
					if (!s.skip_blunk())
					{
						// エラー閉じるカッコがない
						report().push({pp_define_unclosed_parameter, s, name});
						return	false;
					}
					// 仮パラメータ終了
					if (*s == ')')
					{
						++s;
						break;
					}
					if (*s != ',')
					{
						// エラー:カンマ以外のものがる
						report().push({pp_define_not_parameter_separator, s, name, vmstring(s.data(), 1)});
						return	false;
					}
					++s;
				}
			}
		}

		define(name, {
			.filename	= s.filename(),
			.line		= s.line(),
			.params		= shared_buffer<vmstring>(params.begin(), params.end()),
			.has_args	= hasParams,
			.text		= (std::string_view)s
		});

		return	true;
	}



	bool directive_undef(std::string_view directive, codereader s, codereader&)
	{
		if (directive != "undef")
		{
			return	false;
		}

		// マクロ名を読み取る
		auto	name	= s.read(codereader::macroname_type).first;
		if (name.empty())
		{
			// 名前がない
			report().push({pp_undef_has_not_name, s});
			return	false;
		}
		s.skip_blunk();

		auto	it	= mDefines.find(name);
		if (it == mDefines.end())
		{
			// エラー（見つからない）
			report().push({pp_undef_nothing, s, name});
			return	false;
		}
		mDefines.erase(it);
		return	true;
	}



	bool directive_include(std::string_view directive, codereader s, codereader&)
	{
		if (directive != "include")
		{
			return	false;
		}

		vmstring	filename;
		vmstring	file;
		if (!s.empty() && s.front() == '<')
		{
			// 検索パス優先
			++s;
			// > が見つかるまでを抜き出す
			char const*	begin	= s.data();
			while (!s.empty() && s.front() != '>')
			{
				++s;
			}
			if (s.empty())
			{
				// エラー（>が見つからない)
				report().push({pp_include_unclose_filename, s});
				return	true;
			}
			filename	= vmstring(begin, (size_t)(s.data() - begin));
			file		= find_from_includes(filename);
			if (file.empty())
			{
				report().push({
					pp_include_not_include_paths,
					s,
					filename
				});
				return	true;
			}
			++s;
		}
		else
		{
			// 相対パス優先
			auto	[name, type] = s.read(codereader::string_type);
			if (type != codereader::string_type)
			{
				// エラー（ファイル名が見つからない）
				report().push({pp_include_unclose_filename, s});
				return	true;
			}
			filename	= vmstring(trim(name, "\""));
			file		= vmstring(fs::parent_path((std::string_view)s.filename())) + vmstring("/") + filename;
			if (!fs::exists((std::string_view)file))
			{
				report().push({
					pp_include_cant_open,
					s,
					filename
				});
				return	true;
			}
		}

		recode_position({vmstring(file), vmstring()});
		proc({file, fs::file_get_contents<std::string>((std::string_view)file)});
		return	true;
	}



	bool directive_ifelse(std::string_view directive, codereader s, codereader& src)
	{
		if (directive != "if" && directive != "ifdef" && directive != "ifndef")
		{
			return	false;
		}
		if (s.empty())
		{
			// エラー（条件式が無い）
			if (directive == "ifdef")
			{
				report().push({pp_ifdef_nothing_macro_name, s});
			}
			else
			{
				report().push({pp_if_nothing_expression_value, s});
			}
			return	false;
		}

		auto				start		= s.position();
		bool				processed	= false;
		std::string_view	enabled		= {s.data(), 0};
		std::string_view	next		= {s.data(), 0};
		if (directive == "ifdef" || directive == "ifndef")
		{
			auto	[name, nametype]	= s.read(codereader::identify_type);
			if (nametype != codereader::identify_type)
			{
				// エラーマクロ名がない
				report().push({pp_ifdef_nothing_macro_name, s});
				return	false;
			}

			auto	block	= read_ifelse_block(src);
			if (mDefines.find(name) != mDefines.end())
			{
				if (directive == "ifdef")
				{
					processed	= true;
					if (!good())
					{
						return	false;
					}
					enabled	= block.first;
				}
			}
			else
			{
				if (directive == "ifndef")
				{
					processed	= true;
					if (!good())
					{
						return	false;
					}
					enabled	= block.first;
				}
			}
			next	= block.second;
		}
		else if (directive == "if")
		{
			auto	a		= ifelse_expression(s);
			if (!good())
			{
				return	true;
			}
			auto	block	= read_ifelse_block(src);
			if (a.i != 0)
			{
				processed	= true;
				if (!good())
				{
					return	false;
				}
				enabled	= block.first;
			}
			next	= block.second;
		}
		if (next.empty())
		{
			report().push({pp_if_nothing_endif, s, directive});
			return	false;
		}


		while (next != "endif")
		{
			if (next == "else")
			{
				auto	block	= read_ifelse_block(src);
				if (block.second != "endif")
				{
					// エラー: else の後に endif がない
					report().push({pp_if_nothing_endif, s, directive});
					return	false;
				}

				if (!processed)
				{
					enabled		= block.first;
					processed	= true;
				}
				break;
			}

			auto	line	= src.read_line();

			// elif
			auto	a		= ifelse_expression({s.filename(), line, start.line});
			if (!good())
			{
				return	true;
			}
			auto	block	= read_ifelse_block(src);
			if (!processed && a.i != 0)
			{
				enabled		= block.first;
				processed	= true;
			}
			next	= block.second;
		}

		if (enabled.size() > 0)
		{
			push(subproc({s.filename(), enabled, s.line()}));
		}
		return	true;

	}


	bool directive_foreach(std::string_view directive, codereader s, codereader& src)
	{
		if (directive != "foreach")
		{
			return	false;
		}

		std::string_view	counter;
		std::string_view	iterator;
		std::string_view	split;

		if (*s != '(')
		{
			// (がない
			return	false;
		}
		s.next().skip_blunk();
		counter	= s.read(codereader::macroname_type).first;
		if (!s.skip_blunk() || *s != ',')
		{
			// ,がない
			return	false;
		}

		s.next().skip_blunk();
		iterator	= s.read(codereader::macroname_type).first;
		if (!s.skip_blunk() || *s != ',')
		{
			// ,がない
			return	false;
		}

		s.next().skip_blunk();
		split		= s.read(codereader::string_type).first;
		if (!s.skip_blunk() || *s != ')')
		{
			// )がない
			return	false;
		}
		s.next().skip_blunk();


		split	= trim(split, "\"");


		auto	[block, term]	= foreach_read_block(src);
		auto	values			= trim((std::string_view)s, " \t\r\n");
		int		count			= 0;
		while (!values.empty())
		{
			auto	pos	= values.find_first_of(split);
			auto	val	= values;
			if (pos == values.npos)
			{
				values	= {nullptr, 0};
			}
			else
			{
				val		= values.substr(0, pos);
				values	= values.substr(pos + split.size());
			}
			define(counter,		format<vmstring>("%d", count++), s.filename(), s.line());
			define(iterator,	val, s.filename(), s.line());

			push(subproc({s.filename(), block, s.line()}));

			undef(counter);
			undef(iterator);	
		}
		return	true;
	}


	std::pair<std::string_view, std::string_view> foreach_read_block(codereader& s)
	{
		static constexpr std::string_view	none(nullptr, 0);
		auto	begin	= s.position();
		int		nest	= 0;
		for (;;)
		{
			if (s.empty())
			{
				// エラー（閉じられていない）
				return	{ none, none };
			}

			// 文字列など識別可能なトークンは読み飛ばす
			auto	r	= s.read(~0);
			if (r.second != codereader::unkown_type)
			{
				continue;
			}

			if (*s == '#')
			{
				auto	beforeDirectivePosition	= s.position();

				++s;
				s.skip_blunk();
				if (s.empty())
				{
					// エラー（閉じられていない）
					return	{ none, none };
				}

				auto	kws			= s.position();
				auto	[w, type]	= s.read(codereader::identify_type);
				bool	match		= false;
				if (w == "foreach")
				{
					++nest;
				}
				if (type != codereader::identify_type || (w != "endfor"))
				{
					continue;
				}
				if (nest > 0)
				{
					if (w == "endfor")
					{
						--nest;
					}
					continue;
				}
				return	{
					{begin.data(), (size_t)(beforeDirectivePosition.data() - begin.data())},
					{kws.data(), (size_t)(s.data() - kws.data())}
				};
			}

			++s;
		}
	}


	void proc(codereader s)
	{
		using	directive_type	= bool (prepro::*)(std::string_view, codereader, codereader&);
		directive_type	directives[] = {
			&prepro::directive_define,
			&prepro::directive_undef,
			&prepro::directive_include,
			&prepro::directive_ifelse,
			&prepro::directive_foreach
		};


		// 行単位で処理を行う
		while (!s.empty() && good())
		{
			auto	start	= s.position();


			// 空白をスキップ
			s.skip_space();
			if (s.empty())
			{
				push(start, s.position());
				break;
			}

			// -#の場合は # 以降をそのまま出力
			if (!s.match_starts({"-#"}).empty())
			{
				s.prev(1);
				push(s.read_line());
				continue;
			}

			// ディレクティブの処理
			if (*s == '#')
			{
				s.next().skip_space();
				auto	line		= codereader{s.filename(), s.read_line(), start.line};
				while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
				{
					line	= line.substr(0, line.size()-1);
				}
				// directive
				auto	directive	= line.read(codereader::identify_type).first;
				if (!directive.empty())
				{
					line.skip_space();
					bool	processed	= false;
					for (auto proc : directives)
					{
						processed	= (this->*proc)(directive, line, s);
						if (!good())
						{
							return;
						}
						if (processed)
						{
							break;
						}
					}
					if (processed)
					{
						continue;
					}
				}
				s.position(start);
			}

			// 行の置換を行う
			while (!s.empty() && *s != '\r' && *s != '\r')
			{
				auto	prev				= s.position();
				auto	[token, token_type] = s.read(~0);
				if (token_type == codereader::macroname_type)
				{
					if (token == "__FILE__" || token == "__LINE__")
					{
						push(start, s.position());
						push(token == "__FILE__" ? format<vmstring>("\"%s\"", s.filename().c_str()) : format<vmstring>("%d", s.line()));
						start	= s.position();
						continue;
					}
					if (macro_replace(s, token, start, prev))
					{
						start	= s.position();
						continue;
					}
				}
				// 文字列など認識できたトークンはスキップ
				if (token_type != codereader::unkown_type)
				{
					continue;
				}
				// 式展開
				if (!s.match_starts({"%{"}).empty())
				{
					auto	block	= read_expression_block(s);
					if (!s.empty() && *s == '}')
					{
						++s;
						auto	r	= ifelse_expression({s.filename(), block, prev.line});
						push(format<vmstring>("%d", r.i));
						start	= s.position();
						continue;
					}
					s.position(prev);
				}
				++s;
			}


			// 残りを出力
			push(start, s.skip_return().position());
		}
	}


	std::string_view read_expression_block(codereader& s)
	{
		int		nest	= 0;
		auto	pos		= s.position();
		while (!s.empty())
		{
			auto	[tokn, type] = s.read(~0);
			if (type != codereader::unkown_type)
			{
				continue;
			}
			if (*s == '}')
			{
				if (nest == 0)
				{
					return	{pos.data(), (size_t)(s.data() - pos.data())};
				}
				++nest;
			}
			++s;
		}
		s.position(pos);
		return	{};
	}


	vmstring subproc(codereader s)
	{
		std::list<vmstring>	out;
		std::swap(mOutbuffer, out);
		proc(s);
		std::swap(mOutbuffer, out);
		return	join<vmstring>(out.begin(), out.end(), "");
	}



	//! include path からファイルを検索します。
	//! 見つからなければ empty() を返します。
	vmstring find_from_includes(vmstring _path)
	{
		vmstring	paths[2] = {vmstring() , _path};
		for (auto& p : mIncludePaths)
		{
			paths[0]	= p;
			vmstring	path	= join<vmstring>(std::begin(paths), std::end(paths), "/");
			if (filesystem::exists((std::string_view)path))
			{
				return	path;
			}
		}
		return	{};
	}


	bool read_argments(std::vector<vmstring>& args, codereader& s)
	{
		while (!s.empty())
		{
			char const*	begin	= s.data();
			int			bracket	= 0;
			while (!s.empty())
			{
				auto [token, type] = s.read(codereader::string_type);
				if (type == codereader::string_type)
				{
					s.skip_blunk();
					continue;
				}

				if (*s == '(' || *s == '{' || *s == '[')
				{
					++bracket;
				}
				else if (*s == ')')
				{
					if (bracket == 0)
					{
						break;
					}
					--bracket;
				}
				else if (*s == '}' || *s == ']')
				{
					if (bracket > 0)
					{
						--bracket;
					}
				}
				else if (*s == ',' && bracket == 0)
				{
					break;
				}
				++s;
			}
			args.push_back({begin, s.data()});
			if (s.empty())
			{
				break;
			}
			if (*s != ',')
			{
				break;
			}
			++s;
			s.skip_blunk();
		}
		if (!s.empty() && *s == ')')
		{
			++s;
			return	true;
		}
		return	false;
	}


	vmstring replacing(codereader s, params _params)
	{
		std::list<vmstring>		texts;
		char const*				begin	= begin	= s.data();
		char const*				end		= nullptr;
		while (!s.empty())
		{
			end	= s.data();

			if (*s == '#')
			{
				bool	replaced	= false;
				auto	save		= s.position();
				++s;
				if (s.skip_space())
				{
					auto	[token, token_type] = s.read(~0);
					if (token_type == codereader::identify_type)
					{
						for (auto& prm : make_bucket(_params.start, _params.size))
						{
							if (prm.first == (std::string_view)token)
							{
								texts.push_back("\"");
								texts.push_back(prm.second);
								texts.push_back("\"");
								begin		= s.data();
								replaced	= true;
								break;
							}
						}
					}
				}
				if (replaced)
				{
					continue;
				}
				s.position(save);
			}

			// マクロ処理
			auto	[token, token_type] = s.read(~0);
			if (token_type == codereader::identify_type)
			{
				bool	replaced	= false;
				for (auto& prm : make_bucket(_params.start, _params.size))
				{
					if (prm.first == (std::string_view)token)
					{
						texts.push_back({begin, end});
						texts.push_back(subproc({s.filename(), prm.second, s.line()}));

						auto	save	= s.position();
						s.skip_space();
						if (s.match_starts({"##"}) != "##")
						{
							s.position(save);
						}
						begin		= s.data();
						replaced	= true;
						break;
					}
				}
				if (replaced)
				{
					continue;
				}
			}
			// 文字列など認識できたトークンはスキップ
			if (token_type != codereader::unkown_type)
			{
				continue;
			}
			// read() で空白をスキップしたかもしれないので確認
			if (s.empty())
			{
				break;
			}
			++s;
		}

		// 残りのデータを全部出力する
		if (begin != &*s.end())
		{
			texts.push_back({begin, &*s.end()});
		}
		return	join<vmstring>(texts.begin(), texts.end(), "");
	}



	bool macro_replace(codereader& _src, std::string_view identify, codereader::positiondata begin, codereader::positiondata end)
	{
		while (!identify.empty())
		{
			auto	it	= mDefines.find(identify);
			if (it != mDefines.end())
			{
				// 引数のあるマクロは存在しないので即置換する
				if (!it->second.has_args)
				{
					// 保留中のデータを出力
					macro&	m	= it->second.macros.front();
					push(begin, end);
					proc({_src.filename(), m.text, _src.line()});
					return	true;
				}


				codereader	s(_src);
				codereader	s2(_src);
				s2.skip_blunk();
				// ()が無い場合は引数なしのマクロのみを対象とする
				if (s2.empty() || *s2 != '(')
				{
					// 引数なしマクロが存在しないので置換を行わない
					if (!it->second.has_noarg)
					{
						return	false;
					}
					macro&	m	= it->second.macros.front();
					push(begin, end);
					proc({_src.filename(), m.text, _src.line()});
					_src = s;
					return	true;
				}
				s	= s2;
				++s;
				s.skip_blunk();

				// 引数があるっぽいので引数の数を数える
				std::vector<vmstring>	args;
				read_argments(args, s);
				for (auto& m : it->second.macros)
				{
					// 一致するマクロがあればそれに置き換える
					if (m.has_args && m.params.size() == args.size())
					{
						std::vector<std::pair<std::string_view, std::string_view>>	params;
						for (int i = 0; i < args.size(); ++i)
						{
							params.push_back({(std::string_view)m.params[i], (std::string_view)args[i]});
						}
						push(begin, end);
						vmstring	txt	= replacing({_src.filename(), m.text, _src.line()}, {params.data(), params.size()});
						proc({_src.filename(), txt, _src.line()});
						_src = s;
						return	true;
					}
				}

				// 引数なしマクロがあれば置き換える
				if (it->second.has_noarg)
				{
					for (auto& m : it->second.macros)
					{
						// 一致するマクロがあればそれに置き換える
						if (!m.has_args)
						{
							push(begin, end);
							proc({_src.filename(), m.text, _src.line()});
							return	true;
						}
					}
				}
			}

			auto	pos	= identify.find_last_of('.');
			if (pos == identify.npos)
			{
				break;
			}
			_src.prev(identify.size() - pos);
			identify	= identify.substr(0, pos);
		}
		return	false;
	}




	std::pair<std::string_view, std::string_view> read_ifelse_block(codereader& s)
	{
		static constexpr std::string_view	none(nullptr, 0);
		auto	begin	= s.position();
		int		nest	= 0;
		for (;;)
		{
			if (s.empty())
			{
				// エラー（閉じられていない）
				return	{ none, none };
			}

			// 文字列など識別可能なトークンは読み飛ばす
			auto	r	= s.read(~0);
			if (r.second != codereader::unkown_type)
			{
				continue;
			}

			if (*s == '#')
			{
				auto	beforeDirectivePosition	= s.position();

				++s;
				s.skip_blunk();
				if (s.empty())
				{
					// エラー（閉じられていない）
					return	{ none, none };
				}

				auto	kws			= s.position();
				auto	[w, type]	= s.read(codereader::identify_type);
				if (w == "ifdef" || w == "ifndef" || w == "if")
				{
					++nest;
				}
				if (type != codereader::identify_type || (w != "else" && w != "elif" && w != "endif"))
				{
					continue;
				}
				if (nest > 0)
				{
					if (w == "endif")
					{
						--nest;
					}
					continue;
				}
				return	{
					{begin.data(), (size_t)(beforeDirectivePosition.data() - begin.data())},
					{kws.data(), (size_t)(s.data() - kws.data())}
				};
			}

			++s;
		}
	}


	auto ifelse_expression(codereader s) -> expression_value
	{
		codereader	exp{s.filename(), subproc(s), s.line()};
		return	ifelse_expression(exp, opecode_priority::none);
	}


	auto ifelse_expression(codereader& s, opecode_priority _level) -> expression_value
	{
		expression_value	answer;
		answer.type	= expression_value::integer_value;
		answer.i	= 0;
		if (!s.skip_space())
		{
			// エラー：データがなない
			report().push({pp_if_expression_unterminate, s});
			return	answer;
		}


		if (*s == '!')
		{
			++s;
			answer	= ifelse_expression(s, opecode_priority::single_operation);
			answer.i	= answer.i ? 0 : 1;
		}
		else if (*s == '(')
		{
			++s;
			answer	= ifelse_expression(s, opecode_priority::none);
			if (!s.skip_space() || *s != ')')
			{
				// エラー： ) で閉じられていない
				report().push({pp_if_expression_unclosed_circle, s});
				return	answer;
			}
			++s;
		}
		else
		{
			auto	[token, type]	= s.read(codereader::integer_type|codereader::macroname_type);
			if (type == codereader::integer_type)
			{
				answer.type	= expression_value::integer_value;
				answer.i	= 0;
				for (auto c : token)
				{
					answer.i = answer.i * 10 + (c - '0');
				}
			}
			else if (type == codereader::macroname_type)
			{
				if (token == "defined")
				{
					if (!s.skip_space() || *s != '(')
					{
						// エラー：データがなない
						report().push({pp_if_has_not_circle, s});
						return	answer;
					}
					++s;
					s.skip_space();
					auto	identify	= s.read(codereader::macroname_type).first;
					if (identify.empty())
					{
						// エラー： マクロ名が無い
						report().push({pp_if_defined_has_not_name, s});
						return	answer;
					}
					if (!s.skip_space() || *s != ')')
					{
						// エラー： ) で閉じられていない
						report().push({pp_if_has_not_closecircle, s});
						return	answer;
					}
					++s;
					auto	it	= mDefines.find(identify);
					answer.i	= it == mDefines.end() ? 0 : 1;
				}
			}
			else
			{
				report().push({pp_if_expression_unkown, s, vmstring(s.data(), 1)});
				return	answer;
			}
		}

		
		for (;;)
		{
			if (!s.skip_space())
			{
				// 式はこれ以上存在しない
				return	answer;
			}

			auto	save	= s.position();
			auto	code	= s.match_starts({ "+", "-", "*", "/", "%", "&", "|", "^", "==", "!=", ">", ">=", "<", "<=", "&&", "||", "<<", ">>", "?" });
			if (code.empty())
			{
				// エラー：演算子がない
				return	answer;
			}

			#define	__OPECODEPROC(_code, _pri, _expr)	\
				if (code == _code)\
				{\
					if (_level >= opecode_priority::_pri) { s.position(save); return answer; }\
					expression_value	r	= ifelse_expression(s, opecode_priority::_pri);\
					_expr;\
					continue;\
				}\

			__OPECODEPROC("+",  add_sub,     answer.i = answer.i + r.i)
			__OPECODEPROC("-",  add_sub,     answer.i = answer.i - r.i)
			__OPECODEPROC("*",  mul_div,     answer.i = answer.i * r.i)
			__OPECODEPROC("/",  mul_div,     answer.i = answer.i / r.i)
			__OPECODEPROC("%",  mul_div,     answer.i = answer.i % r.i)
			__OPECODEPROC("&",  bitwise,     answer.i = answer.i & r.i)
			__OPECODEPROC("|",  bitwise,     answer.i = answer.i | r.i)
			__OPECODEPROC("^",  bitwise,     answer.i = answer.i ^ r.i)
			__OPECODEPROC("<<", shift,       answer.i = answer.i << r.i)
			__OPECODEPROC(">>", shift,       answer.i = answer.i >> r.i)
			__OPECODEPROC("==", compare,     answer.i = answer.i == r.i ? 1 : 0)
			__OPECODEPROC("!=", compare,     answer.i = answer.i != r.i ? 1 : 0)
			__OPECODEPROC(">",  compare,     answer.i = answer.i >  r.i ? 1 : 0)
			__OPECODEPROC(">=", compare,     answer.i = answer.i >= r.i ? 1 : 0)
			__OPECODEPROC("<",  compare,     answer.i = answer.i <  r.i ? 1 : 0)
			__OPECODEPROC("<=", compare,     answer.i = answer.i <= r.i ? 1 : 0)
			__OPECODEPROC("&&", logic_and,   answer.i = answer.i && r.i ? 1 : 0)
			__OPECODEPROC("||", logic_and,   answer.i = answer.i || r.i ? 1 : 0)
			#undef	__OPECODEPROC

			if (code == "?")
			{
				if (_level >= opecode_priority::conditions)
				{
					s.position(save);
					return	answer;
				}
				auto	v1	= ifelse_expression(s, opecode_priority::none);
				if (!s.skip_space() || *s != ':')
				{
					return	answer;
				}
				++s;

				auto	v2	= ifelse_expression(s, opecode_priority::none);
				answer.i	= answer.i ? v1.i : v2.i;
				continue;
			}

			// 不明
			return	answer;
		}
	}
};



vmstring preprocess(reportor* _report, setting const& _setting)
{
	prepro	pp(_report);
	pp.mDefines			= _setting.defines;
	pp.mIncludePaths	= _setting.include_paths;
	for (auto& s : _setting.sources)
	{
		pp.recode_position(s);
		pp.proc(s);
	}
	return	pp.flush();
}




} PASSION_NAMESPACE_END
