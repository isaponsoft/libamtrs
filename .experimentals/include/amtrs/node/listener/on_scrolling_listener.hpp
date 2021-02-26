/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__listener__on_scrolling_listener__hpp
#define	__libamtrs__node__listener__on_scrolling_listener__hpp

//! scroll_node<> の on_scroll() に対するリスナを設定できるようにします。
template<class SuperT>
class	on_scrolling_listener
		: public SuperT
{
public:
	using	on_scrolling_listener_type	= void(vec2<float>&);

	using	super_type	= SuperT;
	using	super_type::super_type;

	std::function<on_scroll_listener_type> set_scrolling_listener(std::function<on_scrolling_listener_type> _listener)
	{
		std::function<on_scroll_listener_type>	r(std::move(mListener));
		mListener	= std::move(_listener);
		return	r;
	}

protected:
	virtual void on_scrolling(vec2<float>& _scroll) noexcept override
	{
		super_type::on_scrolling(_scroll);
		if (mListener)
		{
			mListener(_scroll);
		}
	}


private:
	std::function<on_scrolling_listener_type>	mListener;
};


#endif
