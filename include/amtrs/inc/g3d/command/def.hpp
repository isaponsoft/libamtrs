#ifndef __libamtrs__g3d__command__def__hpp
#define __libamtrs__g3d__command__def__hpp
#include "../def.hpp"
#include "../../codes/graphics/primitive.hpp"
//Graphics { namespace Primitive
AMTRS_G3D_NAMESPACE_BEGIN

class	command
		: public cry::Graphics::Primitive::Base
{
public:
	using	device_type		= cry::Graphics::Device;
	using	renderer_type	= cry::Graphics::Shader::Render;
	using	texture_type	= cry::Graphics::Texture;

	using	rgba_type		= rgba<float>;
	using	color_type		= typename rgba_type::value_type;


protected:
	//! レンダリング前に行う準備を記述します。
	//! 例えばテクスチャのロードなどが該当します。
	virtual void on_precommand(device_type& _renderer) {}

	//! レンダリング処理を記述します。
	virtual void on_render(renderer_type& _renderer) = 0;

	void setup(device_type& _device)
	{
		Base::Ready(&_device);
	}


private:
	virtual CsResult Render(renderer_type* _renderer) override final
	{
		on_render(*_renderer);
		return	CSR_OK;
	}

	virtual CsResult Ready(device_type* _device) override final
	{
		on_precommand(*_device);
		return	CSR_OK;
	}

};


AMTRS_G3D_NAMESPACE_END
#endif
