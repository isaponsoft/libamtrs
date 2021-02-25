/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__java__lang__Object__hpp
#define	__libamtrs__java__java__lang__Object__hpp
#include "../../../string.hpp"
#include "../../../jni.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {


struct	Object;
using	ObjectClass		= jcls<Object>;
using	ObjectObject	= jobj<Object>;

struct	String;

struct	Object
{
	using					jni_type	= jobject;
	using					class_type	= Object;
	static constexpr auto	classname() noexcept { return make_constring("java/lang/Object"); }
	static constexpr auto	signature() noexcept { return make_constring("Ljava/lang/Object;"); }

	// ========================================================================
	//! 静的メソッド／メンバ。
	// ------------------------------------------------------------------------
	struct	statics	: local_cls
	{
		using	statics_base	= statics;
		using	super_type		= local_cls;
		using	handle_type		= typename super_type::pointer;
		using	ref_type		= super_type;

		using	super_type::super_type;
		using	super_type::operator =;
	};

	// ========================================================================
	//! 動的メソッド／メンバ。
	// ------------------------------------------------------------------------
	struct	dynamics : local_obj
	{
		using	dynamics_base	= dynamics;
		using	ref_type		= local_obj;
		using	handle_type		= typename local_obj::pointer;
		using	local_obj::local_obj;
		using	local_obj::operator =;
		dynamics() = default;
		dynamics(const dynamics&  _r) = default;
		dynamics(      dynamics&& _r) = default;
		dynamics& operator = (const dynamics&  _r) = default;
		dynamics& operator = (      dynamics&& _r) = default;

		dynamics(const local_obj& _r) : local_obj(_r) {}
		dynamics(local_obj&& _r) : local_obj(_r) {}
		dynamics& operator = (const local_obj&  _r){ local_obj::operator = (_r); return *this; }
		dynamics& operator = (      local_obj&& _r){ local_obj::operator = (_r); return *this; }

		explicit dynamics(jobject _o) : local_obj(_o) {}


		// int hashCode()
		AMTRS_JAVA_DEFINE_METHOD(
			hashCode,
			jint()
		)


		AMTRS_JAVA_DEFINE_METHOD(
			toString,
			String()
		)
	};
};



}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
