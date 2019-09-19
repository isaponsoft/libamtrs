#ifndef	__libamtrs__node__node_trats__hpp
#define	__libamtrs__node__node_trats__hpp
#include "../../amtrs.hpp"
#include "../../g3d.hpp"
#include "component/scheduler_component.hpp"
AMTRS_NAMESPACE_BEGIN

template<class Traits>
class	basic_node;


struct	node_trits
{
	struct	director : ref_object
	{
		using	node_type		= basic_node<node_trits>;
	};


	using	draw_argument	= g3d::device*;

	using	render_argument	= g3d::device*;
};


// ============================================================================
//! 描画キューは渡されたパラメータから g3d::device を取り出すための関数。
// ----------------------------------------------------------------------------
//! もし、node_trits をカスタマイズし、 draw_argument を変更した場合は
//! get_g3ddevice() をオーバーロードすることで対応できます。
// ----------------------------------------------------------------------------
template<class T>
class	g3ddevice
{
public:
	g3d::device* operator () (T _t) const noexcept
	{
		return	_t;
	}
};

template<>
class	g3ddevice<g3d::device*>
{
public:
	g3d::device* operator () (g3d::device* _t) const noexcept
	{
		return	_t;
	}
};


template<class T>
g3d::device* get_g3ddevice(T _value) { return g3ddevice<T>{}(_value); }



AMTRS_NAMESPACE_END
#endif
