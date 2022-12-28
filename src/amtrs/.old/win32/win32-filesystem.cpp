/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/filesystem.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

bool create_symlink(char const* _real, char const* _symlink)
{
	return	CreateSymbolicLinkA(_symlink, _real, SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE)
			? true
			: false;
}

bool create_symlink(path const& _real, path const& _symlink)
{
	return	CreateSymbolicLinkW(_symlink.native().c_str(), _real.native().c_str(), SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE)
			? true
			: false;
}


path readlink(path const& _pathname)
{
	wchar_t	buf[MAX_PATH+1]	;

	auto	pn	= _pathname.native();
	HANDLE	f	= CreateFileW(pn.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	DWORD	len	= GetFinalPathNameByHandleW(f, buf, MAX_PATH, 0);
	CloseHandle(f);
	buf[len]	= 0;

	wwildcard	m[1];
	if (wcmatch(m, L"*([A-Z[a-z]:+)", buf))
	{
		return	path(m[0].match);
	}
	return	path(buf);
}



AMTRS_FILESYSTEM_NAMESPACE_END
