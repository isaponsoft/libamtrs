/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__jc__java__io__InputStream__hpp
#define	__libamtrs__java__jc__java__io__InputStream__hpp
#include <amtrs/java/java/lang.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace java::io {

// https://developer.android.com/reference/java/io/InputStream
AMTRS_JAVA_DEFINE_CLASS(InputStream, java::lang::Object)
{
	using	String				= java::lang::String;

	AMTRS_JAVA_CLASS_SIGNATURE("java/io/InputStream");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;


		AMTRS_JAVA_DEFINE_METHOD(available
			, int()
		)

		AMTRS_JAVA_DEFINE_METHOD(close
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(mark
			, void(int readlimit)
		)

		AMTRS_JAVA_DEFINE_METHOD(markSupported
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(read
			, int()
			, int(jbyteArray b, int off, int len)
			, int(jbyteArray b)
		)

		AMTRS_JAVA_DEFINE_METHOD(reset
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(skip
			, jlong(jlong n)
		)


		// ********************************************************************
		// c++用便利命令
		// ********************************************************************

		//! inputstreamの内容をすべて読み取り、std::string にして返します。
		std::string recv_to_string(std::size_t _workSize = 4086)
		{
			auto		env			= get_jnienv();
			auto		recvBuff	= localref<jbyteArray>::make(env->NewByteArray(_workSize));
			std::string	retval;
			for (;;)
			{
				int	len	= read(recvBuff.get());
				if (len == -1)
				{
					break;
				}
				jbyte*	array		= env->GetByteArrayElements(recvBuff.get(), nullptr);
				retval	+= std::string((const char*)array, (std::size_t)len);
				env->ReleaseByteArrayElements(recvBuff.get(), array, 0);
			}
			return	retval;
		}


	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
