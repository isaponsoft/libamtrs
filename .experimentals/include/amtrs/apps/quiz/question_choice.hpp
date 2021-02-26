/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__apps__quiz__question_choice__hpp
#define	__libamtrs__apps__quiz__question_choice__hpp
#include <random>
#include <vector>
#include "namespace.hpp"
AMTRS_APP_QUIZ_MESPACE_BEGIN

// Prototype.
template<class CharT>
struct	q_choice_traits;


template<class CharT, class Traits = q_choice_traits<CharT>>
struct	basic_q_choice
{
	using	char_type	= typename Traits::char_type;
	using	string_type	= typename Traits::string_type;
	template<class E>
	using	array_type	= typename Traits::template array_type<E>;

	struct	choice
	{
		string_type	text;
		int			score	= 0;
	};


	string_type				question;			//!< 問題文
	array_type<choice>		choices;			//!< 選択項目


	//! 選択肢の並びをシャッフルします。
	//! answerもシャッフルに合わせて再設定されます。
	void shuffle();
};




template<class CharT>
struct	q_choice_traits
{
	using	char_type	= CharT;
	using	string_type	= std::basic_string<char_type>;

	template<class E>
	using	array_type	= std::vector<E>;
};



template<class CharT, class Traits>
void basic_q_choice<CharT, Traits>::shuffle()
{
	std::random_device					rd;
	std::mt19937 						e(rd());
	std::shuffle(choices.begin(), choices.end(), e);
}


AMTRS_APP_QUIZ_MESPACE_END
#endif
