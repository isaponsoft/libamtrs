/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__LIBAMRS__UTIL__CMD__PSNVM__OPTS__HXX
#define	__LIBAMRS__UTIL__CMD__PSNVM__OPTS__HXX
AMTRS_CMD_PSNVM_NAMESPACE_BEGIN

struct	options
{
	struct	sourcefile
	{
		sstring		filename;
		sstring		code;
	};

	sstring					cmd;
	simplearray<sourcefile>	sources;

	int parse(int _argc, char** _argv);
};



AMTRS_CMD_PSNVM_NAMESPACE_END
#endif
