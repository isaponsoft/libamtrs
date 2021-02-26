/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkfunc__hpp
#define	__libamtrs__driver__g3d__vulkan__vkfunc__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN

inline VkStencilOp to_vkval(renderstate::stensil_operators _v)
{
	return	_v == renderstate::stencil_op_keep			? VK_STENCIL_OP_KEEP 
		:	_v == renderstate::stencil_op_zero			? VK_STENCIL_OP_ZERO  
		:	_v == renderstate::stencil_op_replace		? VK_STENCIL_OP_REPLACE 
		:	_v == renderstate::stencil_op_invert		? VK_STENCIL_OP_INVERT 
		:	_v == renderstate::stencil_op_inc_and_clamp	? VK_STENCIL_OP_INCREMENT_AND_CLAMP 
		:	_v == renderstate::stencil_op_inc_and_wrap	? VK_STENCIL_OP_INCREMENT_AND_WRAP 
		:	_v == renderstate::stencil_op_dec_and_clamp	? VK_STENCIL_OP_DECREMENT_AND_CLAMP 
		:	_v == renderstate::stencil_op_dec_and_wrap	? VK_STENCIL_OP_DECREMENT_AND_WRAP 
		:	VK_STENCIL_OP_KEEP; 
}

inline VkCompareOp to_vkval(renderstate::compare_modes _v)
{
	return	  _v == renderstate::compare_never			? VK_COMPARE_OP_NEVER 
			: _v == renderstate::compare_always			? VK_COMPARE_OP_ALWAYS
			: _v == renderstate::compare_equal			? VK_COMPARE_OP_EQUAL
			: _v == renderstate::compare_not_equal		? VK_COMPARE_OP_NOT_EQUAL
			: _v == renderstate::compare_less			? VK_COMPARE_OP_LESS
			: _v == renderstate::compare_less_or_equal	? VK_COMPARE_OP_LESS_OR_EQUAL
			: _v == renderstate::compare_grater			? VK_COMPARE_OP_GREATER
			: _v == renderstate::compare_grater_or_equal? VK_COMPARE_OP_GREATER_OR_EQUAL
			: VK_COMPARE_OP_ALWAYS;
}



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
