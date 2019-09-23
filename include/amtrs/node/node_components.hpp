/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__node__node_components__hpp
#define	__libamtrs__node__node_components__hpp
#include "nodefwd.hpp"
#include "component/action_component.hpp"
#include "component/color_component.hpp"
#include "component/director_component.hpp"
#include "component/hierarchy_component.hpp"
#include "component/input_component.hpp"
#include "component/name_component.hpp"
#include "component/renderer_component.hpp"
#include "component/scheduler_component.hpp"
#include "component/transform_component.hpp"
#include "component/user_data_component.hpp"
AMTRS_NAMESPACE_BEGIN


/*!
 * デフォルトのコンポーネント
 */
template<class T, class Traits>
struct	node_components
		: component::action_component		<basic_node<T, Traits>>
		, component::color_component		<basic_node<T, Traits>>
		, component::director_component		<basic_node<T, Traits>, typename Traits::director>
		, component::hierarchy_component	<basic_node<T, Traits>>
		, component::input_component		<basic_node<T, Traits>>
		, component::name_component			<std::string, basic_node<T, Traits>>
		, component::renderer_component		<Traits>
		, component::scheduler_component	<basic_node<T, Traits>, void(float)>
		, component::transform_component	<T, Traits>
		, component::user_data_component	<basic_node<T, Traits>, void*>
{
	using	traits_type					= Traits;
	using	components_type				= node_components<T, Traits>;
	using	node_type					= basic_node<T, traits_type>;

	using	action_type					= basic_action;

	using	director_component			= typename component::director_component<node_type, typename traits_type::director>;
	using	transform_component			= typename component::transform_component<T, Traits>;
	using	name_component				= typename component::name_component<std::string, basic_node<T, Traits>>;
	using	update_scheduler_component	= typename component::scheduler_component<node_type, void(float)>;
	using	hierarcy_component			= typename component::hierarchy_component<node_type>;
	using	director_type				= typename director_component::director_type;
	using	schede_dispatcher_type		= typename components_type::schede_dispatcher_type;

	using	layer						= typename components_type::layer;
	using	render_queue				= typename components_type::render_queue;
	using	on_child_event				= typename components_type::on_child_event;
	using	box_type					= typename components_type::box_type;
	using	position_type				= typename components_type::position_type;


};



AMTRS_NAMESPACE_END
#endif
