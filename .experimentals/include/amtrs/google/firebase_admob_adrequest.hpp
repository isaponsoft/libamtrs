/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__google__firebase_admob_adrequest__hpp
#define	__libamtrs__google__firebase_admob_adrequest__hpp
#include "firebase_admob.hpp"
AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN

struct	admob::adrequest
		: public ref_object
		, public fb::admob::AdRequest
{
	using	request_type	= fb::admob::AdRequest;

	adrequest()
		: request_type{}
	{
		request_type::gender								= fb::admob::kGenderUnknown;
		request_type::tagged_for_child_directed_treatment	= fb::admob::kChildDirectedTreatmentStateTagged;

		set_test_devices({
			"2077ef9a63d2b398840261c8221a0c9b",
			"098fe087d987c9a878965454a65654d7"
		});

		set_keywords({
			"default"
		});

		set_keywords({
			{"the_name_of_an_extra", "the_value_for_that_extra"},
		});
	}

	void set_birthday(int _year, int _month, int _day)
	{
		birthday_year	= _year;
		birthday_month	= _month;
		birthday_day	= _day;
	}

	void set_keywords(std::initializer_list<std::pair<std::string_view, std::string_view>> _extra)
	{
		for (auto& ex : _extra)
		{
			mExtraPair.push_back(std::pair<std::string, std::string>{ex.first, ex.second});
		}
		for (auto& ex : mExtraPair)
		{
			mExrtaPairPtr.push_back({ex.first.c_str(), ex.second.c_str()});
		}
		extras_count			= (decltype(extras_count))mExrtaPairPtr.size();
		extras 					= mExrtaPairPtr.data();
	}



	void set_keywords(std::initializer_list<std::string_view> _keywords)
	{
		mKeywords.set(_keywords);
		keyword_count			= (decltype(keyword_count))mKeywords.size();
		keywords				= mKeywords.data();
	}


	void set_test_devices(std::initializer_list<std::string_view> _devices)
	{
		mTestDevices.set(_devices);
		test_device_id_count	= (decltype(test_device_id_count))mTestDevices.size();
		test_device_ids			= mTestDevices.data();
	}

protected:
	struct	string_list
	{
		std::vector<std::string>	valueList;
		std::vector<const char*>	pointerList;

		void set(std::initializer_list<std::string_view> _strings)
		{
			valueList	.assign(std::begin(_strings), std::end(_strings));
			pointerList	.reserve(valueList.size());
			for (auto& s : valueList)
			{
				pointerList.push_back(s.c_str());
			}
		}

		std::size_t size() const noexcept { return pointerList.size(); }
		const char** data() noexcept { return pointerList.data(); }
	};

	string_list												mKeywords;
	string_list												mTestDevices;

	std::vector<std::pair<std::string, std::string>>		mExtraPair;
	std::vector<fb::admob::KeyValuePair>					mExrtaPairPtr;
};


AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#endif
