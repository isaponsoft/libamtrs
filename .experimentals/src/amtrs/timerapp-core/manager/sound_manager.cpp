/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/media/audio.hpp>
#include <amtrs/sound/@>
#include <amtrs/sound/pcm_ogg.hpp>
#include <amtrs/filesystem/@>
#include <timerapp-core/manager/sound_manager.hpp>
#include <timerapp-core/save/storage.hpp>
using namespace amtrs;

static constexpr std::size_t	player_max	= 12;


struct	player_data
		: sound_manager::playing_status
{
	ref<sound_player>	player;
	int					priority	= 0;
	sound_stream_type	type		= sound_stream_type::stream_default;

	bool is_stopped() override
	{
		return	player->is_stopped();
	}

	void set_notification_callback(std::function<void(amtrs::sound_player::message)>&& _callback)
	{
		player->set_notification_callback(std::move(_callback));
	}

	void stop() override
	{
		player->stop();
	}
};

static player_data* get_player(int _priority, sound_stream_type _type);


static	ref<sound_device>				gDevice;
static	std::vector<ref<player_data>>	gPlayerList;



void sound_manager::initialize()
{
	gDevice			= sound_device::create();
	gPlayerList.resize(player_max);

	sound_format	f;
	f.channels			= 1;
	f.bitsParSamples	= 16;
	f.samplesParSecond	= 44100;
	for (auto& p : gPlayerList)
	{
		auto	pd	= new player_data();
		pd->player	= gDevice->create_player(f);
		p			= pd;
	}
}


void sound_manager::clean()
{
	gDevice.clear();
	gPlayerList.clear();
}


auto sound_manager::play_sys(std::string const& _filename, int _priority) -> playing_status*
{
	if (save::storage::current()->setting.sound.systemSound)
	{
		return	play(_filename, _priority);
	}
	return	nullptr;
}


auto sound_manager::play_alarm(std::string const& _filename, int _priority) -> playing_status*
{
	return	play(_filename, _priority);
}


auto sound_manager::play(std::string const& _filename, int _priority) -> playing_status*
{
	if (_filename.empty())
	{
		return	nullptr;
	}

	auto			type	= sound_stream_type::stream_default;
	player_data*	data	= get_player(_priority, type);
	if (!data)
	{
		// 空いているチャンネルがない
		AMTRS_DEBUG_LOG("Nothing free sound channel.");
		return	nullptr;
	}
	auto		fl			= filesystem::fileloader::get_instance();
	if (fl->exists(_filename))
	{
		auto	fin	= fl->open(_filename);
		auto	ogg	= pcm_ogg::create(fin);
		auto	snd	= ogg->read_all();

		data->player	= gDevice->create_player(ogg->format(), type);

		data->player->set_buffer(gDevice->create_buffer(ogg->format(), snd.data(), snd.size()));
		data->player->play();
		data->priority	= _priority;

		AMTRS_DEBUG_LOG("Play(%s)", _filename.c_str());
		return	data;
	}
	else
	{
		media::audio::create()->play(_filename);
		return	nullptr;
	}
}


void sound_manager::stop_all()
{
	for (auto& p : gPlayerList)
	{
		p->stop();
	}
}



//! 一番低いっプライオリティのプレイヤーを探す
player_data* get_player(int _priority, sound_stream_type _type)
{
	sound_format	f;
	f.channels			= 1;
	f.bitsParSamples	= 16;
	f.samplesParSecond	= 44100;
	for (auto& pd : gPlayerList)
	{
		// 停止中なら無条件
		if (pd->player->is_stopped() && (pd->type == _type))
		{
			return	pd;
		}
	}
	for (auto& pd : gPlayerList)
	{
		// 停止中なら無条件
		if (pd->player->is_stopped())
		{
			pd->player	= nullptr;
			pd->player	= gDevice->create_player(f, _type);
			pd->type	= _type;
			return	pd;
		}
	}

	// プライオリティが最も低いプレイヤーを探す
	player_data*	candidate	= gPlayerList[0];
	for (auto& pd : gPlayerList)
	{
		if (pd->priority < candidate->priority)
		{
			candidate	= pd;
		}
	}
	if (candidate->priority < _priority)
	{
		if (candidate->type != _type)
		{
			candidate->player	= nullptr;
			candidate->player	= gDevice->create_player(f, _type);
			candidate->type		= _type;
		}
		return	candidate;
	}
	return	nullptr;
}
