/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
