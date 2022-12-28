/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkexecuter__hpp
#define	__libamtrs__driver__g3d__vulkan__vkexecuter__hpp
#include <stack>
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	vkpipeline;
class	vkuniform;


struct	vkexecuter : commandexecuter
{
	ref<vkgraphics>					graphics;
	ref<commandlist>				lastlist;
	vkref<VkCommandBuffer>			cmdbuff;
	vkref<VkFramebuffer>			frame;
	vkref<VkRenderPass>				renderpass;

	std::deque<ref<ref_object>>		keeps;
	std::stack<renderstate>			rsstack;

	void initialize()
	{
		cmdbuff.clear();
		renderpass.clear();
		cmdbuff.clear();
		while (!rsstack.empty())
		{
			rsstack.pop();
		}
		keeps.clear();
	}


	virtual void exec(constants_update& _data) override
	{
		vkconstants::realize(_data.target, graphics->device())->update(cmdbuff, _data.source.data(), _data.source.size());
	}

	virtual void exec(vertexbuffer_update& _data) override
	{
		realizing<vkvertexbuffer>(_data.target, graphics->device(), _data.source.data(), _data.source.size());
	}

	virtual void exec(textureset_update& _data) override
	{
		if (_data.source.size() > 0)
		{
			auto	imgbuf	= vktexture::realize(_data.target, {0,0}, graphics->device(), _data.source.data(), _data.source.size());
			imgbuf->sync(cmdbuff, graphics);
		}
	}

	virtual void exec(shadermodule_update& _data) override
	{
		if (_data.source.size() > 0)
		{
			realizing<vkshadermodule>(_data.target, graphics->device())->update(_data.source.data(), _data.source.size());
		}
	}

	virtual void exec(draw_vertex& _data) override
	{
		vkvertexbuffer*	vtx	= realizing<vkvertexbuffer>(_data.target, graphics->device());
		VkDeviceSize	offsets[1]		= { _data.offset };
		VkBuffer		buffers[1]		= { vtx->mVertexBuff };
		vkCmdBindVertexBuffers(cmdbuff, 0, 1, buffers, offsets);
		vkCmdDraw(cmdbuff, _data.count, 1, 0, 0);
	}

	virtual void exec(viewport_scissor& _data) override
	{
		if (_data.flags & 0x01)
		{
			VkViewport	vp
			{
				/*.x		= */_data.viewport.x,
				/*.y		= */_data.viewport.y,
				/*.width	= */_data.viewport.width,
				/*.height	= */_data.viewport.height,
			};
			vkCmdSetViewport(cmdbuff, 0, 1, &vp);
		}
		if (_data.flags & 0x02)
		{
			VkRect2D	s
			{
				//.offset =
				{
					/*.x		= */_data.scissor.x,
					/*.y		= */_data.scissor.y
				},
				//.extent =
				{
					/*.width  = */static_cast<uint32_t>(_data.scissor.width),
					/*.height = */static_cast<uint32_t>(_data.scissor.height)
				},
			};
			vkCmdSetScissor(cmdbuff, 0, 1, &s);
		}
	}

	virtual void exec(renderstatus_push& _data) override
	{
		if (_data.state.empty())
		{
			rsstack.pop();
		}
		else
		{
			rsstack.push(std::move(_data.state));
		}
	}



	virtual void exec(renderpass_begin& _data) override
	{
		auto	frm		= vkframebuffer::realize(_data.target, graphics->device());
		frm->build(graphics->device(), graphics->swapchain(), graphics->next_swapchain());

		renderpass	= frm->renderpass();
		frame		= frm->framebuff();

		VkClearValue			cv[2]
		{
			{ _data.color.r, _data.color.g, _data.color.b, _data.color.a },
			{ /*.depthStencil = */{_data.depth, _data.stencil} }
		};
		VkRenderPassBeginInfo	rbi
		{
			/*.sType			= */VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			/*.pNext			= */nullptr,
			/*.renderPass		= */renderpass,
			/*.framebuffer		= */frame,
			//.renderArea 		= 
			{
				/*.offset = */{ 0, 0, },
				/*.extent = */frame.size()
			},
			/*.clearValueCount	=*/ 2,
			/*.pClearValues		=*/ cv
		};
		vkCmdBeginRenderPass(cmdbuff, &rbi, VK_SUBPASS_CONTENTS_INLINE);
	}


	virtual void exec(renderpass_end& _data) override
	{
		cmdbuff.end_renderpass();
	}

	virtual void exec(pipeline_change& _data) override
	{
		vklasterr.clear();
#if		AMTRS_DEBUG
		if (renderpass.empty())
		{
			AMTRS_WARN_LOG("vulkan execute pipeline change : renderpass is null.");
			return;
		}
		if (_data.pipe.rstate().empty() && rsstack.empty())
		{
			AMTRS_WARN_LOG("vulkan execute pipeline change : renderstate is empty.");
			return;
		}
		if (frame.empty() || !frame.image_count())
		{
			AMTRS_WARN_LOG("vulkan execute pipeline change : framebuffer is empty.");
			return;
		}
#endif

		auto	ppl		= realizing<vkpipeline>(_data.pipe, graphics->device(), graphics->pipeline_cache());
		auto	line	= ppl->pipeline(renderpass, frame.image_view(0).size(), _data.pipe.rstate().empty() ? &rsstack.top() : &_data.pipe.rstate());
		vkCmdBindPipeline(cmdbuff, VK_PIPELINE_BIND_POINT_GRAPHICS, line);
		keeps.push_back(line.handle());
	}


	virtual void exec(uniform_bind& _data) override
	{
		auto	u	= realizing<vkuniform>(_data.u, graphics->device());
		u->update(graphics->device());
		u->exec(cmdbuff);
	}


	virtual void exec(begin& _data) override
	{
		vklasterr.clear();
		cmdbuff.begin();
	}



	virtual void exec(end& _data) override
	{
		cmdbuff.end();
		graphics->submit(cmdbuff);
	}

	virtual void exec(present& _data) override
	{
		vklasterr.clear();
		graphics->present();
//		graphics->next_image();
	}
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif
