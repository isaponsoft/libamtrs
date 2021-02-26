/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/media/audio.hpp>
AMTRS_MEDIA_NAMESPACE_BEGIN


class	audio_
		: public audio
{
public:
	virtual void pickup(std::function<void(std::string)> _listener) override
	{
	}


	virtual void play(std::string const& _identify) override
	{
	}

};


ref<audio> audio::create()
{
	return	new audio_();
}

AMTRS_MEDIA_NAMESPACE_END
