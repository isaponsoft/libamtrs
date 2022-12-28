/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__libamtrs__console__hpp
#define	__libamtrs__libamtrs__console__hpp
#include "../memory/simplearray.hpp"
#include "../string/sstring.hpp"
#include "terminal.hpp"
AMTRS_CONSOLE_NAMESPACE_BEGIN

// コンソール読み取り
class	editline
		: public terminal
{
public:
	editline() = default;
	editline(editline const&) = default;
	AMTRS_API editline(io::fdesc _output, io::fdesc _input);
	AMTRS_API editline& operator = (editline const&) = default;
	AMTRS_API void prompt(sstring _ps);			// プロンプトを変更し、次回更新時から表示されます
	AMTRS_API bool readline(sstring& _dest);		// 入力された行を読み取ります
	AMTRS_API bool add_history(sstring const& _txt);
	simplearray<sstring> const& history() const noexcept { return mHistory; }
	AMTRS_API void remove_history(int _num);

	AMTRS_API int edit_position() const noexcept;
	AMTRS_API int move_edit_position_size(int pos, int _vec) const noexcept;
	AMTRS_API int set_edit_position(int pos);
	AMTRS_API int char_count(int pos);
	AMTRS_API void reset();
protected:
	void find_history(int _vec);
	void file_find();
	void position_reset(int _x, bool _clear);
	void prompt();
	void save_position();
	bool update(sstring _txt, int _pos = -1);

private:
	sstring					mPrompt;
	int						mPromptSize		= 0;
	int						mEditPosition	= 0;
	int						mPositionX;
	int						mDefPositionX;
	int						mHistorySelect;
	int						mInputedSize;		// 実際に入力した長さ
	sstring					mBuff;
	simplearray<sstring>	mHistory;
	// マルチバイト文字の更新用
	char					mMBPendingBuff[8];
	int						mMBPendingSize	= 0;
};


AMTRS_CONSOLE_NAMESPACE_END
#endif
