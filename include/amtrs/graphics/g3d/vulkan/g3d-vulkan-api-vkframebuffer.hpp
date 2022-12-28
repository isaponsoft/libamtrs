/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__vframebuffer__hpp
#define	__libamtrs__g3d__vulkan__vframebuffer__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<>
struct	vkhandle_traits<VkFramebuffer>
{
	using	value_type	= VkFramebuffer;

	// No need destroy.
	void destroy(vkref<VkRenderPass> const& _renderpass, value_type _handle)
	{
		vkDestroyFramebuffer(std::get<VkDevice>(_renderpass), _handle, nullptr);
	}

	VkExtent2D						size;
	std::vector<vkref<VkImageView>>	views;
};


template<>
class	vkref<VkFramebuffer>
		: public vkref_impl<VkFramebuffer, VkRenderPass>
{
	using	super_type	= vkref_impl<VkFramebuffer, VkRenderPass>;
public:
	using	value_type	= typename super_type::value_type;
	using	super_type::super_type;
	using	super_type::operator =;
	template<class... Args> vkref& operator = (Args&&... _args) { return super_type::operator = (std::forward<Args>(_args)...); return *this; }

	vkref<VkImageView> const& image_view(int _index) const
	{
		return	traits().views[_index];
	}

	auto image_count() const noexcept -> size_t 
	{
		return	handle() ? traits().views.size() : 0;
	}

	// ==============================================================
	//! 指定するフォーマットにレンダリングを行うためのパスを作成します。
	// --------------------------------------------------------------
	static vkref<VkFramebuffer> create(VkExtent2D _size, vkref<VkRenderPass> const& _renderpass, std::initializer_list<vkref<VkImageView>> const& _view)
	{
		return	create(_size, _renderpass, &*_view.begin(), _view.size());
	}


	// ==============================================================
	//! 指定するフォーマットにレンダリングを行うためのパスを作成します。
	// --------------------------------------------------------------
	static vkref<VkFramebuffer> create(VkExtent2D _size, vkref<VkRenderPass> const& _renderpass, vkref<VkImageView> const* _view, size_t _viewcount)
	{
		if (vklasterr) { return {}; }
		std::vector<VkImageView>	attachments;
		attachments.resize(_viewcount);
		std::copy_n(_view, _viewcount, attachments.begin());

		VkFramebufferCreateInfo	ci{0};
		ci.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		ci.renderPass			= _renderpass;
		ci.attachmentCount		= static_cast<uint32_t>(attachments.size());
		ci.pAttachments			= attachments.data();
		ci.width				= _size.width;
		ci.height				= _size.height;
		ci.layers				= 1;

		VkFramebuffer			handle	= VK_NULL_HANDLE;
		vkref<VkFramebuffer>	retval;
		_AMTRS_VKINVOKE(vkCreateFramebuffer(std::get<VkDevice>(_renderpass), &ci, nullptr, &handle));

		retval					= vkref<VkFramebuffer>(super_type::_make(handle, _renderpass));
		retval.traits().size	= _size;
		retval.traits().views	= std::vector<vkref<VkImageView>>(_viewcount);
		std::copy_n(_view, _viewcount, retval.traits().views.begin());
		return	retval;
	}

	VkExtent2D size() const noexcept
	{
		return	traits().size;
	}
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
