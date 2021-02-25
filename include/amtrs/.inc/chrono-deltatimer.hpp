/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__deltatimer__hpp
#define	__libamtrs__chrono__deltatimer__hpp
AMTRS_CHRONO_NAMESPACE_BEGIN

template<class Duration = std::chrono::milliseconds, class Clock = std::chrono::high_resolution_clock>
class	deltatimer
{
public:
	using	clock		= Clock;
	using	duration	= Duration;
	using	time_point	= std::chrono::time_point<clock, duration>;

	deltatimer() = default;
	deltatimer(const deltatimer&) = default;
	deltatimer(deltatimer&&) = default;
	deltatimer& operator = (const deltatimer&) = default;
	deltatimer& operator = (deltatimer&&) = default;

	static deltatimer now()
	{
		deltatimer	d;
		d.reset();
		return	d;
	}

	void reset()
	{
		mPrevious	= std::chrono::time_point_cast<duration>(clock::now());
	}

	duration operator () ()
	{
		auto	now		= std::chrono::time_point_cast<duration>(clock::now());
		auto	dlt		= now - mPrevious;
		mPrevious	= now;
		return	dlt;
	}

protected:
	time_point	mPrevious;
};


AMTRS_CHRONO_NAMESPACE_END
#endif
