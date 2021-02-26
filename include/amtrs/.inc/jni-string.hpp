/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__string__hpp
#define	__libamtrs__java__string__hpp
AMTRS_JAVA_NAMESPACE_BEGIN

inline const local_string& to_jstring(const local_string& _string)
{
	return	_string;
}

inline local_string&& to_jstring(local_string&& _string)
{
	return	std::move(_string);
}


//!	jstring へ変換します。
inline local_string to_jstring(const char* _string)
{
	return	local_string::make(get_jnienv()->NewStringUTF(_string));
}

//!	jstring へ変換します。
template<std::size_t N>
inline local_string to_jstring(char const (&_string)[N])
{
	return	local_string::make(get_jnienv()->NewStringUTF(_string));
}


//!	jstring へ変換します。
template<class Traits, class Allocator>
local_string to_jstring(const std::basic_string<char, Traits, Allocator>& _string)
{
	return	to_jstring(_string.c_str());
}

//!	jstring へ変換します。
inline local_string to_jstring(std::string_view _sv)
{
	std::string	tmp(_sv);
	return	to_jstring(tmp);
}




inline const local_string& to_jstring(JNIEnv* _env, const local_string& _string)
{
	return	_string;
}

inline local_string&& to_jstring(JNIEnv* _env, local_string&& _string)
{
	return	std::move(_string);
}


//!	jstring へ変換します。
inline local_string to_jstring(JNIEnv* _env, const char* _string)
{
	return	local_string::make(_env->NewStringUTF(_string));
}

//!	jstring へ変換します。
template<std::size_t N>
inline local_string to_jstring(JNIEnv* _env, char const (&_string)[N])
{
	return	local_string::make(_env->NewStringUTF(_string));
}


//!	jstring へ変換します。
template<class Traits, class Allocator>
local_string to_jstring(JNIEnv* _env, const std::basic_string<char, Traits, Allocator>& _string)
{
	return	to_jstring(_env, _string.c_str());
}

//!	jstring へ変換します。
inline local_string to_jstring(JNIEnv* _env, std::string_view _sv)
{
	std::string	tmp(_sv);
	return	to_jstring(_env, tmp);
}

AMTRS_JAVA_NAMESPACE_END



AMTRS_STD_NAMESPACE_BEGIN

//!	jstring から jstring へ変換します。
//! stdライブラリの拡張機能です。
template<class Traits = std::char_traits<char>, class Allocator = std::allocator<char>>
inline std::basic_string<char, Traits, Allocator> to_string(JNIEnv* _env, jstring _string)
{
	std::basic_string<char, Traits, Allocator>	retval;
	if (_string)
	{
		auto	chars(_env->GetStringUTFChars(_string, 0));
		retval	= std::string(chars);
		_env->ReleaseStringUTFChars(_string, chars);
	}
	return	retval;
}

//!	jstring から jstring へ変換します。
//! stdライブラリの拡張機能です。
template<class Traits = std::char_traits<char>, class Allocator = std::allocator<char>>
inline std::basic_string<char, Traits, Allocator> to_string(jstring _string)
{
	std::basic_string<char, Traits, Allocator>	retval;
	if (_string)
	{
		auto	chars(amtrs::java::get_jnienv()->GetStringUTFChars(_string, 0));
		retval	= std::string(chars);
		amtrs::java::get_jnienv()->ReleaseStringUTFChars(_string, chars);
	}
	return	retval;
}

AMTRS_STD_NAMESPACE_END
#endif
