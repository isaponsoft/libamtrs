#ifndef	__libamtrs__java__java__lang__CharSequence__hpp
#define	__libamtrs__java__java__lang__CharSequence__hpp
#include "Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::lang {

AMTRS_JAVA_DEFINE_CLASS(CharSequence, Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("java/lang/CharSequence");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
};

}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
