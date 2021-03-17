/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system_env__hpp
#define	__libamtrs__system_env__hpp
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(env)
template<class S>
inline S getenv(char const* _name);
AMTRS_IMPLEMENTS_END(env)

enum class specialpathtype
{
	home			= 0,		// %USERPROFILES%/                    ${HOME}/
	cache			= 1,		// home/AddData/Local/_appname/		  home/.cache/_appname/
	app_local		= 2,		// home/AddData/Local/_appname/		  home/.lcaol/_appname/
	app_backup		= 3,		// home/AddData/Roaming/_appname/     home/.lcaol/_appname/
	config			= 4,		// home/AddData/Local/_appname/       home/.config/_appname/
};



// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
template<class S>
S getenv(char const* _name)
{
	return	AMTRS_IMPLEMENTS(env)::getenv<S>(_name);
}


void setenv(std::string_view _key, std::string_view _value);

//! プラットフォームの特殊なファイルパスを取得します。
bool special_path(amtrs_bufferif_one_init _destinate, specialpathtype _type, std::string_view _appname);



template<class T>
bool special_path(T& _destinate, specialpathtype _type, std::string_view _appname = {})
{
	return	special_path(amtrs_bufferif_one_init
	{
		.object		= &_destinate,
		.allocate	= [](void* _object, size_t _size) -> void*
		{
			T*	dest	= reinterpret_cast<T*>(_object);
			bufferif_one_init_traits<T>::allocate(dest, _size);
			return	dest->data();
		}
	}, _type, _appname);
}


template<class T>
T special_path(specialpathtype _type, std::string_view _appname = {})
{
	T	retval;
	special_path(retval, _type, _appname);
	return	retval;
}

AMTRS_NAMESPACE_END
#define	AMTRS_PLIB_NAME	system-env.hpp
#include "include-platform.hpp"
#endif
