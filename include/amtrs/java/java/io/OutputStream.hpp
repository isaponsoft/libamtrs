#ifndef	__libamtrs__java__jc__java__io__OutputStream__hpp
#define	__libamtrs__java__jc__java__io__OutputStream__hpp
#include <amtrs/java/java/lang.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {

// https://developer.android.com/reference/java/io/OutputStream
AMTRS_JAVA_DEFINE_CLASS(OutputStream, java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("java/io/OutputStream");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(close
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(flush
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(write
			, void(int b)
			, void(jbyteArray b, int off, int len)
			, void(jbyteArray b)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
