/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__gui__window__hpp
#define	__libamtrs__gui__window__hpp
#include "../graphics/geometory/size.hpp"
#include "../memory/ref_object.hpp"
#include "../string/sstring.hpp"
#include "../util/callback.hpp"
AMTRS_NAMESPACE_BEGIN

class	window : public ref_object
{
public:
	class	listener;

	using	window_size_type	= graph::size2<unsigned int>;
	using	style_type			= unsigned int;
	using	flags_type			= unsigned int;
	using	repaint_callback	= callback<void()>;

	static constexpr unsigned int	use_default	= ~0;

	static constexpr style_type		style_default	= 0;

	struct	createparams
	{
		sstring				title;
		style_type			style		= style_default;
		flags_type			flags		= 0;
		window_size_type	size		= {use_default, use_default};
		class listener*		listener	= nullptr;
	};

	class	listener
	{
	public:
		using	window_size_type	= window::window_size_type;

		virtual void on_created(window* _sender) {}
		virtual void on_destroyed(window* _sender) {}
		virtual void on_size(window* _sender, window_size_type _size, bool _fullscreen) {}
		virtual void on_activate(window* _sender) {}
		virtual void on_deactivate(window* _sender) {}
	};

	static ref<window> create(createparams& _params, ref<window> _parent = {});

	auto repaint(repaint_callback _callback) -> bool;

	auto size() const noexcept -> window_size_type;
};

AMTRS_NAMESPACE_END
#endif
