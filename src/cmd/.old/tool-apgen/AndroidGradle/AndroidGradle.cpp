/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <regex>
#include <unordered_map>
#include <amtrs/amtrs.hpp>
#include <amtrs/console.hpp>
#include <amtrs/filesystem.hpp>
#include <amtrs/string.hpp>
#include <amtrs/system.hpp>
#include <apgen.hpp>

using namespace apgen;
using namespace amtrs;
using		ll	= console::loglevel;
namespace	fs	= amtrs::filesystem;


class	AndroidGradleGenerator
		: public generator_base
{
public:
	AndroidGradleGenerator(option const& _opt)
	{
		using namespace amtrs;

		std::string	basedir	= (std::string)fs::parent_path(_opt.genFile);

		mAttributes["CMAKELISTS"]			= basedir + "/CMakeLists.txt";
		mAttributes["MAINACTIVITY"]			= ".MainActivity";
		mAttributes["ASSETS_SRCDIRS"]		= format<std::string>("[ '%s' ]", (basedir + "/asset").c_str());
		mAttributes["RESOURCES_SRCDIRS"]	= format<std::string>("[ '%s' ]", (basedir + "/res").c_str());
	}

	// 引数の整合性チェック
	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) override
	{
		if (_key == "GRADLE_URL")
		{
			mGradleUrl		= _value;
			return	true;
		}

		mAttributes[_key]	= _value;
		return	true;
	}

	virtual int execute(status const& _state, option const& _opt) override
	{
		if (mAttributes.find("PACKAGE") == mAttributes.end())
		{
			mAttributes["PACKAGE"]		= mAttributes["APPLICATIONID"];
		}

		store_gradle(mGradleUrl);


		fs::create_directory("src/main");


		generate_src_main_AndroidManifest_xml(_state);
		generate_build_gradle(_state);
		generate_gradle_properties(_state);
		generate_local_properties(_state);
		generate_proguard_rules(_state);
		generate_settings_gradle(_state);

		return	0;
	}

	template<class... Args>
	std::string fmt(const char* _fmt, Args&&... _args)
	{
		return	amtrs::format<std::string>(_fmt, std::forward<Args>(_args)...);
	}

	std::string path_escape(std::string const& _path, bool _escape = true)
	{
		return	fs::normalize_path(_path, _escape);
	}

	//! src/main/AndroidManifest.xml
	void generate_src_main_AndroidManifest_xml(status const& _state)
	{
		generate_with_template("src/main/AndroidManifest.xml", "Android.template.AndroidManifest.xml", _state, "AndroidManifest.xml");
	}

	//! build.gradle
	void generate_build_gradle(status const& _state)
	{
		generate_with_template("build.gradle", "Android.template.build.gradle", _state, "build.gradle");
	}

	//! gradle.properties
	void generate_gradle_properties(status const& _state)
	{
		generate_with_template("gradle.properties", "Android.template.gradle.properties", _state, "gradle.properties");
	}

	//! local.properties
	void generate_local_properties(status const& _state)
	{
		generate_with_template("local.properties", "Android.template.local.properties", _state, "local.properties");
	}

	//! proguard-rules.pro
	void generate_proguard_rules(status const& _state)
	{
		generate_with_template("proguard-rules.pro", "Android.template.proguard_rules.pro", _state, "proguard-rules.pro");

	}

	//! settings.gradle
	void generate_settings_gradle(status const& _state)
	{
		generate_with_template("settings.gradle", "Android.template.settings.gradle", _state, "settings.gradle");
	}

	void generate_with_template(std::string _target, std::string _filename, status const& _state, std::string _template)
	{
		std::string	templatename	= gAmtrsHome;
		templatename += "/apgen/android-gradle/";
		templatename += _template;
		templatename =  amtrs::filesystem::normalize_path(templatename);

		console::log(ll::ll_detail, "generate '%s' in '%s'.", _target.c_str(), templatename.c_str());

		if (!amtrs::filesystem::exists(templatename))
		{
			apgen::error(amtrs::format<std::string>("template \"%s\" not found.", templatename.c_str()));
			return;
		}

		std::string	txt;
		std::string	filename;
		if (mAttributes.find(_filename) != mAttributes.end())
		{
			std::string	fname	= mAttributes[_filename];
			if (!amtrs::filesystem::exists(fname))
			{
				apgen::error(amtrs::format<std::string>("\"%s\" not found.", fname.c_str()));
				return;
			}
			filename		= fname;
		}
		else
		{
			filename		= templatename;
		}
		txt		= amtrs::filesystem::file_get_contents<std::string>(filename);
		txt		= process(const_cast<status&>(_state), txt, filename);
		update(_target, txt);
	}


	void generate(std::string const& _filename, std::string_view _template)
	{
		amtrs::filesystem::create_directory(amtrs::filesystem::parent_path(_filename));

		auto	txt	= amtrs::regex_replace_callback<char>(std::string(_template), std::basic_regex<char>("##(.*)##"), [&](auto& m) -> std::string
					{
						auto	val	= std::string();
						auto	key	= std::string(m[1]);
						auto	it	= mAttributes.find(key);
						if (it != mAttributes.end())
						{
							val	= it->second;
						}
						return	val;
					});

		update(std::string(_filename), txt);
	}


	void store_gradle(std::string const& _url)
	{
		using namespace amtrs;

		if (filesystem::is_directory(".gradle"))
		{
			return;
		}

		std::string	saved;

		ssu::download_result	dr;
		try
		{
			dr	= ssu::download(_url, filesystem::current_path());
			if (!dr)
			{
				apgen::error(amtrs::format<std::string>("\"%s\" download failed.", _url.c_str()));
				return;
			}
		}
		catch (std::exception& e)
		{
			apgen::error(amtrs::format<std::string>("%s: \"%s\" download failed.", e.what(), _url.c_str()));
			throw	e;
		}
		console::log(console::loglevel::ll_detail, "download success: '%s'.", dr.savedname.c_str());

		ssu::extract(dr.savedname, filesystem::current_path()+"/.gradle", {
			.sourcename	= "gradle",
			.skiproot	= true
		});

#if	0
		auto			dir	= unzip(buff.data(), buff.size());
		dir	= std::string(trim((std::string_view)dir, "/"));

		#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
		auto			cmd	= format<std::string>("%s/bin/gradle.bat wrapper", dir.c_str());
		cmd	= amtrs::regex_replace_callback(cmd, std::regex("/"), [](auto&) { return "\\"; });
		#else
		auto			bin	= format<std::string>("%s/bin/gradle", dir.c_str());
		::chmod(bin.c_str(), S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
		auto			cmd	= format<std::string>("%s/bin/gradle wrapper", dir.c_str());
		#endif

		update("settings.gradle", std::string(""));
		report(cmd + " run.");
		if (auto r = amtrs::system::exec(cmd.c_str()); r)
		{
			report(cmd + " failed.");
			exit(r);
		}
		report(cmd + " success.");
#endif
	}

protected:
	std::map<std::string, std::string>	mAttributes;
	std::string							mGradleUrl		= "https://services.gradle.org/distributions/gradle-6.6-bin.zip";
};


apgen::autolink	_androidGradle({"android-gradle", [](auto& opt) -> generator_base*
{
	return	new AndroidGradleGenerator(opt);
}});

