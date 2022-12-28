/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__path__hpp
#define	__libamtrs__filesystem__path__hpp
#include "../memory/bufferif.h"
#include "../string/text.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


/*
 * 絶対パスを返します。
 */
AMTRS_API bool absolute_path(amtrs_bufferif_one_init _destinate, text _path, text _current_path);
AMTRS_API bool absolute_path(amtrs_bufferif_one_init _destinate, text _path);

/*
 * カレントディレクトリを変更します。
 */
AMTRS_API bool current_path(text _path);

/*
 * カレントディレクトリを取得します。
 */
AMTRS_API bool current_path(amtrs_bufferif_one_init _destinate);

/*
 * ディレクトリセパレータとして利用可能な文字の一覧を取得します。
 * 環境によっては複数の文字が返されることがあります。
 */
AMTRS_API text directory_separators() noexcept;

/*
 * 拡張子部分を抜き出します。
 */
AMTRS_API text extension(text _path) noexcept;

/*
 * ファイル名部分を抜き出します。
 */
AMTRS_API text filename(text _path) noexcept;

/*
 * ディレクトリかどうか調べます。
 */
AMTRS_API bool is_directory(text _path);


/*
 * 指定した文字がセパレータかどうか調べます。
 */
inline bool is_separator(char _c) noexcept { return directory_separators().find_first_of(_c) != text::npos; }

/*
 * パスを正規化します。
 * "." や ".."、重複しているセパレータを削除します。
 */
AMTRS_API bool normalize_path(amtrs_bufferif_one_init _destinate, text _path, text _separators, text _replaced) noexcept;
inline    bool normalize_path(amtrs_bufferif_one_init _destinate, text _path, text _replaced) noexcept { return normalize_path(_destinate, _path, directory_separators(), _replaced); }
inline    bool normalize_path(amtrs_bufferif_one_init _destinate, text _path) noexcept { return normalize_path(_destinate, _path, directory_separators(), "/"); }


/* セパレータを置換します。
 *	_separators		: セパレータとして判定する文字のリスト。Unixなら "/", Windowsなら "/\\"
 *	_replaced		: 置換後のセパレータ文字列。
 */
AMTRS_API bool replace_path_separator(amtrs_bufferif_one_init _destinate, text _path, text _separators, text _replaced) noexcept;
inline    bool replace_path_separator(amtrs_bufferif_one_init _destinate, text _path, text _replaced) noexcept { return replace_path_separator(_destinate, _path, directory_separators(), _replaced); }
inline    bool replace_path_separator(amtrs_bufferif_one_init _destinate, text _path) noexcept { return replace_path_separator(_destinate, _path, directory_separators(), "/"); }


/*
 * 親ディレクトリを返します。
 */
AMTRS_API text parent_path(text _path) noexcept;
AMTRS_API text parent_path(text _path, size_t _level) noexcept;


/*
 * シンボリックリンクであればリンク先を読み取ります。
 */
AMTRS_API bool readlink(amtrs_bufferif_one_init _destinate, text _path) noexcept;


/*
 * ルート名部分を抜き出します。
 */
AMTRS_API text root_name(text _path) noexcept;


/*
 * ルートディレクトリ部分を抜き出します。
 */
AMTRS_API text root_directory(text _path) noexcept;


/*
 * ルートパス部分を抜き出します。
 */
AMTRS_API text root_path(text _path) noexcept;

/*
 * 拡張子を含まないファイル名を返します。
 */
text stem(text _path) noexcept;


/*
 * 相対パスかどうか調べます。
 */
inline bool is_relative(text _path) noexcept { return root_path(_path).empty() ? true : false; }


/*
 * 絶対パスかどうか調べます。
 */
inline bool is_absolute(text _path) noexcept { return !is_relative(_path); }



template<class T>
bool absolute_path(T& _destinate, text _path) noexcept
{
	return	absolute_path(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path);
}


template<class T>
T absolute_path(text _path) noexcept
{
	T		dest;
	bool	r	= absolute_path(dest, _path);
	return	dest;
}


template<class T>
T current_path()
{
	T		retval;
	bool	r	= current_path(amtrs_bufferif_one_init
	{
		&retval,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	});
	return	retval;
}


template<class T>
bool replace_path_separator(T& _destinate, text _path, text _separators, text _replaced) noexcept
{
	return	replace_path_separator(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path, _separators, _replaced);
}


template<class T>
T replace_path_separator(text _path, text _separators, text _replaced) noexcept
{
	T		dest;
	bool	r	= replace_path_separator(dest, _path, _separators, _replaced);
	return	dest;
}



template<class T>
bool replace_path_separator(T& _destinate, text _path, text _replaced) noexcept
{
	return	replace_path_separator(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path, _replaced);
}



template<class T>
bool normalize_path(T& _destinate, text _path, text _separators, text _replace) noexcept
{
	return	normalize_path(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path, _separators, _replace);
}


template<class T>
T normalize_path(text _path, text _separators, text _replace) noexcept
{
	T		dest;
	bool	r	= normalize_path(dest, _path, _separators, _replace);
	return	dest;
}


template<class T>
bool normalize_path(T& _destinate, text _path, text _separators) noexcept
{
	return	normalize_path(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path, _separators);
}


template<class T>
T normalize_path(text _path, text _separators) noexcept
{
	T		dest;
	bool	r	= normalize_path(dest, _path, _separators);
	return	dest;
}



template<class T>
bool normalize_path(T& _destinate, text _path) noexcept
{
	return	normalize_path(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path);
}


template<class T>
T normalize_path(text _path) noexcept
{
	T		dest;
	bool	r	= normalize_path(dest, _path);
	return	dest;
}



template<class T>
bool readlink(T& _destinate, text _path) noexcept
{
	return	readlink(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _path);
}


template<class T>
T readlink(text _path) noexcept
{
	T		dest;
	bool	r	= readlink(dest, _path);
	return	dest;
}

AMTRS_FILESYSTEM_NAMESPACE_END
#endif
