/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/main.hpp>

extern "C" jlong _libamtrs_gui_main(amtrs_application_class* _activity);
extern "C" JNIEXPORT jlong JNICALL Java_jp_libamtrs_ActivityInitializer_initialize(JNIEnv* _env, jclass _clz, jobject _activity);


class	autoload
{
public:
	autoload()
	{
		AMTRS_SYSTRACE_LOG("***************************************************************");
		AMTRS_SYSTRACE_LOG("libamtrs startup initializer");
		AMTRS_SYSTRACE_LOG("--------------------------------------------------------------");
		activityInitialize	= (void*)&Java_jp_libamtrs_ActivityInitializer_initialize;
	}

	void*	activityInitialize;
};



// ============================================================================
// Automatic initializer.
// ----------------------------------------------------------------------------
// set(LIBAMTRS_LINKER_FLAGS	"-u libamtrs_initializer")
// ----------------------------------------------------------------------------
autoload	libamtrs_initializer;


extern "C" JNIEXPORT jlong JNICALL Java_jp_libamtrs_ActivityInitializer_initialize(JNIEnv* _env, jclass _clz, jobject _activity)
{
	#ifdef	AMTRS_SYSTRACE_LOG
	AMTRS_SYSTRACE_LOG("=======================================================================");
	AMTRS_SYSTRACE_LOG("=-----------------------                       -----------------------=");
	AMTRS_SYSTRACE_LOG("=------------              Application start              ------------=");
	AMTRS_SYSTRACE_LOG("=-----------------------                       -----------------------=");
	AMTRS_SYSTRACE_LOG("=======================================================================");
	#endif
//	amtrs::java::set_jnienv(_env);
	return	_libamtrs_gui_main(reinterpret_cast<amtrs_application_class*>(_activity));
}


extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* _vm, void* _reserved)
{
	AMTRS_SYSTRACE_LOG("=======================================================================");
	AMTRS_SYSTRACE_LOG("=-----------------------                       -----------------------=");
	AMTRS_SYSTRACE_LOG("=------------                  JNI OnLoad                 ------------=");
	AMTRS_SYSTRACE_LOG("=-----------------------                       -----------------------=");
	AMTRS_SYSTRACE_LOG("=======================================================================");
	amtrs::java::set_javavm(_vm);

	JNIEnv*	env;
	if (_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
	{
		AMTRS_CRITICAL_LOG("JNI GetEnv error");
		return	JNI_ERR;
	}
	amtrs::java::set_jnienv(env);
	return JNI_VERSION_1_6;
}




AMTRS_JAVA_NAMESPACE_BEGIN




AMTRS_IMPLEMENTS_BEGIN(jnienv)
template<class JNIENV>
struct	res
{
	static JNIENV*& value()
	{
		thread_local JNIENV*	s_env	= nullptr;
		return	s_env;
	}
};
AMTRS_IMPLEMENTS_END(jnienv)





struct	jnienv_keep
{
	jnienv_keep()
	{
		if (auto vm = get_javavm(); vm)
		{
			vm->AttachCurrentThread(&env, nullptr);
			autodelete	= true;
		}
	}

	~jnienv_keep()
	{
		if (autodelete)
		{
			get_javavm()->DetachCurrentThread();
		}
	}

	JNIEnv*	env			= nullptr;
	bool	autodelete	= false;
};


static			JavaVM*			tVM	= nullptr;
thread_local	jnienv_keep		tEnv;


JNIEnv* get_jnienv()
{
	return	tEnv.env;
}



void set_jnienv(JNIEnv* _env)
{
	tEnv.env	= _env;
}


JavaVM* get_javavm()
{
	return	tVM;
}


void set_javavm(JavaVM* _vm)
{
	tVM	= _vm;
}

AMTRS_JAVA_NAMESPACE_END
