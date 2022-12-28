/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__uniform_layout__hpp
#define	__libamtrs__graphics__g3d__uniform_layout__hpp
#include "../../memory/shared_buffer.hpp"
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


/*
 * いくつかの共有データをまとめたものを扱う。
 */
class	uniform_layout
{
public:
	struct	layout
	{
		shader_stage				stage		= shader_stage::none_stage;
		format_types				type		= format_types::unknown_value;
		uint32_t					location	= 0;
		shared_buffer<attribute>	attributes;

		layout() = default;
		layout(layout const&) = default;
		layout(layout&&) = default;
		layout& operator = (layout const&) = default;
		layout& operator = (layout&&) = default;

		layout(shader_stage _stage, uint32_t _location, shared_buffer<attribute> _attr);
		layout(shader_stage _stage, uint32_t _location, std::initializer_list<attribute> _attr) : layout(_stage, _location, shared_buffer<attribute>(_attr)) {}
		layout(uint32_t _location, shared_buffer<attribute> _attr) : layout(shader_stage::none_stage, _location, std::move(_attr)) {}
		layout(uint32_t _location, std::initializer_list<attribute> _attr) : layout(shader_stage::none_stage, _location, shared_buffer<attribute>(_attr)) {}
	};

	uniform_layout() = default;
	uniform_layout(uniform_layout const&) = default;
	uniform_layout(uniform_layout&&) = default;
	uniform_layout& operator = (uniform_layout const&) = default;
	uniform_layout& operator = (uniform_layout&&) = default;

	uniform_layout(shared_buffer<layout> _bindings)
		: mBindings(std::move(_bindings))
	{}

	uniform_layout(std::initializer_list<layout> _bindings)
		: mBindings(_bindings.begin(), _bindings.end())
	{}

	auto size() const noexcept { return mBindings.size(); }
	auto layouts() const noexcept -> shared_buffer<layout> const& { return mBindings; }
	auto empty() const noexcept { return size() == 0; }
	void stage(shader_stage _stage) { for (auto& l : mBindings) { l.stage = _stage; } }

private:
	shared_buffer<layout>		mBindings;
};


AMTRS_G3D_NAMESPACE_END
#endif
