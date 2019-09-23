#ifndef	__libamtrs__java__class_bridge__hpp
#define	__libamtrs__java__class_bridge__hpp
#include "../logging/@"
#include "def.hpp"
#include "ref.hpp"
#include "call_init.hpp"
#include "call_method.hpp"
#include "new_object.hpp"
#include "jni_traits.hpp"
#include "jcls.hpp"
#include "jobj.hpp"
AMTRS_JAVA_NAMESPACE_BEGIN





// ========================================================================
//! Javaのクラスを宣言します。
// ------------------------------------------------------------------------
//!	AMTRS_JAVA_DEFINE_CLASS(クラス名, ベースクラス名)
// ------------------------------------------------------------------------
//!	example)
//!		namespace org::mydomain {
//!		using namespace	amtrs::java::classes;
//!
//!		// MyClass の宣言
//!		AMTRS_JAVA_DEFINE_CLASS(MyClass, java::lang::Object)
//!		{
//!			// クラスのシグネチャを宣言
//!			AMTRS_JAVA_CLASS_SIGNATURE("org/mydomain/MyClass")
//!
//!			
//!		}
//!
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DEFINE_CLASS(_classname, _baseclass)	\
		struct	_classname;\
		using	_classname##Class	= amtrs::java::jcls<_classname>;\
		using	_classname##Object	= amtrs::java::jobj<_classname>;\
		struct	_classname : amtrs::java::define_class<_classname, _baseclass>


// ========================================================================
//! クラスのシグネチャを定義します。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_CLASS_SIGNATURE(_classname)	\
	using					jni_type	= jobject;\
	static constexpr auto	classname() noexcept { return amtrs::make_constring(_classname); }\
	static constexpr auto	signature() noexcept { return amtrs::make_constring("L" _classname ";"); }\


// ========================================================================
//! 静的メンバの宣言ブロックを開始します。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DEFINE_STATIC_MEMBER	\
		struct	statics : define_member<typename base_java_class::statics>


// ========================================================================
//! 静的メンバブロックの基本的宣言を行います。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_STATICS_BASIC	\
		using	_base_type	= define_member<typename base_java_class::statics>;\
		using	_base_type::_base_type;\
		using	_base_type::operator =;\


// ========================================================================
//! 動的メンバの宣言ブロックを開始します。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER	\
		struct	dynamics : define_member<typename base_java_class::dynamics>


// ========================================================================
//! 動的メンバブロックの基本的宣言を行います。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DYNAMICS_BASIC	\
		using	_base_type	= define_member<typename base_java_class::dynamics>;\
		using	_base_type::_base_type;\
		using	_base_type::operator =;\


// ========================================================================
//! Javaメソッドのラッパーインターフェースを作ります。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DEFINE_METHOD(_name, ...)	\
		template<class... Args>\
		auto _name(Args... _args)\
		{\
			overload_call<__VA_ARGS__>	func;\
			JNIEnv*						env	= get_jnienv();\
			handle_type					h	= ref_type::get();\
			if constexpr (std::is_same<handle_type, jobject>::value) { auto c = local_cls::make(env->GetObjectClass(h)); return func(#_name, env, h, c.get(), typename jni_traits<Args>::argv_type(_args)...); }\
			if constexpr (std::is_same<handle_type, jclass >::value) {                                                   return func(#_name, env, h,          typename jni_traits<Args>::argv_type(_args)...); }\
		}



// ========================================================================
//! フィールドを定義します。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DEFINE_FIELD(_type, _name)	\
	auto get_##_name()\
	{\
		constexpr auto	name	= make_constring(#_name);\
		constexpr auto	field	= define_field<decltype(name), _type>(#_name);\
		handle_type		h	= get();\
		return	field.get(h);\
	}


// ========================================================================
//! init(コンストラクタ)を定義します。
// ------------------------------------------------------------------------
#define	AMTRS_JAVA_DEFINE_INIT(...)	\
		template<class... Argv>\
		auto init(Argv&&... _argv)\
		{\
			return	call_init<class_type, __VA_ARGS__>{}(get_jnienv(), get(), std::forward<Argv>(_argv)...);\
		}




template<class Class, class BaseClass>
struct	define_class : BaseClass
{
	using	base_java_class		= BaseClass;
	using	class_type			= Class;


	// ========================================================================
	//! クラスを検索して返します。
	// ------------------------------------------------------------------------
	static jcls<class_type> find()
	{
		jclass	cls	= get_jnienv()->FindClass(jni_traits<Class>::classname().c_str());
		if (!cls)
		{
			AMTRS_DEBUG_LOG("No such class \"%s\".", jni_traits<Class>::classname().c_str());
		}
		return	jcls<Class>::make(cls);
	}

	template<class... Argv>
	static jobj<class_type> init(Argv&&... _argv)
	{
		return	find().init(std::forward<Argv>(_argv)...);
	}


	template<class BaseMember>
	struct	define_member : BaseMember
	{
		using	class_type		= Class;
		using	_base_type		= BaseMember;
		using	handle_type		= typename BaseMember::handle_type;
		static constexpr auto	classname() noexcept { return class_type::classname(); }
		static constexpr auto	signature() noexcept { return class_type::signature(); }

		using	_base_type::_base_type;
		using	_base_type::operator =;


	protected:
		template<class NameT, class Type>
		struct	define_field
		{
			using					class_type		= Class;
			using					jni_traits_type	= jni_traits<Type>;
			using					jni_type		= typename jni_traits_type::jni_type;

			const NameT		fieldname;

			constexpr define_field(NameT _name)
				: fieldname(_name)
			{}

			auto get(handle_type _handle) const
			{
				JNIEnv*		e	= get_jnienv();
				if constexpr (std::is_same<handle_type, jclass>::value)
				{
					jfieldID	f		= e->GetStaticFieldID(_handle, fieldname.c_str(), jni_traits_type::signature().c_str());
					if constexpr (std::is_same<jni_type, float>::value || std::is_same<jni_type, jfloat>::value)
					{
						return	e->GetStaticFloatField(_handle, f);
					}
					else if constexpr (std::is_same<jni_type, jint>::value)
					{
						return	e->GetStaticIntField(_handle, f);
					}
					else if constexpr (std::is_same<jni_type, jobject>::value)
					{
						auto	ref		= local_obj::make(e->GetStaticObjectField(_handle, f));
						auto	retval	= jobj<Type>(std::move(ref));
						return	retval;
					}
					else
					{
						unkown_return_type<jni_type, jni_type>();
					}
				}

				if constexpr (std::is_same<handle_type, jobject>::value)
				{
					auto		c		= local_cls::make(e->GetObjectClass(_handle));
					jfieldID	f		= e->GetFieldID(c.get(), fieldname.c_str(), jni_traits_type::signature().c_str());
					if constexpr (std::is_same<jni_type, jfloat>::value || std::is_same<jni_type, jfloat>::value)
					{
						return	e->GetFloatField(_handle, f);
					}
					else if constexpr (std::is_same<jni_type, jint>::value)
					{
						return	e->GetIntField(_handle, f);
					}
					else if constexpr (std::is_same<jni_type, jobject>::value)
					{
						return	jobj<Type>(local_obj::make(e->GetObjectField(_handle, f)));
					}
					else
					{
						unkown_return_type<jni_type, jni_type>();
					}
				}
			}
		};



	private:
		template<class T1, class T2>
		static void unkown_return_type()
		{
			static_assert(std::is_same<T1, T2>::value, "unkown type");
		}
	};
};



AMTRS_JAVA_NAMESPACE_END
#endif
