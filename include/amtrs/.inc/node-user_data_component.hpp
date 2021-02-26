/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__component__user_data_component__hpp
#define	__libamtrs__node__component__user_data_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {

template<class Node, class UserDataT>
class	user_data_component
{
public:
	using	user_data_type	= UserDataT;

	void set_user_data(user_data_type _ud) noexcept
	{
		mUserData = _ud;
	}

	user_data_type user_data() const noexcept
	{
		return	mUserData;
	}


private:
	user_data_type		mUserData	= nullptr;
};


} AMTRS_NAMESPACE_END
#endif
