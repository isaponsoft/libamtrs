#ifndef	__libamtrs__node__clipping_node__hpp
#define	__libamtrs__node__clipping_node__hpp
#include "fill_node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! クリッピングマスクを作成します。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
template<class BaseT>
class	clipping_node
		: public fill_node<BaseT>
{
public:
	using	_base_type	= fill_node<BaseT>;
	using	node_type	= typename _base_type::node_type;

	using	_base_type::_base_type;

	void set_stencil_id(uint8_t _value)
	{
		mStencilCode	= _value;
	}

protected:
	virtual node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		//! 自分の範囲外のプレスイベントは子ノードに伝えないようにする。
		if (auto mouse = _e->event()->get_mouse_event(); mouse && mouse->state == input_event::press)
		{
			auto	mat		= _base_type::world_matrix().inverse();
			if (!_base_type::size().is_inner(mouse->template get_point<float>() * mat))
			{
				return	nullptr;
			}
		}
		return	_base_type::on_input(_e);
	}


	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		// ステンシルバッファをクリア後
		// fill_node の描画の際にステンシルバッファに 1 を書き込むように指定する。
		_queue.draw([this](auto g)
		{
			glClearStencil(0);
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, mStencilCode, ~0);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		//	glColorMask(0, 0, 0, 0);
		//	glDepthMask(0);
		});

		// fill_node で矩形をステンシルバッファへ描画
		_base_type::draw(_queue);

		// 子ノードに対してステンシルテストを行う
		_queue.draw([this](auto g)
		{
			glStencilFunc(GL_EQUAL, mStencilCode, ~0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		//	glColorMask(1,1,1,1);
		//	glDepthMask(1);
		});
		_base_type::_base_type::on_render(_queue);

		// ステンシルテストを終了する
		_queue.draw([](auto g)
		{
			glDisable(GL_STENCIL_TEST);
		});
	}

	uint8_t	mStencilCode	= 1;
};

AMTRS_NAMESPACE_END
#endif
