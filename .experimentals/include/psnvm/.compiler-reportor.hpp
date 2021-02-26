/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__compiler__reportor__hpp
#define	__psnvm__compiler__reportor__hpp
#include <list>
#include ".compiler-codereader.hpp"
#include ".error.hpp"
PASSION_NAMESPACE_BEGIN

class	reportor
{
public:
	struct	error
	{
		errorcode	code;				//!< エラーコード
		vmstring	filename;			//!< エラーファイル名
		int			line;				//!< エラー行
		vmstring	hints[2];			//!< エラーのヒント

		error(errorcode _e, vmstring _fname, int _line)
			: code(_e)
			, filename(std::move(_fname))
			, line(_line)
		{}

		error(errorcode _e, codereader const& _code, vmstring _hints1 = {}, vmstring _hints2 = {})
			: code(_e)
			, filename(_code.filename())
			, line(_code.line())
		{
			hints[0] = _hints1;
			hints[1] = _hints2;
		}

		error(errorcode _e, codereader const& _code, codereader::positiondata _point, vmstring _hints1 = {}, vmstring _hints2 = {})
			: code(_e)
			, filename(_code.filename())
			, line(_point.line)
		{
			hints[0] = _hints1;
			hints[1] = _hints2;
		}

		vmstring whats() const;
	};

	void push(error _e) { mErrors.push_back(std::move(_e)); }

	bool empty() const noexcept { return mErrors.empty(); }

	auto begin() const noexcept { return mErrors.begin(); }
	auto end() const noexcept { return mErrors.end(); }


protected:
	std::list<error>	mErrors;
};



PASSION_NAMESPACE_END
#endif
