#ifndef	__passion__compiler__node_stmnt_extends__hpp
#define	__passion__compiler__node_stmnt_extends__hpp
#include "node.hpp"
PASSION_NAMESPACE_BEGIN


template<class Compiler>
struct	decl_extends
{
	using	compiler_type	= Compiler;

	enum	phase_type
	{
		start,
		access,
		type,			// 型
		separator,
		end,
		pass
	};


	vmtypename identify() const noexcept
	{
		return	mIdentify;
	}

	const std::vector<lexref<classname<Compiler>>>& types() const noexcept { return mBaseClasses; }


	template<class Lex, class C, class T>
	lex_result<decl_extends> lex(Lex& _lex, phase_type _phase, C& _cmplr, T& _token)
	{
		switch (_phase)
		{
			case phase_type::start :
				mIdentify	= _lex.parent()->identify();
			case phase_type::access :
			{
				if (_token == "private")
				{
					mNextAccess	= attribute_flag::private_attribute;
					return	phase_type::type;
				}
				if (_token == "protected")
				{
					mNextAccess	= attribute_flag::protected_attribute;
					return	phase_type::type;
				}
				if (_token == "public")
				{
					mNextAccess	= attribute_flag::public_attribute;
					return	phase_type::type;
				}
			}
			case phase_type::type :
			{
				auto	type	= _lex.template create<classname<Compiler>>();
				mBaseClasses.push_back(type);
				return	{ phase_type::separator, type->lex(_cmplr, _token) };
			}

			case phase_type::separator :
			{
				if (_token == ",")
				{
					mNextAccess	= attribute_flag::private_attribute;
					return	phase_type::access;
				}
				return	phase_type::pass;
			}

			default:
			{
				return	phase_type::pass;
			}
		}
	}



	void declere(compiler_type& _cmplr, vmclass* _clazz)
	{
		for (auto& base : mBaseClasses)
		{
			_clazz->add_base_class(_cmplr.find_class(base->type()));
		}
	}

private:
	vmtypename									mIdentify;
	attribute_flag								mNextAccess	= attribute_flag::private_attribute;
	std::vector<lexref<classname<Compiler>>>	mBaseClasses;
};


PASSION_NAMESPACE_END
#endif
