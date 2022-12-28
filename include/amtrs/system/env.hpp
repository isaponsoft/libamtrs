/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__env__hpp
#define	__libamtrs__system__env__hpp
#include "../memory/bufferif.h"
#include "../string/text.hpp"
AMTRS_SYSTEM_NAMESPACE_BEGIN

enum class specialpathtype
{
	home			= 0,		// %USERPROFILES%/                    ${HOME}/
	cache			= 1,		// home/AddData/Local/_appname/		  home/.cache/_appname/
	app_local		= 2,		// home/AddData/Local/_appname/		  home/.lcaol/_appname/
	app_backup		= 3,		// home/AddData/Roaming/_appname/     home/.lcaol/_appname/
	config			= 4,		// home/AddData/Local/_appname/       home/.config/_appname/
};

/*
 * 環境変数のセパレータを取得します。
 */
AMTRS_API text env_separator();

/*
 * 実行プログラムのパスを返します。
 */
AMTRS_API bool get_execute_path(amtrs_bufferif_one_init _destinate, char const* _argv0);

//! 環境変数を取得します
// この関数を使う代わりに getenv<sstring>(name); を使うと任意の文字列型で
// 受け取ることができます。
AMTRS_API bool getenv(amtrs_bufferif_one_init _destinate, text _name);

//! 環境変数を設定します
AMTRS_API void setenv(text _key, text _value);

//! プラットフォームの特殊なファイルパスを取得します。
// この関数を使う代わりに special_path<sstring>(specialpathtype::home, name); を使うと任意の文字列型で
// 受け取ることができます。
AMTRS_API bool special_path(amtrs_bufferif_one_init _destinate, specialpathtype _type, text _appname);

//! コンピュータ名を取得します
AMTRS_API bool hostname(amtrs_bufferif_one_init _destinate);

//! ユーザー名名を取得します
AMTRS_API bool username(amtrs_bufferif_one_init _destinate);


/*
 * Get locale. RFC3282.
 */
sstring get_locale_rfc3282();


template<class T>
bool get_execute_path(T& _destinate, char const* _argv0)
{
	return	get_execute_path(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _argv0);
}

template<class T>
T get_execute_path(char const* _argv0)
{
	T		dest;
	bool	r	= get_execute_path(dest, _argv0);
	return	dest;
}


template<class T>
bool getenv(T& _destinate, char const* _name)
{
	return	getenv(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _name);
}

template<class T>
T getenv(char const* _name)
{
	T		dest;
	bool	r	= getenv(dest, _name);
	return	dest;
}


template<class T>
bool special_path(T& _destinate, specialpathtype _type, text _appname = {})
{
	return	special_path(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _type, _appname);
}


template<class T>
T special_path(specialpathtype _type, text _appname = {})
{
	T	retval;
	special_path(retval, _type, _appname);
	return	retval;
}


template<class T>
bool hostname(T& _destinate)
{
	return	hostname(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	});
}

template<class T>
T hostname()
{
	T	retval;
	hostname(retval);
	return	retval;
}


template<class T>
bool username(T& _destinate)
{
	return	username(amtrs_bufferif_one_init
	{
		&_destinate,
		[](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	});
}

template<class T>
T username()
{
	T	retval;
	username(retval);
	return	retval;
}


// ホームディレクトリを取得します
template<class T> T homedir() { return special_path<sstring>(specialpathtype::home, ""); }


AMTRS_SYSTEM_NAMESPACE_END
#endif
