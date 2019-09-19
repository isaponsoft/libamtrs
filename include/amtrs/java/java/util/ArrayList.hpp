#ifndef	__libamtrs__java__jc__java__util_ArrayList__hpp
#define	__libamtrs__java__jc__java__util_ArrayList__hpp
#include "List.hpp"

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::util {


AMTRS_JAVA_DEFINE_CLASS(ArrayList, List)
{

	AMTRS_JAVA_CLASS_SIGNATURE("java/util/ArrayList");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			 ArrayList()
		)
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
