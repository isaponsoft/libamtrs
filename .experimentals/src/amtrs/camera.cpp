/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/camera.hpp>
AMTRS_NAMESPACE_BEGIN

auto camera::choise(int _min, int _max, facing_types _facing, format_types _format) -> std::pair<ref<camera>, format>
{
	std::pair<ref<camera>, format>	ret;
	ret.second.size	= {0, 0};
	enumlate([&](auto _camera)
	{
		_camera->formats([&](auto _fmt)
		{
			if ((_fmt.type == _format)
			 && (_fmt.size.width >= _min && _fmt.size.width <= _max)
			 && (_fmt.size.width > ret.second.size.width || ret.second.size.width > _max))
			{
				ret.first	= _camera;
				ret.second	= _fmt;
				return	true;
			}
			return	false;
		});
		return	true;
	});

	return	ret;
}

AMTRS_NAMESPACE_END
