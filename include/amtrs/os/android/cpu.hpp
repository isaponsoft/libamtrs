#include <cpu-features.h>
#include "def.hpp"
AMTRS_OS_ANDROID_NAMESPACE_BEGIN

// https://developer.android.com/ndk/guides/cpu-features
class	cpu_feature
{
public:
	static std::string get_cpu_family()
	{
		std::string	retval;
		switch (android_getCpuFamily())
		{
			case ANDROID_CPU_FAMILY_ARM:		retval	= "ANDROID_CPU_FAMILY_ARM"; break;
			case ANDROID_CPU_FAMILY_X86:		retval	= "ANDROID_CPU_FAMILY_X86"; break;
			case ANDROID_CPU_FAMILY_MIPS:		retval	= "ANDROID_CPU_FAMILY_MIPS"; break;
			case ANDROID_CPU_FAMILY_ARM64:		retval	= "ANDROID_CPU_FAMILY_ARM64"; break;
			case ANDROID_CPU_FAMILY_X86_64:		retval	= "ANDROID_CPU_FAMILY_X86_64"; break;
			case ANDROID_CPU_FAMILY_MIPS64:		retval	= "ANDROID_CPU_FAMILY_MIPS64"; break;
			default:							retval	= "Unkown"; break;
		}
		return	retval;
	}


	static std::string get_cpu_feature()
	{
		std::vector<std::string>	values;
		uint64_t					flags	= android_getCpuFeatures();
		switch (android_getCpuFamily())
		{
			case ANDROID_CPU_FAMILY_ARM:
				if (flags & ANDROID_CPU_ARM_FEATURE_VFPv2)			values.push_back("ANDROID_CPU_ARM_FEATURE_VFPv2");
				if (flags & ANDROID_CPU_ARM_FEATURE_ARMv7)			values.push_back("ANDROID_CPU_ARM_FEATURE_ARMv7");
				if (flags & ANDROID_CPU_ARM_FEATURE_VFPv3)			values.push_back("ANDROID_CPU_ARM_FEATURE_VFPv3");
				if (flags & ANDROID_CPU_ARM_FEATURE_VFP_D32)		values.push_back("ANDROID_CPU_ARM_FEATURE_VFP_D32");
				if (flags & ANDROID_CPU_ARM_FEATURE_NEON)			values.push_back("ANDROID_CPU_ARM_FEATURE_NEON");
				if (flags & ANDROID_CPU_ARM_FEATURE_VFP_FP16)		values.push_back("ANDROID_CPU_ARM_FEATURE_VFP_FP16");
				if (flags & ANDROID_CPU_ARM_FEATURE_VFP_FMA)		values.push_back("ANDROID_CPU_ARM_FEATURE_VFP_FMA");
				if (flags & ANDROID_CPU_ARM_FEATURE_NEON_FMA)		values.push_back("ANDROID_CPU_ARM_FEATURE_NEON_FMA");
				if (flags & ANDROID_CPU_ARM_FEATURE_IDIV_THUMB2)	values.push_back("ANDROID_CPU_ARM_FEATURE_IDIV_THUMB2");
				if (flags & ANDROID_CPU_ARM_FEATURE_iWMMXt)			values.push_back("ANDROID_CPU_ARM_FEATURE_iWMMXt");
				if (flags & ANDROID_CPU_ARM_FEATURE_LDREX_STREX)	values.push_back("ANDROID_CPU_ARM_FEATURE_LDREX_STREX");
				break;
			case ANDROID_CPU_FAMILY_X86:
				if (flags & ANDROID_CPU_X86_FEATURE_SSSE3)			values.push_back("ANDROID_CPU_X86_FEATURE_SSSE3");
				if (flags & ANDROID_CPU_X86_FEATURE_POPCNT)			values.push_back("ANDROID_CPU_X86_FEATURE_POPCNT");
				if (flags & ANDROID_CPU_X86_FEATURE_MOVBE)			values.push_back("ANDROID_CPU_X86_FEATURE_MOVBE");
				break;
			case ANDROID_CPU_FAMILY_MIPS:
				if (flags & ANDROID_CPU_MIPS_FEATURE_R6)			values.push_back("ANDROID_CPU_MIPS_FEATURE_R6");
				if (flags & ANDROID_CPU_MIPS_FEATURE_MSA)			values.push_back("ANDROID_CPU_MIPS_FEATURE_MSA");
				break;
			case ANDROID_CPU_FAMILY_ARM64:
				if (flags & ANDROID_CPU_ARM64_FEATURE_FP)			values.push_back("ANDROID_CPU_ARM64_FEATURE_FP");
				if (flags & ANDROID_CPU_ARM64_FEATURE_ASIMD)		values.push_back("ANDROID_CPU_ARM64_FEATURE_ASIMD");
				if (flags & ANDROID_CPU_ARM64_FEATURE_AES)			values.push_back("ANDROID_CPU_ARM64_FEATURE_AES");
				if (flags & ANDROID_CPU_ARM64_FEATURE_CRC32)		values.push_back("ANDROID_CPU_ARM64_FEATURE_CRC32");
				if (flags & ANDROID_CPU_ARM64_FEATURE_SHA1)			values.push_back("ANDROID_CPU_ARM64_FEATURE_SHA1");
				if (flags & ANDROID_CPU_ARM64_FEATURE_SHA2)			values.push_back("ANDROID_CPU_ARM64_FEATURE_SHA2");
				if (flags & ANDROID_CPU_ARM64_FEATURE_PMULL)		values.push_back("ANDROID_CPU_ARM64_FEATURE_PMULL");
				break;
			case ANDROID_CPU_FAMILY_X86_64:
				break;
			case ANDROID_CPU_FAMILY_MIPS64:
				break;
			default:
				break;
		}

		std::string	retval;
		for (auto& s : values)
		{
			if (!retval.empty()) retval += ",";
			retval += s;
		}
		return	retval;
	}
};



AMTRS_OS_ANDROID_NAMESPACE_END
