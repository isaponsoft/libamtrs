/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__application__application_linux__hpp
#define	__libamtrs__application__application_linux__hpp
AMTRS_NAMESPACE_BEGIN


inline filesystem::path application::cache_dir() const
{
	return	".";
}


inline filesystem::path application::documents_dir() const
{
	return	{};
}



inline filesystem::path application::files_dir(bool _external) const
{
	return	".";
}


inline void application::foreground()
{
}


inline int application::run(int _argc, char* _args[])
{
	this->on_created(_argc, _args);
	while (!this->is_quit())
	{
		this->on_update();
	}
	this->on_event(amtrs::application::system_event::stop);
	return	0;
}



AMTRS_NAMESPACE_END
#endif
