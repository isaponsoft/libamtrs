/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__node__component__action_component__hpp
#define	__libamtrs__node__component__action_component__hpp
#include <list>
#include "../action.hpp"
AMTRS_NAMESPACE_BEGIN
class	basic_action;		// Prototype. Not use.
AMTRS_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN namespace component {


template<class NodeT>
class	action_component
{
public:
	using	node_type	= NodeT;
	using	action_type	= basic_action;


	//! 実行中のアクションの数を取得する
	std::size_t running_action_count() const noexcept { return mActionCount; }


	//! アクションの実行を開始する
	action_type* run_action(action_type* _action);


	//! アクションをすべてクリアする
	void clear_action();


	//! 指定するアクションをクリアする
	bool remove_action(action_type* _action);


private:
	      node_type* owner()       noexcept { return static_cast<      node_type*>(this); }
	const node_type* owner() const noexcept { return static_cast<const node_type*>(this); }

	std::size_t					mActionCount	= 0;
};


template<class NodeT>
auto action_component<NodeT>::run_action(action_type* _action) -> action_type*
{
	ref<action_type>	act	= _action;

	_action->set_target(owner());
	++mActionCount;
	owner()->template schedule<action_type*>(_action, [this, act](float _delta)
	{
		act->update(_delta);
		if (!act->is_running())
		{
			remove_action(act);
		}
	});
	return	_action;

}


template<class NodeT>
void action_component<NodeT>::clear_action()
{
	mActionCount -= owner()->template unschedule_for_type<action_type*>();
}


template<class NodeT>
bool action_component<NodeT>::remove_action(action_type* _action)
{
	auto	rs	= owner()->unschedule(_action);
	if (!rs)
	{
		return	false;
	}
	mActionCount -= rs;
	return	true;
}


} AMTRS_NAMESPACE_END
#endif
