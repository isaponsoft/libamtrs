#ifndef	__libamtrs__utility__singleton__hpp
#define	__libamtrs__utility__singleton__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


AMTRS_IMPLEMENTS_BEGIN(singleton)
template<class T>
struct	destroy_monitor : public T
{
	using	T::T;

	destroy_monitor()
	{
		*instance() = this;
	}

	virtual ~destroy_monitor()
	{
		*instance()	= nullptr;
	}

	static destroy_monitor** instance()
	{
		static destroy_monitor*	_instance	= nullptr;
		return	&_instance;
	}
};
AMTRS_IMPLEMENTS_END(singleton)



template<class T, class... Args>
T* get_singleton_instance(Args&&... _args)
{
	T*	retval	= *AMTRS_IMPLEMENTS(singleton)::destroy_monitor<T>::instance();
	if (!retval)
	{
		retval	= new AMTRS_IMPLEMENTS(singleton)::destroy_monitor<T>(std::forward<Args>(_args)...);
	}
	return	retval;
}



template<class T>
struct	basic_creater;


template<class T, class CreaterT = basic_creater<T>>
class	singleton
{
	using	_instance_type	= T;
	using	_creator_type	= CreaterT;

	static auto create_instance() -> decltype(_creator_type{}()) { return _creator_type{}(); }

	using	_return_type	= decltype(create_instance());

public:
	static _return_type get_instance();

protected:
	singleton();
	~singleton();

private:
	static _instance_type** instance();
};



template<class T>
struct	basic_creater
{
	T* operator () () const
	{
		return	new T();
	}
};



template<class T, class CreaterT>
singleton<T, CreaterT>::singleton()
{
	//AMTRS_DEBUG_LOG("singleton <%s> : create", typeid(*this).name());
	*instance()	= static_cast<_instance_type*>(this);
}



template<class T, class CreaterT>
singleton<T, CreaterT>::~singleton()
{
	//AMTRS_DEBUG_LOG("singleton <%s> : destroy", typeid(*this).name());
	*instance()	= nullptr;
}



template<class T, class CreaterT>
auto singleton<T, CreaterT>::get_instance() -> _return_type
{
	_return_type	retval	= *instance();
	if (!retval)
	{
		retval	= create_instance();
	}
	return	retval;
}



template<class T, class CreaterT>
auto singleton<T, CreaterT>::instance() -> _instance_type**
{
	static _instance_type*	s_instance	= nullptr;
	return	&s_instance;
}


AMTRS_NAMESPACE_END
#endif
