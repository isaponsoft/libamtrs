#ifndef	__libamtrs__sprite_button_node__hpp
#define	__libamtrs__sprite_button_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN


template<class NodeT>
class	sprite_button_node
		: public button_node<sprite_node<NodeT>>
{
	using	base_type	= button_node<sprite_node<NodeT>>;

public:
	//! テクスチャは上下で２パターンにする。
	sprite_button_node(g3d::texture* _texture)
	{
		set_texture(_texture);
	}

	void set_texture(g3d::texture* _texture)
	{
		auto	ts	= _texture->size();
		mRelease	= new g3d::sub_texture(_texture, {0, 0          }, {ts.width, ts.height/2});
		mPress		= new g3d::sub_texture(_texture, {0, ts.height/2}, {ts.width, ts.height/2});
		this->base_type::set_texture(mRelease, true);
	}

protected:
	// ========================================================================
	//! ボタンの押下状態が変化したときに呼ばれる。
	// ------------------------------------------------------------------------
	//! 表示するグラフィックの変更のみを行うこと。
	// ------------------------------------------------------------------------
	virtual void on_change_press_state(bool _press) override
	{
		this->base_type::set_texture(_press ? mPress : mRelease, false);
	}


	ref<g3d::texture>	mRelease;
	ref<g3d::texture>	mPress;
};


AMTRS_NAMESPACE_END
#endif
