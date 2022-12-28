/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <signal.h>
#include <amtrs/console/editline.hpp>
#include <amtrs/console/escape.hpp>
#include <amtrs/filesystem/enumfiles.hpp>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/string/format.hpp>
#include <amtrs/string/multibyte.hpp>
#include <amtrs/string/split_iterator.hpp>
#include <amtrs/system/env.hpp>
AMTRS_CONSOLE_NAMESPACE_BEGIN
using namespace amtrs::console;
using namespace amtrs::strutil;

#if	defined(DEBUG) || defined(_DEBUG)
#define	_LOGGING(m, ...)	\
	{\
		io::fdesc	log;\
		log.append("log.txt", 0x03);\
		log << format("{}({}):", {__FILE__, __LINE__});\
		log << format(m, { __VA_ARGS__ });\
		log << "\n";\
	}
#else
#define	_LOGGING(...)
#endif


static bool is_mb_secondary(text data, char const* pos)
{
	mbstate_t	mbs	= {0};
	auto		cur	= data.s;
	auto		end	= pos < data.e ? pos : data.e;
	while (cur < end)
	{
		mbstate_t	mbs	= {0};
		int			len	= static_cast<int>(mbrlen(cur, end - cur, &mbs));
		if (len == -2)
		{
			return	true;	// 途中
		}
		if (len == -1)
		{
			++cur;		// 無効な文字はとりあえずスキップ
			continue;
		}
		cur += len;
	}
	return	false;
}

editline::editline(io::fdesc _output, io::fdesc _input)
	: terminal(_output, _input)
{
}

int editline::edit_position() const noexcept
{
	return	mEditPosition;
}

int editline::set_edit_position(int pos)
{
	auto	curpos	= terminal::position();
	mEditPosition	= char_count(pos);
	cursor_move(out(), {(mPromptSize + mEditPosition) - curpos.x, 0});
	return	mEditPosition;
}

int editline::move_edit_position_size(int pos, int _vec) const noexcept
{
	int	def	= pos;
	if (_vec < 0)
	{
		if (pos > 0)
		{
			do
			{
				--pos;
			} while (is_mb_secondary(mBuff, &mBuff[pos]) && (pos > 0));
		}
	}
	else
	{
		if (pos < mBuff.size())
		{
			do
			{
				++pos;
			} while (is_mb_secondary(mBuff, &mBuff[pos]) && (pos < mBuff.size()));
		}
	}
	return	pos - def;
}



bool editline::add_history(sstring const& _txt)
{
	// 重複する項目を検索し、みつかった場合は登録せずに入れ替えを行う
	for (size_t i = 0; i < mHistory.size(); ++i)
	{
		if (mHistory[i] == _txt)
		{
			// 重複する項目があったので最後尾に入れ替える
			for (size_t j = i + 1; j < mHistory.size(); ++j)
			{
				mHistory[j-1].swap(mHistory[j]); 
			}
			return	true;
		}
	}
	// 重複がなかったので最後尾に追加
	mHistory.push_back(_txt);
	return	true;
}

void editline::file_find()
{
	simplearray<sstring>		mlist;
	size_t						maxlen	= 0;

	simplearray<text>			paths;
	sstring						cur	= filesystem::current_path<sstring>();
	sstring						env;

	text	patturn(mBuff);
	text	dir	= patturn.substr(0, 0);
	auto	replacePos	= patturn.find_last_of(' ');
	if (replacePos != text::npos)
	{
		++replacePos;
		patturn	= patturn.substr(replacePos);
		if (patturn.front() == '"' || patturn.front() == '\'')
		{
			++replacePos;
			patturn	= patturn.substr(1);
		}
	}

	if (patturn.empty())
	{
		paths.push_back(cur);
		if (!patturn.empty())
		{
			env		= system::getenv<sstring>("PATH");
			for (auto p : split_chars(env, system::env_separator()))
			{
				paths.push_back(p);
			}
		}
	}
	else if (amtrs::filesystem::is_directory(patturn) && !patturn.ends_with("/."))
	{
		paths.clear();
		if (patturn.back() != '/')
		{
			mlist.push_back(sstring(patturn) + "/");
			maxlen = patturn.size() + 1;
		}
		else
		{
			paths.push_back(patturn);
			dir		= patturn;
			patturn	= patturn.substr(0, 0);
		}
	}
	else
	{
		// has parentdir
		dir	= amtrs::filesystem::parent_path(patturn);
		if (!dir.empty())
		{
			paths.push_back(dir);
			patturn	= patturn.substr(dir.size());
		}
		else
		{
			paths.push_back(cur);
			if (!patturn.empty())
			{
				env		= system::getenv<sstring>("PATH");
				for (auto p : split_chars(env, system::env_separator()))
				{
					paths.push_back(p);
				}
			}
		}
	}
	sstring	wildcard;
	if (!patturn.empty())
	{
		wildcard.append(patturn);
		wildcard.append("*");
	}
	for (auto& findpath : paths)
	{
		int	err	= amtrs::filesystem::enumfiles(findpath, wildcard, [&](amtrs::filesystem::enumentry& stat) -> bool
		{
			if (stat.name == "." || stat.name == "..")
			{
				return	true;
			}
			if ((replacePos == text::npos) && !(stat.type & amtrs::filesystem::enumentry::fst_execute))
			{
				return	true;
			}
		//	if (patturn.empty() || text(stat.name).substr(0, patturn.size()) == patturn)
			{
				if (stat.type & amtrs::filesystem::enumentry::fst_directory)
				{
					stat.name.append("/");
				}
				else if (stat.type & amtrs::filesystem::enumentry::fst_symlink)
				{
					sstring	path	= amtrs::filesystem::readlink<sstring>(sstring(findpath) + "/" + stat.name);
					if (amtrs::filesystem::is_directory(path))
					{
						stat.name.append("/");
					}
				}
				mlist.push_back(stat.name);
				if (maxlen < stat.name.size())
				{
					maxlen = stat.name.size();
				}
			}
			return	true;
		});
		if (err)
		{
		//	out() << findpath << ": " << strerror(err) << "\n";
		}
	}
	if (mlist.empty())
	{
		return;
	}

	// 複数マッチしたけど途中まで文字列が一緒ならそれをいったん確定とする
	if (mlist.size() > 1)
	{
		text	match	= text(mlist.front());
		for (auto& m : mlist)
		{
			size_t	minlen		= match.size() < m.size() ? match.size() : m.size();
			int		matchlen	= 0;
			while (matchlen < minlen && match[matchlen] == m[matchlen])
			{
				++matchlen;
			}
			match	= match.substr(0, (size_t)matchlen);
			if (match.empty())
			{
				break;
			}
		}
		if (!match.empty() && match.size() > patturn.size())
		{
			sstring	m(match);
			mlist.clear();
			mlist.push_back(std::move(m));
		}
	}

	if (mlist.size() == 1)
	{
		auto&	match	= mlist.front();
		sstring	txt	= mBuff;
		if (replacePos != text::npos)
		{
			txt	= text(txt).substr(0, replacePos);
			txt.append(dir);
		}
		else
		{
			txt	= dir;
		}
		txt.append(match);
		update(std::move(txt), static_cast<int>(txt.size()));
		return;
	}
	out() << new_line;
	
	auto	wndsize	= terminal::size();
	int		xpos	= 0;
	constexpr int	padd	= 2;
	for (auto& f : mlist)
	{
		auto	len	= f.size();
		if (xpos + maxlen + padd >= wndsize.width)
		{
			xpos	= 0;
			out().write_noconv(new_line);
		}
		if (xpos) { out().write_noconv("  "); }
		out().write_noconv(f);
		for (size_t dif = maxlen - len; dif > 0; --dif)
		{
			out().write_noconv(" ");
		}
		xpos	+= (int)(maxlen + padd);
	}
	out() << new_line;
	prompt();
	set_edit_position(0);
	if (mBuff.size() > 0)
	{
		out().write_noconv(text(mBuff.data(), mBuff.size()));
		set_edit_position(static_cast<int>(mBuff.size()));
	}
}

void editline::find_history(int _vec)
{
	if (mHistory.empty())
	{
		return;
	}
	int	select	= mHistorySelect;
	for (;;)
	{
		select += _vec;
		if (select < 0)
		{
			return;
		}

		// 前方一致のみマッチさせる
		if (mBuff.size() > 0 && mInputedSize > 0)
		{
			// もう候補がないので終了
			if (select < 0 || select >= (int)mHistory.size())
			{
				if (select >= (int)mHistory.size())
				{
					update(text(mBuff).substr(0, mInputedSize));
					mHistorySelect	= select;
				}
				return;
			}
			auto&	h		= mHistory[select];
			bool	match	= true;
			for (int i = 0; i < mInputedSize; ++i)
			{
				if (h.data()[i] != mBuff.data()[i])
				{
					match	= false;
					break;
				}
			}
			if (!match)
			{
				continue;
			}
			// 確定
			update(h, static_cast<int>(h.size()));
			mHistorySelect	= select;
			return;
		}
		if (select < 0 || select >= (int)mHistory.size())
		{
			update("");
			mHistorySelect	= select < -1 ? 0 : (int)mHistory.size();
			return;
		}
		// 確定
		auto&	h	= mHistory[select];
		update(h, static_cast<int>(h.size()));
		mHistorySelect	= select;
		break;
	}
}

int editline::char_count(int pos)
{
	int		charCount	= 0;
	int		cur			= 0;
	int		size		= (int)mBuff.size();
	while (cur != size && cur != pos)
	{
		int	charLen	= move_edit_position_size(cur, 1);
		cur 		+= charLen;
		charCount	+= charLen == 1 ? 1 : 2;
	}
	return	charCount;
}


void editline::position_reset(int _x, bool _clear)
{
	auto	cpos	= terminal::position();
	auto	targetX	= mPromptSize + char_count(edit_position());
	cursor_move(out(), {targetX - cpos.x, 0});
	if (_clear)
	{
		out() << clear_line;
	}
}

void editline::remove_history(int _num)
{
	if (_num >= 0 && _num < mHistory.size())
	{
		for (int i = _num; i < mHistory.size()-1; ++i)
		{
			mHistory[i] = std::move(mHistory[i+1]);
		}
		mHistory.resize(mHistory.size()-1);
	}
}

void editline::save_position()
{
	mDefPositionX	= (int)0;

	auto	curpos	= terminal::position();
	mDefPositionX	= (int)curpos.x;
	mPositionX		= 0;
}

void editline::prompt()
{
	sstring	path	= filesystem::current_path<sstring>();
	sstring	home	= system::homedir<sstring>();
	if (path == home)
	{
		path	= "~";
	}

	sstring	p		= mPrompt.replace("\\u", system::username<sstring>()).replace("\\h", system::hostname<sstring>()).replace("\\w", path);
	mPromptSize		= static_cast<int>(text(p.data()).size());
	out().write_noconv(p);
}

void editline::reset()
{
	mBuff.clear();
	terminal::reset();
}

void editline::prompt(sstring _ps)
{
	mPrompt	= std::move(_ps);
}

bool editline::readline(sstring& _dest)
{
	prompt();
	set_edit_position(0);
	if (mBuff.size() > 0)
	{
		out().write_noconv(text(mBuff.data(), mBuff.size()));
		set_edit_position(static_cast<int>(mBuff.size()));
	}
	mHistorySelect	= (int)mHistory.size();
	mInputedSize		= 0;
	while (!is_done())
	{
		// 改行までを返す
		auto	cpos	= terminal::position();
		size_t	defsize	= mBuff.size();
		size_t	pos		= 0;
		for (pos = 0; pos < mBuff.size(); ++pos)
		{
			if (mBuff.data()[pos] == '\n' || mBuff.data()[pos] == '\r')
			{
				_dest	= text(mBuff).substr(0, pos);
				if (!_dest.empty())
				{
					add_history(_dest);
				}
				mBuff			= text(mBuff).substr(pos + 1);
				out() << new_line;
				return	true;
			}
		}
		// キーを読み取ってバッファに追加する
		auto	in	= terminal::getkey();
		if (is_done() || in.code == ck_sigint)
		{
			done(false);
			mMBPendingSize	= 0;
			mBuff.clear();
			_dest.clear();
			out() << new_line;
			return	false;
		}
		if (!is_ctrlkey(in.code) || (in.code != ck_up && in.code != ck_down))
		{
			mHistorySelect	= (int)mHistory.size();
		}

		if (is_ctrlkey(in.code))
		{
			// コントロールキー
			if (in.code == ck_up || in.code == ck_down)
			{
				find_history(in.code == ck_up ? -1 : 1);
			}
			if (in.code == ck_left || in.code == ck_right)
			{
				int	offset	= move_edit_position_size(edit_position(), in.code == ck_left ? -1 : 1);
				set_edit_position(edit_position() + offset);
			}
			// バックスペース/デリート
			if (in.code == ck_backspace || in.code == ck_delete)
			{
				// 編集位置を計算
				int	delPos	= edit_position();
				int	charLen	= 0;
				if (in.code == ck_backspace)
				{
					charLen	= move_edit_position_size(edit_position(), -1);
					delPos	+= charLen;
					charLen	= -charLen;
				}
				else
				{
					charLen	= move_edit_position_size(edit_position(),  0);
				}

				if (delPos >= 0 && delPos < mBuff.size())
				{
					sstring	buff	= mBuff;
					for (int i = delPos; i < buff.size() - charLen; ++i)
					{
						buff.data()[i] = buff.data()[i+charLen];
					}
					buff.resize(buff.size()-charLen);
					if (in.code == ck_backspace)
					{
						set_edit_position(edit_position() - charLen);
					}

					// 内容を更新
					update(buff);

					mInputedSize	= (int)mBuff.size();
				}
			}
			continue;
		}
		// tab
		if (in.code == 0x09)
		{
			file_find();
			mInputedSize	= (int)mBuff.size();
			continue;
		}
		// ctrl+d
		if (in.code == 0x04)
		{
			if (mBuff.empty())
			{
				update("exit");	
				_dest	= "exit";
				out() << new_line;
				return	false;
			}
			continue;
		}
		if (in.code == '\n' || in.code == '\r')
		{
			_dest	= std::move(mBuff);
			if (!_dest.empty())
			{
				add_history(_dest);
			}
			out() << new_line;
			return	true;
		}

		// マルチバイト対策
		mMBPendingBuff[mMBPendingSize++]	= (char)in.code;
		int	copySize	= (int)mbstrlen({&mMBPendingBuff[0], (size_t)mMBPendingSize});
		if (copySize == 0)
		{
			continue;
		}

		// バッファを拡張
		auto	size	= mBuff.size();
		sstring	buff;
		if (!buff.resize(size + copySize))
		{
			return	false;
		}
		// 挿入位置より前のデータをコピー
		auto	dest		= buff.data();
		auto	src			= mBuff.data();
		int		insertPos	= edit_position();
		for (size_t i = 0; i < insertPos; ++i)
		{
			*dest++ = *src++;
		}
		for (size_t i = 0; i < copySize; ++i)
		{
			*dest++ = (char)mMBPendingBuff[i];
		}
		for (size_t i = insertPos; i < mBuff.size(); ++i)
		{
			*dest++ = *src++;
		}

		// ペンディングバッファをシフト
		for (int i = 0; i < mMBPendingSize - copySize; ++i)
		{
			mMBPendingBuff[i] = mMBPendingBuff[mMBPendingSize - copySize + i];
		}
		mMBPendingSize -= copySize;

		// 読み取ったキーを表示する
		update(buff);
		mInputedSize	=  (int)mBuff.size();
	}
	return	false;
}

bool editline::update(sstring _txt, int _pos)
{
	// current:  ABCDEFG
	// uptext:   ABCDxEFG
	// matchLen:     ^ (4)
	auto	current		= mbtext(mBuff);								// 現在表示済みのもの
	auto	uptext		= mbtext(_txt);									// 更新後のもの
	auto	matchLen	= current.match_first_of(uptext).size();		// 一致している長さを前方から調べる	
	matchLen	= mbtext(current.substr(0, matchLen)).size();			// マルチバイトの境界に合わせる

_LOGGING("{} => {}", current, uptext)

	// 変更なしなら何もしない
	if (matchLen == current.size() && matchLen == uptext.size())
	{
		return	false;
	}

	// 変更のあった部分を書き換える
	set_edit_position(static_cast<int>(matchLen));
	if (uptext.size() > matchLen)
	{
		out().write_noconv(text(_txt.data() + matchLen, uptext.size() - matchLen));
	}
	if (uptext.size() < current.size())
	{
		// 短くなっているならカーソルより後ろを削除
		out().write_noconv(clear_line);
	}
	mBuff		= std::move(_txt);

	// カーソル位置を変更のあった次の文字へカーソルを移動する
	auto	cursor	= _pos;
	if (cursor < 0)
	{
		cursor	= static_cast<int>(matchLen + 1);
		while (is_mb_secondary(uptext.data(), uptext.data() + cursor))
		{
			++cursor;
		}
	}
	set_edit_position(cursor);
_LOGGING("set position {}", cursor)
	return	true;
}

AMTRS_CONSOLE_NAMESPACE_END
