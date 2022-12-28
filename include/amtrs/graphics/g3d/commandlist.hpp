/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__commandlist__hpp
#define	__libamtrs__graphics__g3d__commandlist__hpp
#include "commandexecuter.hpp"
#include "constants.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


class	commandlist
		: public ref_object
{
	struct	subcommand
	{
		ref<commandlist>	cmd;
	};
public:
	static constexpr size_t		allocate_size	= 2048;
	static constexpr size_t		align_size		= sizeof(void*) * 2;

	struct	bufferstate
	{
		int		usingsize	= 0;
	};

	using	commandbuff			= shared_buffer<char, bufferstate>;

	commandlist()
	{
		create_buffer();
	}

	~commandlist()
	{
		clear();
	}

	//! 記録されているコマンドを実行します。
	//! 通常、デバイスドライバから呼ばれます。
	void execute(commandexecuter& _exec);

	//! 記録されているコマンドをクリアします。
	void clear();


	void sub(ref<commandlist> _cmd);

	template<class T>
	ref<T> sub();

	void update(constants _target, shared_buffer<void> _data);

	template<class T>
	void update(constants _target, T const& _data);

	void update(vertexbuffer _target, shared_buffer<void> _data);

	void update(textureset _target, shared_buffer<void> _data);

	void update(shadermodule _target, shared_buffer<void> _data);

	void pipeline(class pipeline _pipeline);

	void renderstate_push(class renderstate _state);
	void renderstate_pop();

	//! 現在のパイプラインに定数データをバインドします。
	void bind(uint32_t _index, constants _uniform);

	//! 現在のパイプラインにテクスチャとサンプラをバインドします。
	void bind(uint32_t _index, textureset _resource, imagesampler _sampler);

	void bind(uniformset _uniform);

	void draw(vertexbuffer _target, uint32_t _offset, uint32_t _count);

	//! ビューポートとシザーを設定します。
	//! nullptrを渡した場合は設定済みの値をそのまま使用します。
	void viewport_scissor(viewport const* _viewport, scissor const* _scissor);

	void renderpass_begin(framebuffer _target, rgba<float> _clearColor, float _depth = 1, float _stencil = 0);

	void renderpass_end();

	//! コマンドの記録の開始を行います。
	void begin();

	//! コマンドの記録の終了し、コマンドの処理を行います。
	void end();

	//! 画面へ反映さえます。
	void present();

private:
	template<class T>
	T* regist(T _data);

	template<class T>
	size_t align()
	{
		return	(sizeof(std::type_index) + (sizeof(T) + align_size - 1) / align_size * align_size);
	}

	commandbuff* create_buffer()
	{
		bufferstate	bs{0};
		bs.usingsize	= 0;
		mBuffer.push_back(commandbuff(allocate_size, bs));
		return	&mBuffer.back();
	}

	std::deque<commandbuff>		mBuffer;

	friend	cmdlist_pipeline;

};



inline void commandlist::sub(ref<commandlist> _cmd)
{
	if (!_cmd.empty())
	{
		regist<subcommand>({
			std::move(_cmd),
		});
	}
}

template<class T>
inline ref<T> commandlist::sub()
{
	ref<T>	retval	= new T();
	sub(retval);
	return	retval;
}

inline void commandlist::update(constants _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::constants_update>({
		std::move(_target),
		std::move(_data)
	});
}

template<class T>
inline void commandlist::update(constants _target, T const& _data)
{
	update(std::move(_target), {&_data, 1});
}


inline void commandlist::update(vertexbuffer _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::vertexbuffer_update>({
		std::move(_target),
		std::move(_data)
	});
}


inline void commandlist::update(textureset _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::textureset_update>({
		std::move(_target),
		std::move(_data)
	});
}


inline void commandlist::update(shadermodule _target, shared_buffer<void> _data)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _target is empty");
	}
	if (_data.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::update(_target, _data) : _data is empty");
	}
	#endif
	regist<commandexecuter::shadermodule_update>({
		std::move(_target),
		std::move(_data)
	});
}


inline void commandlist::viewport_scissor(viewport const* _viewport, scissor const* _scissor)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (!_viewport && !_scissor)
	{
		AMTRS_DEBUG_LOG("commandlist::viewport_scissor(_viewport, _scissor) : _viewport and _scissor is null");
	}
	#endif
	regist<commandexecuter::viewport_scissor>({
		(_viewport ? 0x01U : 0x00U) | (_scissor ? 0x02U : 0x00U),
		_viewport ? *_viewport : viewport{},
		_scissor  ? *_scissor  : scissor{}
	});
}


inline void commandlist::renderstate_push(class renderstate _state)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_state.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::renderstate_push(_state) : _state is empty");
	}
	#endif
	regist<commandexecuter::renderstatus_push>({
		std::move(_state)
	});
}


inline void commandlist::renderstate_pop()
{
	regist<commandexecuter::renderstatus_push>({
		renderstate()
	});
}

inline void commandlist::pipeline(class pipeline _pipeline)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_pipeline.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::pipeline(_pipeline) : _pipeline is empty");
	}
	#endif
	regist<commandexecuter::pipeline_change>({
		std::move(_pipeline)
	});
}


inline void commandlist::bind(uniformset _uniform)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_uniform.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::bind(_uniform) : _uniform is empty");
	}
	#endif
	regist<commandexecuter::uniform_bind>({
		std::move(_uniform),
	});
}

inline void commandlist::draw(vertexbuffer _target, uint32_t _offset, uint32_t _count)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::draw(_target) : _target is empty");
	}
	#endif
	regist<commandexecuter::draw_vertex>({
		std::move(_target),
		_offset,
		_count
	});
}

inline void commandlist::renderpass_begin(framebuffer _target, rgba<float> _clearColor, float _depth, float _stencil)
{
	#if	AMTRS_DEBUG_LOG_USE
	if (_target.empty())
	{
		AMTRS_DEBUG_LOG("commandlist::renderpass_begin(_target) : _target is empty");
	}
	#endif
	regist<commandexecuter::renderpass_begin>({
		std::move(_target),
		_clearColor,
		_depth,
		_stencil
	});
}

inline void commandlist::renderpass_end()
{
	regist<commandexecuter::renderpass_end>({
	});
}

inline void commandlist::begin()
{
	regist<commandexecuter::begin>({
	});
}

inline void commandlist::end()
{
	regist<commandexecuter::end>({
	});
}

inline void commandlist::present()
{
	regist<commandexecuter::present>({
	});
}


template<class T>
T* commandlist::regist(T _data)
{
	auto*	current		= &mBuffer.back();
	int		capacity	= static_cast<int>(current->size()) - current->header().usingsize;
	if (capacity < (sizeof(std::type_index) + sizeof(T)))
	{
		current	= create_buffer();
	}
	char*	buff		= current->data() + current->header().usingsize;
	new (buff) std::type_index(typeid(T));
	buff += sizeof(std::type_index);
	new (buff) T(std::move(_data));
	current->header().usingsize	+= static_cast<int>(align<T>());
	return	reinterpret_cast<T*>(buff);
}

AMTRS_G3D_NAMESPACE_END
#endif
