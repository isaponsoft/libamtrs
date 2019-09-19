#ifndef	__libamtrs__java__java__lang__ClassLoader__hpp
#define	__libamtrs__java__java__lang__ClassLoader__hpp
#include "Object.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {
	struct	File;
	struct	FileInputStream;
	struct	FileOutputStream;
	struct	InputStream;
	struct	OutputStream;
}

namespace java::lang {


AMTRS_JAVA_DEFINE_CLASS(ClassLoader , java::lang::Object)
{
	using	Class				= java::lang::Class;
	using	String				= java::lang::String;
	using	File				= java::io::File;
	using	FileInputStream		= java::io::FileInputStream;
	using	FileOutputStream	= java::io::FileOutputStream;
	using	InputStream			= java::io::InputStream;
	using	OutputStream		= java::io::OutputStream;

	AMTRS_JAVA_CLASS_SIGNATURE("java/lang/ClassLoader");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(getSystemClassLoader
			, ClassLoader()
		)

		AMTRS_JAVA_DEFINE_METHOD(getSystemResource
			, ClassLoader(String name)
		)

		AMTRS_JAVA_DEFINE_METHOD(getSystemResourceAsStream
			, InputStream(String name)
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getSystemResources
			, Enumeration<URL> (String name)
		)*/

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(clearAssertionStatus
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getParent
			, ClassLoader()
		)

//		AMTRS_JAVA_DEFINE_METHOD(getResource
//			, URL(String name)
//		)

		AMTRS_JAVA_DEFINE_METHOD(getResourceAsStream
			, InputStream(String name)
		)

/*		AMTRS_JAVA_DEFINE_METHOD(getResources
			, Enumeration<URL> (String name)
		)*/

		AMTRS_JAVA_DEFINE_METHOD(loadClass
			, Class(String name)
		)


	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
