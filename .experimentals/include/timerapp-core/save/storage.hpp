/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__storage__hpp
#define	__libschedule__save__storage__hpp
#include <amtrs/amtrs.hpp>
#include "purchase.hpp"
#include "setting_energy.hpp"
#include "setting_event.hpp"
#include "setting_looks.hpp"
#include "setting_sound.hpp"
#include "schedule.hpp"
namespace save {

using namespace amtrs;

struct	settingdata
{
	static constexpr uint32_t	alarm_disable_bits		= 1<<0;

	uint32_t					flags	= 0;

	setting_energy				energy;
	setting_event				event;
	setting_looks				looks;
	setting_sound				sound;


	bool alarm_disable() const noexcept { return flags & alarm_disable_bits; }
	void alarm_disable(bool _on) noexcept { if (_on) { flags |= alarm_disable_bits;  } else { flags &= ~alarm_disable_bits; } }
};


bool deserialize(settingdata& _data, std::istream& _in);
bool serialize(std::ostream& _out, settingdata const& _data);



// ******************************************************************
//! スケジュールデータ
// ------------------------------------------------------------------
//!
// ==================================================================
struct	storage : ref_object
{
	std::string		filename;

	settingdata		setting;
	schedulelist	schedules;		//!< スケジュールの一覧
	purchasedata	purchase;		//!< 課金情報

	static storage* current() { return _instance().get(); }
	static void set_current(ref<storage> _s) { _instance() = std::move(_s); }


	storage();
	storage(std::string _filename);
	~storage();

	std::string create_schedule_identify();

	bool load();
	bool save();
	void dump(std::string const& _tag);

private:
	static inline ref<storage>& _instance()
	{
		static	ref<storage>	_;
		return	_;
	}
};




template<class T, class Stream>
bool deserialize(T& _data, Stream _in)
{
	std::istream*	v=&_in;
	return	deserialize(_data, *v);
}

}
#endif
