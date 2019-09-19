#ifndef	__libamtrs__java__jc__java__util_Collection__hpp
#define	__libamtrs__java__jc__java__util_Collection__hpp
#include <amtrs/java/java/lang.hpp>
#include <amtrs/java/java/lang/Object.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::util {


AMTRS_JAVA_DEFINE_CLASS(Collection, java::lang::Object)
{

	AMTRS_JAVA_CLASS_SIGNATURE("java/util/Collection");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(add
			, jboolean(jobject e)
			, jboolean(jint index, jobject element)
		)


		AMTRS_JAVA_DEFINE_METHOD(clear
			, void()
		)


		AMTRS_JAVA_DEFINE_METHOD(contains
			, jboolean(jobject e)
		)


		AMTRS_JAVA_DEFINE_METHOD(equals
			, jboolean(jobject o)
		)


		AMTRS_JAVA_DEFINE_METHOD(get
			, Object(jint index)
		)


		AMTRS_JAVA_DEFINE_METHOD(indexOf
			, jint(jobject o)
		)


		AMTRS_JAVA_DEFINE_METHOD(isEmpty
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(remove
			, Object(int index)
			, jboolean(jobject o)
		)

		AMTRS_JAVA_DEFINE_METHOD(set
			, Object(int index, jobject element)
		)

		AMTRS_JAVA_DEFINE_METHOD(size
			, jint()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
