/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/scriptutil.hpp>
#include <amtrs/system.hpp>
#include <stdlib.h>
#include <iostream>

#include "icpp.hpp"

using namespace amtrs;
using namespace amtrs::filesystem;
using namespace amtrs::system;

#define	VSSETUP_EXE_URL	"https://download.visualstudio.microsoft.com/download/pr/5c555b0d-fffd-45a2-9929-4a5bb59479a4/7a92444b6df4ad128e5eaf1e787fa6fe0fe8e86ba039e37b98b8be6bcc0ea878/vs_Community.exe"

 bool setup_visualstudio(bool _exec, config& cnf)
{
	std::cerr << "Download and install the compiler." << std::endl;
	ssu::download(VSSETUP_EXE_URL, normalize_path(cnf.cacheDir + "/vs_Community.exe"));
	std::cerr << "Depending on the environment, you may have to wait about an hour." << std::endl;
	std::string	out;
	std::string	err;
	if (ssu::exec(out, err, "vs_Community.exe --quiet --wait --norestart --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended"))
	{
		std::cerr << out << err;
		std::cerr << "Visual Studio Install error" << std::endl;
		
		return	false;
	}
	return	true;
}
