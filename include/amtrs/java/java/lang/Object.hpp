#ifndef	__libamtrs__java__java__lang__Object__hpp
#define	__libamtrs__java__java__lang__Object__hpp
#include "../../../java.hpp"
#include "../../../string.hpp"
#include "../../def.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {


struct	Object;
using	ObjectClass		= jcls<Object>;
using	ObjectObject	= jobj<Object>;

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
		using	ref_type	= local_cls;
		using	handle_type	= typename ref_type::pointer;
		using	ref_type::ref_type;
		using	ref_type::operator =;
		statics() = default;
		statics(const statics&  _r) = default;
		statics(      statics&& _r) = default;
		statics& operator = (const statics&  _r) = default;
		statics& operator = (      statics&& _r) = default;

		statics(const ref_type& _r) : ref_type(_r) {}
		statics(ref_type&& _r) : ref_type(_r) {}
		statics& operator = (const ref_type&  _r){ ref_type::operator = (_r); return *this; }
		statics& operator = (      ref_type&& _r){ ref_type::operator = (_r); return *this; }

	};

	// ========================================================================
	//! 動的メソッド／メンバ。
	// ------------------------------------------------------------------------
	struct	dynamics : local_obj
	{
		using	ref_type	= local_obj;
		using	handle_type	= typename local_obj::pointer;
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
	};
};



}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
