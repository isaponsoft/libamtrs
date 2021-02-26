/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <amtrs/filesystem/@>
#include <apgen/xcode/pbxedit.hpp>
using namespace amtrs;

#define	__DEBUG_LOG(_txt)	if (gDebugMode) { _txt }

bool	gDebugMode	= false;


class	InfoPlistStrings
{
public:
	struct	Value
	{
		std::string	id;
		std::string	locale;
		std::string	file;
	};

	InfoPlistStrings(std::string* _prj)
		: mProject(_prj)
	{
		mFileId		= pbxroot(mProject).generateFileId("InfoPlistStrings");
		mResourceId	= pbxroot(mProject).generateFileId("InfoPlistStrings:Resource");
	}

	void set_region(std::string const& _reg)
	{
		mDefRegion	= _reg;
	}

	void add_localize(std::string const& _region, std::string const& _file, std::string const& _fileId = {})
	{
		std::string	fid	= _fileId;
		if (fid.empty())
		{
			fid	= pbxroot(mProject).generateFileId(_region);
		}

		__DEBUG_LOG(
			std::cerr << "Add localize file: Regeon=" << _region << ", File=" << _file << ", ID=" << fid << std::endl;
		)

		mPlists.push_back({
			std::move(fid),
			_region,
			_file
		});
	}


	void save()
	{
		auto	mainGroupCode	= pbxroot(mProject).mainGroupCode();
		auto	rootObjectCode	= pbxroot(mProject).rootObjectCode();

		__DEBUG_LOG(
			std::cerr << "Edit: " << mFileId << " /* InfoPlist.strings */" << std::endl;
		)

		pbxroot(mProject)
			.objects()
			.find_dict(mainGroupCode).value()
			.find_array("children").value()
			.push_back(format<std::string>("%s /* InfoPlist.strings */,\n", mFileId.c_str()))
		;

		__DEBUG_LOG(
			std::cerr << "Edit: " << mResourceId << " InfoPlist.strings in Resources */ = {isa = PBXBuildFile; fileRef = " << mFileId << " /* InfoPlist.strings */; }" << std::endl;
		)

		pbxroot(mProject)
			.objects()
			.find_section("PBXBuildFile").inner()
			.push_back(
				format<std::string>("%s /* InfoPlist.strings in Resources */ = {isa = PBXBuildFile; fileRef = %s /* InfoPlist.strings */; };\n", mResourceId.c_str(), mFileId.c_str())
			)
		;

		for (auto& loc : mPlists)
		{
			__DEBUG_LOG(
				std::cerr << "Edit: "
				<< loc.id << " /* " << loc.locale << " */ = {isa = PBXFileReference; lastKnownFileType = text.plist.strings; name = " << loc.locale << "; path = " << loc.file << "; sourceTree = \"<group>\"; };\n"
				<< std::endl;
			)

			pbxroot(mProject)
				.objects()
				.find_section("PBXFileReference").inner()
				.push_back(
					format<std::string>(
						"%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = text.plist.strings; name = %s; path = %s; sourceTree = \"<group>\"; };\n"
						, loc.id.c_str()
						, loc.locale.c_str()
						, loc.locale.c_str()
						, loc.file.c_str()
					)
				)
			;
		}

		// PBXVariantGroup が存在しないなら追加する
		if (!pbxroot(mProject).objects().find_section("PBXVariantGroup").good())
		{
			__DEBUG_LOG(
				std::cerr << "Edit: PBXVariantGroup"
				<< std::endl;
			)

			pbxroot(mProject)
				.objects()
				.push_back(format<std::string>(R"(
					/* Begin PBXVariantGroup section */
							%s /* InfoPlist.strings */ = {
								isa = PBXVariantGroup;
								children = ();
								name = InfoPlist.strings;
								sourceTree = "<group>";
							};
					/* End PBXVariantGroup section */
					)", mFileId.c_str())
				);
		}


		for (auto& loc : mPlists)
		{
			__DEBUG_LOG(
				std::cerr << "Edit: PBXVariantGroup : children : " << loc.id << " /* "<< loc.locale <<" */," << std::endl;
			)

			pbxroot(mProject)
				.objects()
				.find_section("PBXVariantGroup")
				.find_array("children").value()
				.push_back(format<std::string>("%s /* %s */,\n", loc.id.c_str(), loc.locale.c_str()))
			;
		}


		std::string	regions	= "";
		for (auto& loc : mPlists)
		{
			regions += loc.locale;
			regions += ", ";
		}
		__DEBUG_LOG(
			std::cerr << "Regions : " << regions << std::endl;
		)

		__DEBUG_LOG(
			std::cerr << "Edit: developmentRegion = " << mDefRegion << std::endl;
		)
		__DEBUG_LOG(
			std::cerr << "Edit: knownRegions = ( Base, " << regions << ");" << std::endl;
		)

		pbxroot(mProject)
			.objects()
			.find_dict(rootObjectCode).value()
			.push_back(
				format<std::string>("\t\tdevelopmentRegion = %s;\n", mDefRegion.c_str())
			)
			.push_back(
				format<std::string>("\t\tknownRegions = ( Base, %s);\n", regions.c_str())
			)
		;

		__DEBUG_LOG(
			std::cerr << "Edit: " << mResourceId << " /* InfoPlist.strings in Resources */," << std::endl;
		)

#if	0
		if (pbxroot(mProject)
			.objects()
			.find_section("PBXResourcesBuildPhase").empty())
		{
			std::cerr << "PBXResourcesBuildPhase" << std::endl;
		}

		pbxroot(mProject)
			.objects()
			.find_section("PBXResourcesBuildPhase")
			.find_array("files").value()
			.push_back(format<std::string>("%s /* InfoPlist.strings in Resources */,\n", mResourceId.c_str()))
		;
#endif
		__DEBUG_LOG(
			std::cerr << "Complite" << std::endl;
		)
	}

private:
	std::string*		mProject;
	std::string			mFileId;
	std::string			mResourceId;
	std::string			mDefRegion;
	std::list<Value>	mPlists;
};


/*
 * -InfoPlistStrings
 *		DevelopmentRegeon LocalePlist
 *		Regeon LocalePlist
 *		Regeon LocalePlist ...
 *
 */
int main(int _argc, char* _args[])
{
	auto		loader	= filesystem::fileloader::get_instance();
	loader->push_back(filesystem::stdvfs::get_instance());

	if (_argc <= 1)
	{
		std::cerr << "xcodeedit pbxproj-filename opt..." << std::endl;
		return	1;
	}

	std::string	pbxname	= _args[1];
	if (!loader->exists(pbxname))
	{
		std::cerr << pbxname << " no such file." << std::endl;
		return	1;
	}

	std::string	proj	= filesystem::file_get_contents<std::string>(pbxname);
	if (proj.empty())
	{
		std::cerr << pbxname << " is blunk." << std::endl;
		return	1;
	}

	std::string	outname	= pbxname;
	for (int i = 2; i < _argc;)
	{
		std::string_view	cmd(_args[i++]);

		if (cmd == "-d")
		{
			gDebugMode	= true;
			continue;
		}

		if (cmd == "-o")
		{
			if (i == _argc)
			{
				std::cerr << "-o has not output filename.";
				return	1;
			}
			outname	= _args[i++];
			__DEBUG_LOG(
				std::cerr << "Change output filename \"" << outname << "\"" << std::endl;
			)
			continue;
		}

		if (cmd == "-InfoPlistStrings")
		{
			if (i == _argc)
			{
				std::cerr << "-InfoPlistStrings has not default region.";
				return	1;
			}
			std::string	regn	= _args[i++];
			__DEBUG_LOG(
				std::cerr << "InfoPlistStrings development regeion \"" << regn << "\"" << std::endl;
			)

			if (i == _argc)
			{
				std::cerr << "-InfoPlistStrings has not localize directory.";
				return	1;
			}
			std::string	indir	= _args[i++];


			InfoPlistStrings	edit(&proj);
			edit.set_region(regn);

			__DEBUG_LOG(
				std::cerr << "InfoPlistStrings search *.lproj in \"" << indir << "\"" << std::endl;
			)

			bool	has	= false;
			for (auto e : std::filesystem::directory_iterator(indir))
			{
				std::smatch	m;
				std::string	file	= e.path().string();
				if (!std::regex_match(file, m, std::regex(".*/([a-z]+)\\.lproj")))
				{
					continue;
				}
				__DEBUG_LOG(
					std::cerr << "InfoPlistStrings such \"" << m[1].str() << "\"" << std::endl;
				)
				has	= true;
				edit.add_localize(m[1].str(), file);
			}
			if (!has)
			{
				std::cerr << "InfoPlistStrings nothing lproj" << std::endl;
				return	1;
			}
			else
			{
				std::cerr << "saving" << std::endl;
				edit.save();
				std::cerr << "saving - ok" << std::endl;
			}
			continue;
		}


		std::cerr << "xcodeedit unkown command " << cmd << std::endl;
		return	1;
	}


	if (outname == "-")
	{
		__DEBUG_LOG(
			std::cerr << "Output to stdout" << std::endl;
		)
		std::cout << proj << std::endl;
	}
	else
	{
		__DEBUG_LOG(
			std::cerr << "Output to \"" << outname << "\"." << std::endl;
		)
		std::ofstream	o(outname, std::ios::trunc);
		o.write(proj.data(), proj.size());
	}
	return	0;
}

