/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <algorithm>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/string/split_iterator.hpp>
#include <amtrs/string/sstring.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

bool absolute_path(amtrs_bufferif_one_init _destinate, text _path, text _current_path)
{
	// もともと絶対パスが指定されているならそのままバッファへコピーする
	if (is_absolute(_path))
	{
		return	normalize_path(_destinate, _path, directory_separators(), "/");
	}

	sstring	tmp;
	tmp.append(_current_path);
	tmp.append("/");
	tmp.append(_path);
	return	normalize_path(_destinate, tmp, directory_separators(), "/");
}


bool absolute_path(amtrs_bufferif_one_init _destinate, text _path)
{
	sstring	cur	= current_path<sstring>();
	if (cur.empty())
	{
		return	false;
	}
	return	absolute_path(_destinate, _path, cur);
}


text extension(text _path) noexcept
{
	text	path(_path);
	auto	pos1	= path.find_last_of('.');
	if (pos1 == text::npos)
	{
		return	path.substr(path.size());
	}
	auto		pos2	= path.find_last_of(directory_separators());
	if (pos2 == text::npos || pos1 > pos2)
	{
		return	path.substr(pos1);
	}
	return	path.substr(path.size());
}


text filename(text _path) noexcept
{
	text	path(_path);
	if ((path == ".") || (path == ".."))
	{
		return	path.substr(0, 0);
	}

	auto		pos	= path.find_last_of(directory_separators());
	if (pos == text::npos)
	{
		return	path;
	}
	return	path.substr(pos+1);
}


bool normalize_path(amtrs_bufferif_one_init _destinate, text _path, text _separators, text _replaced) noexcept
{
	bool				fstSep	= false;
	bool				lstSep	= false;
	text				path	= _path;

	// パスがないならそのまま終了
	if (path.empty())
	{
		return	reinterpret_cast<char*>(_destinate.allocate(_destinate.object, 0)) ? true : false;
	}

	// 最初と最後にセパレータがあれば処理しやすいように外しておく
	fstSep	= _separators.find_first_of(path.front()) != text::npos;	// セパレータから始まっている
	if (fstSep)
	{
		path	= path.substr(1);
	}
	lstSep	= _separators.find_first_of(path.back()) != text::npos;	// セパレータで終了している
	if (lstSep)
	{
		path	= path.substr(0, path.size() - 1);
	}

	// ノードを分解する
	simplearray<text>	nodes;
	for (auto dir : split_chars(path, _separators))
	{
		if (dir.empty() || dir == ".")
		{
			continue;
		}
		if (dir == "..")
		{
			if (nodes.size() > 0)
			{
				nodes.pop_back();
			}
			continue;
		}
		nodes.push_back(dir);
	}

	// バッファの長さをカウント
	size_t	size		= (fstSep ? _replaced.size() : 0) + (lstSep ? _replaced.size() : 0);
	bool	fastPath	= true;
	for (auto& n : nodes)
	{
		if (fastPath)
		{
			fastPath =  false;
		}
		else
		{
			size     += _replaced.size();
		}
		size += n.size();
	}

	// バッファを確保
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, size));
	if (!data)
	{
		return	false;
	}
	char*	dest	= reinterpret_cast<char*>(data);

	// 先頭のセパレータ
	if (fstSep)
	{
		std::copy(_replaced.begin(), _replaced.end(), dest);
		dest += _replaced.size();
	}

	fastPath	= true;
	for (auto dir : nodes)
	{
		if (fastPath)
		{
			fastPath = false;
		}
		else
		{
			std::copy(_replaced.begin(), _replaced.end(), dest);
			dest += _replaced.size();
		}
		std::copy(dir.begin(), dir.end(), dest);
		dest += dir.size();
	}

	// 最後尾のセパレータ
	if (lstSep)
	{
		std::copy(_replaced.begin(), _replaced.end(), dest);
		dest += _replaced.size();
	}

	return	true;
}


text parent_path(text _path) noexcept
{
	text	path(_path);
	if (path.empty() || (path.size() == 1 && is_separator(path.front())))
	{
		return	path;
	}
	auto	seppos	= path.find_last_of(directory_separators());
	if (seppos == text::npos)
	{
		return	path.substr(0, 0);
	}
	path	= path.substr(0, seppos);
	return	path.empty()
			? path.substr(0, 1)
			: path;
}


text parent_path(text _path, size_t _level) noexcept
{
	text	path(_path);
	for (; _level > 0; --_level)
	{
		path	= parent_path(path);
	}
	return	path;
}


bool replace_path_separator(amtrs_bufferif_one_init _destinate, text _path, text _separators, text _replaced) noexcept
{
	bool		fstSep	= false;
	bool		lstSep	= false;
	text		path	= _path;

	// パスがないならそのまま終了
	if (path.empty())
	{
		return	reinterpret_cast<char*>(_destinate.allocate(_destinate.object, 0)) ? true : false;
	}

	// 最初と最後にセパレータがあれば処理しやすいように外しておく
	fstSep	= _separators.find_first_of(path.front()) != text::npos;	// セパレータから始まっている
	if (fstSep)
	{
		path	= path.substr(1);
	}
	lstSep	= _separators.find_first_of(path.back()) != text::npos;	// セパレータで終了している
	if (lstSep)
	{
		path	= path.substr(0, path.size() - 1);
	}

	// バッファの長さをカウント
	size_t	size		= (fstSep ? _replaced.size() : 0) + (lstSep ? _replaced.size() : 0);
	bool	fastPath	= true;
	for (auto dir : split_chars(path, _separators))
	{
		if (dir.empty())
		{
			continue;
		}
		if (fastPath)
		{
			fastPath =  false;
		}
		else
		{
			size     += _replaced.size();
		}
		size += dir.size();
	}

	// バッファを確保
	auto*	data	= reinterpret_cast<char*>(_destinate.allocate(_destinate.object, size));
	if (!data)
	{
		return	false;
	}
	char*	dest	= reinterpret_cast<char*>(data);

	// 先頭のセパレータ
	if (fstSep)
	{
		std::copy(_replaced.begin(), _replaced.end(), dest);
		dest += _replaced.size();
	}

	fastPath	= true;
	for (auto dir : split_chars(path, _separators))
	{
		if (dir.empty())
		{
			continue;
		}
		if (fastPath)
		{
			fastPath = false;
		}
		else
		{
			std::copy(_replaced.begin(), _replaced.end(), dest);
			dest += _replaced.size();
		}
		std::copy(dir.begin(), dir.end(), dest);
		dest += dir.size();
	}

	// 最後尾のセパレータ
	if (lstSep)
	{
		std::copy(_replaced.begin(), _replaced.end(), dest);
		dest += _replaced.size();
	}

	return	true;
}


text root_directory(text _path) noexcept
{
	text	path(_path);
	text	rn(root_name(_path));
	if (!rn.empty())
	{
		path	= path.substr(rn.size());
	}
	if (!path.empty() && is_separator(path.front()))
	{
		return	path.substr(0, 1);
	}
	return	path.substr(0, 0);
}


text root_path(text _path) noexcept
{
	text	path(_path);
	auto	rn		= root_name(path);
	auto	rd		= root_directory(path);
	if (rn.empty() && rd.empty())
	{
		return	path.substr(0, 0);
	}
	return	path.substr(0, rd.size() + rn.size());
}


text stem(text _path) noexcept
{
	text	path(filename(_path));
	auto	ext	= extension(path);
	return	path.substr(0, path.size() - ext.size());
}


AMTRS_FILESYSTEM_NAMESPACE_END
