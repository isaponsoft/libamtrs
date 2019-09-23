#include <iostream>
#include <iomanip>
#include <amtrs/filesystem.hpp>
#include <amtrs/io.hpp>
#include <amtrs/string.hpp>
#include <passion/psnvm.hpp>
#include <passion/collection/console.hpp>
#include <passion/collection/filesystem.hpp>
#include <passion/collection/process.hpp>
#include <passion/collection/regex.hpp>
#include <passion/collection/vm.hpp>
using namespace PASSION_NAMESPACE;


static bool read_and_compile(vmclass_manager* _vm, const char* _sourcefile_path);
static int run(vmclass_manager* _vm, const vmstring& _entryName, int _argn, char* _argv[]);

namespace {
	bool	sListupMode;
	bool	sBuildOnly;
	bool	sDumpClassHierarchy;
}

// ============================================================================
//!	@brief	エントリポイント
// ----------------------------------------------------------------------------
int main(int _argn, char* _argv[])
{
	int		r;
	auto	cmd_argn	= _argn;
	auto	cmd_argv	= _argv;
	if (cmd_argn == 1)
	{
		std::cerr << "Has not source file name" << std::endl ;
		return	1;
	}


	vmstring	entryName	= "main";


	// コマンド解析。
	while (cmd_argn > 1)
	{
		++cmd_argv;
		--cmd_argn;

		std::string	command = *cmd_argv;
		if (amtrs::starts_with(std::string_view(command), std::string_view("-")))
		{
			// option
			if (command == "-entry")
			{
				++cmd_argv;
				--cmd_argn;
				entryName	= *cmd_argv;
			}
			else if (command == "--function-list")
			{
				sListupMode	= true;
			}
			else if (command == "--build-only")
			{
				sBuildOnly	= true;
			}
			else if (command == "--dump-class-hierarchy")
			{
				sDumpClassHierarchy	= true;
			}
			else
			{
				std::cerr << command << " : unkown command line option." << std::endl;
				return	1;
			}
		}
		else
		{
			break;
		}
	}


	{
		// VMを用意し、組み込み関数を登録する。
		vmclass_manager			psnvm;
		psn::collection::register_console(&psnvm, _argv[0]);
		psn::collection::register_filesystem(&psnvm);
		psn::collection::register_process(&psnvm);
		psn::collection::register_regex(&psnvm);
		psn::collection::register_vm(&psnvm);

		// ファイルを読み込んでコンパイルする。
		if (!read_and_compile(&psnvm, *cmd_argv))
		{
			return	1;
		}

		if (sListupMode)
		{
			for (auto c : psnvm.classes())
			{
				for (auto f : c.second->function_list())
				{
					std::cout << std::string_view(f->symbol) << std::endl;
				}
			}
			return	0;
		}
		if (sDumpClassHierarchy)
		{
			for (auto it : psnvm.classes())
			{
				vmclass*	cls	= it.second;
				std::cout << "Class " << cls->name() << std::endl;
				auto*		tbl			= cls->variable_info_table();
				for (auto& vi : make_bucket(tbl->variables, tbl->variables + tbl->variable_size))
				{
					auto	decl	= cls->get_variable_info(vmstring(amtrs_string_to_string_view(vi.name)));
					std::cout << "  +" << std::hex << std::setw(2) << (vi.get_offset()) << ": ";
					if (decl.second)
					{
						std::cout << decl.second->type << " ";
					}
					std::cout << vi.name << std::endl;
				}

				for (auto& vtbl : cls->vtables())
				{
					std::cout << vtbl.vtbl << " : ";

					std::cout << "vtable" << std::endl;
					for (size_t i = 0; i < vtbl.size; ++i)
					{
	//					std::cout << "  +" << std::hex << std::setw(2) << (sizeof(void*) * i) << ": ";
	//					std::cout << (*(vtbl.vtbl))[i]->identify() << std::endl;
					}
				}
				std::cout << std::endl;
			}
		}
		if (sBuildOnly)
		{
			return	0;
		}

		// 実行する。
		r	= run(&psnvm, entryName, cmd_argn, cmd_argv);
	}

#ifdef	DEBUG
//	AMTRS_TRACE_LOG("object count = %zd", vmobject::sObjectCount);
#endif
	return	r;
}


// ============================================================================
//!	@brief	ソースコードを読み込んでコンパイルし、VMに展開します。
// ----------------------------------------------------------------------------
bool read_and_compile(vmclass_manager* _vm, const char* _sourcefile_path)
{
	std::string	script;

	// Read sourcefile.
	try
	{
		script	= io::readall<std::string, io::fio>(_sourcefile_path);
	}
	catch (...)
	{
		std::cerr << _sourcefile_path << " : Can't read sourcefile." << std::endl;
		return	false;
	}

	// Compile source code.
	try
	{
		compiler	c(_vm);
		if (!c.compile(script, _sourcefile_path))
		{
			auto	e	= c.get_error();
			std::ostringstream	location;
			location << e->file;
			location << "(";
			location << e->line;
			location << ")";

			std::cerr << location.str();
			std::cerr << " : error : C" << std::setfill('0') << std::setw(4) << (int)e->code << " : " << e->msg << std::endl;
			return	false;
		}
		return	true;
	}
	catch (const std::exception& _e)
	{
		std::cerr << "compiler error " << _e.what() << std::endl;
		return	false;
	}
}



// ============================================================================
//!	@brief	指定したエントリポイントから起動します。
// ----------------------------------------------------------------------------
int run(vmclass_manager* _vm, const vmstring& _entryName, int _argn, char* _argv[])
{
	try
	{
		vmcontext					context;
		auto						clz		= _vm->get_class("");
		std::vector<ref<vmobj>>		keep;

		for (int i = 0; i < _argn; ++i)
		{
			keep.push_back(vmstrobj::create(_argv[i]));
		}


		struct	mainfunc : vmfunction
		{
			mainfunc()
				: vmfunction("__startup", "__startup", "", {})
			{
				mFunctionInfo->data		= this;
				mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
										{
											auto	thiz	= reinterpret_cast<mainfunc*>(_data);
											switch (_context->ip++)
											{
												case 0 :
													_context->call(thiz->mainf->function(), thiz->args.size());
													break;
												case 1 :
													thiz->finished	= true;
													return	context_result::cr_finish;
											}
											return	context_result::cr_continue;
										};
			}

			std::vector<vmvalue>	args;
			vmfunction*				mainf		= nullptr;
			bool					finished	= false;
		};
		mainfunc	mf;

		if (clz->find_function({_entryName}, _argn))
		{
			for (auto& k : keep)
			{
				mf.args.push_back(vmvalue(static_cast<vmobj*>(k.get())));
			}
			mf.mainf	= clz->find_function(_entryName, _argn);
		}
		else if (clz->find_function({_entryName}, 0))
		{
			mf.mainf	= clz->find_function(_entryName, 0);
		}

		context	= vmcontext::create(mf.function(), mf.args.size(), (const vmvalue*)mf.args.data());
		vmcontext::set_context(&context);
		while (!mf.finished)
		{
			context.run();
		}
		context.destroy_global_variable(_vm);
		vmcontext::set_context(nullptr);
		keep	= {};

		return	*reinterpret_cast<int32_t*>(context.argument_address(1));
	}
	catch (const PASSION_NAMESPACE::compiler_error& _e)
	{
		std::ostringstream	location;
		location << _e.location().name();
		location << "(";
		location << _e.location().line();
		location << ")";

		std::cerr << location.str();
		std::cerr << " : error : C" << std::setfill('0') << std::setw(4) << (int)_e.code() << " : " << _e.what() << std::endl;
		return	false;
	}
	catch (const std::exception& _e)
	{
		std::cerr << "runtime error " << _e.what() << std::endl;
		return	1;
	}
}

