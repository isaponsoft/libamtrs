/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__purchase__hpp
#define	__libschedule__save__purchase__hpp
#include <ctime>
#include <vector>
#include <amtrs/amtrs.hpp>
#include <amtrs/string/@>
#include "time.hpp"
namespace save {



// ******************************************************************
//! 課金情報
// ------------------------------------------------------------------
//!
// ==================================================================
struct	purchasedata
{
	//! 永続アイテム
	struct	eternal
	{
		std::string	product;
		timetype	date;
	};

	struct	eternalsdata
			: std::vector<eternal>

	{};


	eternalsdata	eternals;


	//! 購入済みか調べる
	bool is_purchased(std::string const& _product) const noexcept;
};


bool deserialize(purchasedata& _data, std::istream& _in);
bool deserialize(purchasedata::eternalsdata& _data, std::istream& _in);
bool deserialize(purchasedata::eternal& _data, std::istream& _in);


bool serialize(std::ostream& _out, purchasedata const& _data);
bool serialize(std::ostream& _out, purchasedata::eternalsdata const& _data);
bool serialize(std::ostream& _out, purchasedata::eternal const& _data);



}
#endif
