/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__vmfunction__hpp
#define	__passion__vmfunction__hpp
#include "def.hpp"
#include "types.hpp"
PASSION_NAMESPACE_BEGIN



struct	vmfunction
		: public ref_object
{
	vmfunction(amtrs_string_view _classname, amtrs_string_view _name, amtrs_string_view _result, std::initializer_list<vmstring> _arguments)
		: vmfunction(std::move(_classname), std::move(_name), std::move(_result), _arguments.begin(), _arguments.end())
	{}

	template<class It>
	vmfunction(amtrs_string_view _classname, amtrs_string_view _name, amtrs_string_view _result, It _argFirst, It _argLast)
	{
		auto								argSize	= std::distance(_argFirst, _argLast);
		std::vector<amtrs_string_view>	params;
		params.reserve(argSize);
		for (auto& a : make_bucket(_argFirst, _argLast))
		{
			params.push_back(std::string_view(a));
		}

		mFunctionInfo	= passion_function_create(_result, _classname, _name, params.data(), argSize);
	}

	~vmfunction()
	{
		if (mFunctionInfo)
		{
			passion_function_destroy(mFunctionInfo);
		}
	}

	vmstring type() const
	{
		vmstring		args;
		for (std::size_t i = 0; i < parameter_size(); ++i)
		{
			if (i > 0)
			{
				args += ",";
			}
			args += parameter(i);
		}
		return	vmstring(result_type()) + vmstring("(") + args + ")";
	}

	void add_attribute(uint32_t _attr) noexcept	// passion_function_attributes
	{
		mFunctionInfo->attributes |= _attr;
	}

	bool has_attributes(uint32_t _attr) const noexcept
	{
		return	mFunctionInfo->attributes & _attr;
	}

	std::string_view name() const noexcept { return mFunctionInfo->function_name; }
	std::string_view identify() const noexcept { return mFunctionInfo->symbol; }
	std::string_view signature() const noexcept { return mFunctionInfo->signature; }
	std::string_view result_type() const noexcept { return mFunctionInfo->return_type; }
	std::string_view parameter(std::size_t _i) const noexcept { return std::string_view(mFunctionInfo->arguments[_i]); }
	std::size_t parameter_size() const noexcept { return mFunctionInfo->argument_size; }

	bool is_constractor() const noexcept { return mFunctionInfo->attributes & (uint32_t)attribute_flag::constractor_attribute; }
	bool is_destractor() const noexcept { return mFunctionInfo->attributes & (uint32_t)attribute_flag::destractor_attribute; }
	bool is_static() const noexcept { return mFunctionInfo->attributes & (uint32_t)attribute_flag::static_attribute; }
	bool is_virtual() const noexcept { return mFunctionInfo->attributes & (uint32_t)attribute_flag::virtual_attribute; }
	bool is_variadic() const noexcept { return mFunctionInfo->attributes & (uint32_t)attribute_flag::varidic_attribute; }
	bool is_thisable() const noexcept { return !is_static(); }

	//! オーバーライド可能な関数か調べる。
	bool can_override(vmfunction* _f) const noexcept
	{
		if ((std::string_view)mFunctionInfo->return_type   == (std::string_view)_f->mFunctionInfo->return_type
		 && (std::string_view)mFunctionInfo->signature     == (std::string_view)_f->mFunctionInfo->signature
		 && (std::string_view)mFunctionInfo->function_name == (std::string_view)_f->mFunctionInfo->function_name)
		{
			return	true;
		}
		return	false;
	}

	passion_function* function() noexcept
	{
		return	mFunctionInfo;
	}

protected:
	passion_function*				mFunctionInfo	= nullptr;

	friend struct vmclass;
};


PASSION_NAMESPACE_END
#endif
