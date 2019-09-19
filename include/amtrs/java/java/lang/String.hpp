#ifndef	__libamtrs__java__java__lang__String__hpp
#define	__libamtrs__java__java__lang__String__hpp
#include "Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {


AMTRS_JAVA_DEFINE_CLASS(String, Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("java/lang/String");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(length
			, jint()
		)


		std::string to_string()
		{
			auto		chars(amtrs::java::get_jnienv()->GetStringUTFChars((jstring)get(), 0));
			std::string	retval	= std::string(chars);
			amtrs::java::get_jnienv()->ReleaseStringUTFChars((jstring)get(), chars);
			return	retval;
		}
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
