#ifndef	__passion__collection__regex__hpp
#define	__passion__collection__regex__hpp
#include <regex>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <amtrs/system.hpp>
#include "../psnvm.hpp"
PASSION_NAMESPACE_BEGIN
namespace collection {

// ****************************************************************************
//! regex class
// ----------------------------------------------------------------------------
class	vmnc_regex
		: public vmobject::vmnative
{
public:
	using	_base_type	= vmobject::vmnative;
	static constexpr auto signature() { return make_constring("regex"); }


	class	match_result
			: public vmobject::vmnative
	{
	public:
		static constexpr auto signature() { return make_constring("regex::match_result"); }


		static std::size_t native_size(vmclass::argument_type* _argv, std::size_t _size)
		{
			return	sizeof(match_result);
		}

		static vmnative* native_new(void* _mem, void* _extra, vmclass::argument_type* _argv, std::size_t _size)
		{
			return	new (_mem) match_result();
		}


		static void native_functions(vmclass_manager* _vm, vmclass* _clazz)
		{
			// マッチした長さ
			_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "size", [](passion_context& _context, match_result* _thiz) -> vmint_t
			{
				return	_thiz->mMatch.size();
			}));
		}

	private:

		std::smatch		mMatch;
		friend	struct vmclass;
		friend	class vmnc_regex;
	};




	static std::size_t native_size(vmclass::argument_type* _argv, std::size_t _size)
	{
		return	sizeof(vmnc_regex);
	}

	static vmnative* native_new(void* _mem, void* _extra, vmclass::argument_type* _argv, std::size_t _size)
	{
		//auto			str		= std::get<ref<vmobject>>(_argv[0])->native<vmstrobj>();
		vmnc_regex*		thiz	= new (_mem) vmnc_regex();
		//thiz->mRegex	= std::regex(str->c_str());
		return	thiz;
	}


	static void native_functions(vmclass_manager* _vm, vmclass* _clazz)
	{
		sRegexMatchClazz	= _vm->get_class(match_result::signature().c_str());

		// パターンの設定
		_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "patturn", [](passion_context& _context, vmnc_regex* _thiz, vmstrobj* _s) -> vmint_t
		{
			_thiz->mRegex	= std::regex(_s->c_str());
			return	0;
		}));

		// 置換
		_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "replace", [](passion_context& _context, vmnc_regex* _thiz, vmstrobj* _s, vmstrobj* _rep) -> std::string
		{
			return	std::regex_replace(std::string(_s->begin(), _s->end()), _thiz->mRegex, _rep->c_str());
		}));

		// マッチ
		_clazz->add_function(native::create_vfunc(_vm, _clazz->name(), "match", [](passion_context& _context, vmnc_regex* _thiz, vmstrobj* _s) -> ref<match_result>
		{
			auto		retval	= vmobject::create(sRegexMatchClazz);
			auto		res		= retval->base()->native<match_result>();
			std::string	s		= std::string(_s->begin(), _s->end());
			std::regex_match(s, res->mMatch, _thiz->mRegex);
			auto		r		= ref<match_result>(res);
			return	r;
		}));
	}


private:
	static inline	vmclass*	sRegexMatchClazz;

	std::regex		mRegex;


	friend	struct vmclass;
};



inline void register_regex(vmclass_manager* _vm)
{
	_vm->create_native_class<vmnc_regex::match_result>();
	_vm->create_native_class<vmnc_regex>();
}


}


namespace native {

template<>
struct	__native_func_argument<collection::vmnc_regex*>
{
	collection::vmnc_regex* get(collection::vmnc_regex* _o)
	{
		return	_o;
	}

	collection::vmnc_regex* get(vmobject* _o)
	{
		return	_o->native<collection::vmnc_regex>();
	}

	collection::vmnc_regex* get(vmvariant& _v)
	{
		auto	o	= std::get<ref<vmobj>>(_v);
		return	get(o->base());
	}
};


template<>
struct	__native_func_argument<collection::vmnc_regex::match_result*>
{
	collection::vmnc_regex::match_result* get(collection::vmnc_regex::match_result* _o)
	{
		return	_o;
	}

	collection::vmnc_regex::match_result* get(vmobject* _o)
	{
		return	_o->native<collection::vmnc_regex::match_result>();
	}

	collection::vmnc_regex::match_result* get(vmvariant& _v)
	{
		auto	o	= std::get<ref<vmobj>>(_v);
		return	get(o->base());
	}
};

}
 PASSION_NAMESPACE_END
#endif
