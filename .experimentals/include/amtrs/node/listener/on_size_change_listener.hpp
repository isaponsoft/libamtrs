/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__listener__on_size_change_listener__hpp
#define	__libamtrs__node__listener__on_size_change_listener__hpp


template<class SuperT>
class	on_size_changed_listener
		: public SuperT
{
public:
	using	super_type	= SuperT;
	using	size_type	= typename super_type::size_type;
	using	super_type::super_type;

	std::function<void(size_type&)> set_size_changed_listener(std::function<void(size_type&)> _listener)
	{
		std::function<void(size_type&)>	r(std::move(mListener));
		mListener	= std::move(_listener);
		return	r;
	}

protected:
	virtual void on_size_change(size_type& _size) override
	{
		super_type::on_size_change(_size);
		if (mListener)
		{
			mListener(_size);
		}
	}


private:
	std::function<void(size_type&)>	mListener;
};


template<class SuperT>
class	on_size_changing_listener
		: public SuperT
{
public:
	using	super_type	= SuperT;
	using	size_type	= typename super_type::size_type;
	using	super_type::super_type;

	std::function<void(size_type&)> set_size_changing_listener(std::function<void(size_type&)> _listener)
	{
		std::function<void(size_type&)>	r(std::move(mListener));
		mListener	= std::move(_listener);
		return	r;
	}

protected:
	virtual void on_size_change(size_type& _size) override
	{
		if (mListener)
		{
			mListener(_size);
		}
		super_type::on_size_change(_size);
	}


private:
	std::function<void(size_type&)>	mListener;
};

#endif
