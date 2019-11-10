#ifndef	__VM_HPP
#define	__VM_HPP
#include "passion.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <string_view>
#include <amtrs/amtrs.hpp>
#include <amtrs/variant/@>
#include "err/error.hpp"

#ifdef	_MSC_VER
#pragma	warning(push)
#pragma	warning(disable:4100)	// Unuse function parameter.
#endif

#include "vm/vm.hpp"
#include "vm/vmaryobj.hpp"
#include "vm/vmarygcobj.hpp"
#include "vm/vmbytecode.hpp"
#include "vm/vmclass.hpp"
#include "vm/vmfunc_static.hpp"
#include "vm/vmfunction.hpp"
#include "vm/vmfunction.hpp"
#include "vm/vmnative.hpp"
#include "vm/vmstrobj.hpp"
#include "vm/vmcode.hpp"

#include "token/brunch_point.hpp"
#include "token/escape.hpp"
#include "token/source_location.hpp"
#include "token/token.hpp"
#include "token/token_reader.hpp"
#include "token/token_buffer.hpp"

#include "asm/assembler.hpp"

#include "cmplr/compiler.hpp"

#ifdef	_MSC_VER
#pragma	warning(pop)
#endif

#endif
