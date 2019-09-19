#ifndef	__passion__vmobject__hpp
#define	__passion__vmobject__hpp
#include "def.hpp"

struct	passion_object
{
	passion_vtable*	vtbl;


#ifdef	__cplusplus
	void add_ref() const;
	void dec_ref() const;
	size_t use_count() const noexcept;
	void down_ref();
	psn::vmobject* base() const noexcept;

	template<class T> T* native() noexcept { return reinterpret_cast<T*>(this + 1); }
#endif

};
static_assert(sizeof(passion_object) == sizeof(void*));


PASSION_NAMESPACE_BEGIN


using	vmobj	= passion_object;

//! オブジェクトがインスタンス化されたときの状態
struct	vmobject
{
	void add_ref() const { ++mRef; }
	void dec_ref() const { if (!--mRef) { delete this; } }
	auto use_count() const noexcept { return mRef.load(); }
	void down_ref() { --mRef; }


	struct	vmnative
	{
		virtual ~vmnative() = default;
		vmnative*	next	= nullptr;


		vmobj* obj() noexcept { return reinterpret_cast<vmobj*>(this)-1; }
		const vmobj* obj() const noexcept { return reinterpret_cast<const vmobj*>(this)-1; }
		void add_ref() const { obj()->add_ref(); }
		void dec_ref() const { obj()->dec_ref(); }
		size_t use_count() const noexcept { return obj()->use_count(); }
		void down_ref() { obj()->down_ref();}
	};


	virtual ~vmobject()
	{
		auto	ntv	= mNative;
		while (ntv)
		{
			auto	dest	= ntv;
			ntv	= ntv->next;
			dest->~vmnative();
		}
	}

	static std::size_t top_offset() noexcept
	{
		return	reinterpret_cast<std::size_t>(&static_cast<vmobject*>(nullptr)->mObj);
	}

	      vmobj* top()       noexcept { return reinterpret_cast<      vmobj*>(&mObj); }
	const vmobj* top() const noexcept { return reinterpret_cast<const vmobj*>(&mObj); }
	      void* member()       noexcept { return top(); }
	const void* member() const noexcept { return top(); }

	static ref<vmobj> create(vmclass* _clazz, vmvariant* _args = nullptr, std::size_t _size = 0);

	template<class T>
	T* native()
	{
		vmnative*	ntv	= mNative;
		while (ntv)
		{
			if (T* ret = dynamic_cast<T*>(ntv); ret)
			{
				return	ret;
			}
			ntv	= ntv->next;
		}
		return	nullptr;
	}

private:
	mutable std::atomic<std::size_t>	mRef	= 0;
	vmnative*							mNative	= nullptr;
	vmobj								mObj;
};


PASSION_NAMESPACE_END

inline void passion_object::add_ref() const { base()->add_ref(); }
inline void passion_object::dec_ref() const { base()->dec_ref(); }
inline size_t passion_object::use_count() const noexcept { return base()->use_count(); }
inline void passion_object::down_ref() { base()->down_ref(); }
inline psn::vmobject* passion_object::base() const noexcept {  return reinterpret_cast<psn::vmobject*>((std::intmax_t)this - vtbl->base_offset); }



#endif
