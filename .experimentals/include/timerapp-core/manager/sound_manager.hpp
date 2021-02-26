/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__sound_manager__hpp
#define	__libscheduler__sound_manager__hpp
struct	sound_manager
{
	struct	playing_status
			: amtrs::ref_object
	{
		virtual bool is_stopped() = 0;
		virtual void stop() = 0;
	};


	static void initialize();
	static void clean();

	//! システム音声の再生
	static playing_status* play_sys(std::string const& _filename, int _priority = -1);

	//! アラーム音声の再生
	static playing_status* play_alarm(std::string const& _filename, int _priority = -1);

	static void stop_all();

private:
	static playing_status* play(std::string const& _filename, int _priority = -1);
};
#endif
