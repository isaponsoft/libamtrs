/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <sstream>
#include <amtrs/tests.hpp>
#include <amtrs/console/escape.hpp>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/lang/psnvm.hpp>
#include <amtrs/memory/hash_map.hpp>
#include <amtrs/string/shared_string.hpp>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/sstring.hpp>
#include <amtrs/string/trnstext.hpp>
#include <amtrs/system/log.hpp>
using namespace amtrs;
using namespace amtrs::filesystem;
using namespace psnvm;
using namespace psnvm::lang;

#define	_PRFX(_name)	lang_passion_pfl_##_name

// 結果が 一致する値であるかのテスト
#define	OK_TEST(_datatype, _value)	\
			out = std::stringstream();\
			pvlnode		code;\
			vmachine	vm;\
			validator	vv;\
			auto var = __run(code, vm, vv, src);\
			if (var.datatype.name() != _datatype || var.value.i != (intmax_t)_value)\
			{\
				auto m = amtrs::format("{} ({})", {(intmax_t)var.value.i, (text)var.datatype.name()});\
				m = m + (text)(" != " #_value " (" #_datatype ")");\
				AMTRS_TEST_ERROR((text)m)\
			}\
			else if (AMTRS_TEST_NAMESPACE::oklog)\
			{\
				AMTRS_TEST_NAMESPACE::teststream() << "Line:" << __LINE__ << ": OK" << std::endl;\
			}\

#define	OK_INTTTEST(_value)	\
			out = std::stringstream();\
			pvlnode		code;\
			vmachine	vm;\
			validator	vv;\
			auto var = __run(code, vm, vv, src);\
			if (var.value.i != (intmax_t)_value)\
			{\
				auto m = amtrs::format("0x{X} ({})", {(intmax_t)var.value.i, (text)var.datatype.name()});\
				m = m + " != " + (text)amtrs::format("0x{X}", {_value}) + " (int)";\
				AMTRS_TEST_ERROR((text)m)\
			}\
			else\
			{\
				if (logs)\
				{\
					auto m = amtrs::format("0x{X} ({})", {(intmax_t)var.value.i, (text)var.datatype.name()});\
					m = m + " == " + (text)amtrs::format("0x{X}", {_value}) + " (int)";\
					AMTRS_FORCE_LOG(m.c_str());\
				}\
				if (AMTRS_TEST_NAMESPACE::oklog)\
				{\
					AMTRS_TEST_NAMESPACE::teststream() << "Line:" << __LINE__ << ": OK" << std::endl;\
				}\
			}

// 結果が constring であるかのテスト
#define	TEST_CONSTRING(_value)	\
			out = std::stringstream();\
			pvlnode		code;\
			vmachine	vm;\
			validator	vv;\
			auto var = __run(code, vm, vv, src);\
			if (var.datatype.name() != "psn::constring" || text(_value) != text((char const*)var.value.p))\
			{\
				auto m = amtrs::format("{} ({})", {(char const*)var.value.p, (text)var.datatype.name()});\
				m = m + (text)(" != " #_value " (psn::constring)");\
				AMTRS_TEST_ERROR((text)m)\
			}\
			else if (AMTRS_TEST_NAMESPACE::oklog)\
			{\
				AMTRS_TEST_NAMESPACE::teststream() << "Line:" << __LINE__ << ": OK" << std::endl;\
			}\

// validated したノードの一致テスト
#define	TEST_VALIDATE(_value)	\
			out = std::stringstream();\
			validator	vv;\
			pvlnode		v	= vv.validate(src);\
			serialize(v, [](text _text) { out << _text; });\
			for (auto& ft : vv.functions)\
			{\
				for (auto& f : ft.second)\
				{\
					out << std::endl;\
					if (f->statement)\
					{\
						out << "(function `" << (text)f->name << "`";\
						if (f->statement) out << " ";\
						serialize(f->statement, [](text _text)\
						{\
							out << _text;\
						});\
						out << ")";\
					}\
				}\
			}\
			auto s = sstring(out.str()).replace("\r", " ").replace("\n", " ");\
			AMTRS_TEST_EQ((text)s, (text)_value);


#define	TEST_VALIDATE_MAINONLY(_value)	\
			out = std::stringstream();\
			validator	vv;\
			pvlnode		v	= vv.validate(src);\
			serialize(v, [](text _text) { out << _text; });\
			auto s = sstring(out.str()).replace("\r", " ").replace("\n", " ");\
			AMTRS_TEST_EQ((text)s, (text)_value);


#define	ECHO_TEST(_str)	\
			AMTRS_TEST_EQ((text)_str, (text)out.str())

namespace {
	std::stringstream	out;
	vmvariable __run(pvlnode& code, vmachine& vm, validator& vv, text _src);
}


// C++のコードとそれを文字列化したものを用意します。
#define	__MAKE_TESTCODE_INT(_answer, ...)	\
	{\
		__VA_ARGS__;\
		auto src = #__VA_ARGS__ " return " #_answer ";";\
		OK_INTTTEST(_answer);\
	}


#if	1
AMTRS_TEST_F(_PRFX(test))
{
	sstring	rootpath	= normalize_path<sstring>((sstring)parent_path(parent_path(__FILE__)) + "/share/psnvm/translate/");
	trnstext::set_from_file(rootpath, "psnvm.txt");

	{
std::cout << std::endl;
	static const bool	logs	= true;
#if	1


	__MAKE_TESTCODE_INT(
		x[0].a,
		class	XYZ
		{
		public:
			XYZ()
				: a(1)
			{}
			int	a;
		};
		XYZ*	x	= new XYZ[10];
	)

#else
		{
		auto	src = R"(
	struct	A
	{
		virtual int vtbl() { return 0; }
		int32_t	a;
	};
	struct	B : A
	{
		int32_t	b;
	};
	struct	C : B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	return	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C));
		)";


			validator	valid;
			valid.add("echo", "void", {"int"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
			{
				out << _argv[0];
				return	0;
			});
			valid.add("operator new", "void*", {"size_t"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
			{
				size_t	s	= _argv[0];
				void*	p	= operator new (s);
				vmachine::current()->add_memory(p, s);
				return	reinterpret_cast<intmax_t>(p);
			});
			valid.add("operator delete", "void", {"void*"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
			{
				void*	p	= reinterpret_cast<void*>(_argv[0]);
				if (vmachine::current()->remove_memory(p))
				{
					operator delete (p);
				}
				return	0;
			});
			pvlnode		v	= valid.validate(src);
			serialize(v, [](text _text)
			{
				std::cout << _text;
			});
			std::cout << std::endl;
			for (auto& ft : valid.functions)
			{
				for (auto& f : ft.second)
				{
					if (f->statement)
					{
						std::cout << "(function `" << (text)f->name << "`";
						if (f->statement) std::cout << " ";
						serialize(f->statement, [](text _text)
						{
							std::cout << _text;
						});
						std::cout << ")" << std::endl;
					}
					else
					{
					//	std::cout << "(native `" << (text)f->name << "`)";
					}
				}
			}

		for (auto& cls : valid.classes)
		{
			auto		c		= cls.second.at<vdecl_class>();
			if (!c) { continue; }
			std::cout << "----------------------------" << std::endl;
			std::cout << "class " << (text)c->name << std::endl;

			std::cout << " vars" << std::endl;
			for (auto& l : c->variables)
			{
				size_t	sz	= l.datatype.size_of();
				std::cout << (text)format("  +{04X} {2X} {-14s} {}", { l.offset, sz, (text)l.datatype.name(), (text)l.name });
				if (((text)l.name).starts_with("$_vtbl"))
				{
					std::cout << " + " << l.vtblidx;
				}
				std::cout << std::endl;
			}
			std::cout << (text)format("  size : {1$d}({1$04X}), align : {2$d}({2$04X})", {c->size_of(), c->align_of()}) << std::endl;
			if (c->castoffset.size() > 0)
			{
				std::cout << std::endl;
				std::cout << " cast" << std::endl;
				for (auto& cpos : c->castoffset)
				{
					std::cout << (text)format("  {04X} {}", { cpos.offset, (text)cpos.cls->name }) << std::endl;
				}
			}
			if (c->vtbl.size() > 0)
			{
				std::cout << std::endl;
				std::cout << " vtbl" << std::endl;
				for (auto& vf : c->vtbl)
				{
					std::cout << (text)format("  {}", { (text)vf->name }) << std::endl;
				}
			}
			std::cout << std::endl;
		}


			vmachine	vm;
			auto		ret		= vm.run(v);
			if (!vm.bounds_error().empty())
			{
				sourcelocation	loc	= compute_location(src, vm.bounds_error().data(), 1);
				std::cout << std::endl << (loc.filename.empty() ? (text)"src" : loc.filename) << "(" << loc.line << ":" << loc.cols << ") : bounds error." << std::endl;

				auto	line	= sstring(text(loc.linetop, vm.bounds_error().e).read_line().trim("\r\n")).replace("\t", " ");
				std::cout << "         ";
				std::cout << text(line) << std::endl;
				for (int i = 0; i < loc.cols; ++i)
				{
					std::cout << ' ';
				}
				std::cout << "         ";
				std::cout << "^" << std::endl;
			}
			else
			{
				std::cout << std::hex << ret.value.i << std::endl;
			}

		}
#endif
	}
}
#endif

AMTRS_TEST_F(_PRFX(0_expression))
{
	sstring	rootpath	= normalize_path<sstring>((sstring)parent_path(parent_path(__FILE__)) + "/share/psnvm/translate/");
	trnstext::set_from_file(rootpath, "psnvm.txt");

	static const bool	logs	= false;

	{
		auto	src	= R"( int a = 96; return a;	)";
		OK_TEST("int", 96);
	}
	{
		auto	src	= R"(
			void f() { return; }
			f();
			return	1;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			int f() { return 33; }
			int	a	= 7;
			f();
			return	a;
		)";
		OK_TEST("int", 7);
	}


	// static expression
	{
		auto	src	= R"(	return 1;		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(	1 == 2;	)";
		OK_TEST("bool", false);
	}
	{
		auto	src	= R"( int f() { return 1 + 3; } return f();	)";
		OK_TEST("int", 4);
	}
	{
		auto	src	= R"( int f() { int	a = 96; return a; } return f();	)";
		OK_TEST("int", 96);
	}
	// function argv
	{
		auto	src	= R"(
			int f(int a)
			{
				int	b = 96;
				return a;
			}
			return f(11);
		)";
		OK_TEST("int", 11);
	}
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	b;
			}
			return f(11, 12);
		)";
		OK_TEST("int", 12);
	}
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	a + b + 7;
			}
			return f(11, 12);
		)";
		OK_TEST("int", 30);
	}
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	(a + b) * 2;
			}
			return f(3, 5);
		)";
		OK_TEST("int", 16);
	}
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	a + b * 2;
			}
			return f(3, 5);
		)";
		OK_TEST("int", 13);
	}
	// reference
	{
		auto	src	= R"(
			void f(int& a)
			{
				a	= 12;
			}
			int	x	= 2;
			f(x);
			return	x;
		)";
		OK_TEST("int", 12);
	}
	{
		auto	src	= R"(
			void f(int& a, int& b)
			{
				a	= b;
			}
			int	x	= 2;
			int	y	= 99;
			f(x, y);
			return	x;
		)";
		OK_TEST("int", 99);
	}
	// overload
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	b;
			}
			int f(int a)
			{
				return	0;
			}
			return f(11, 12);
		)";
		OK_TEST("int", 12);
	}
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	b;
			}
			int f(int a)
			{
				return	0;
			}
			return f(11, 12);
		)";
		OK_TEST("int", 12);
	}
	// return type
	{
		auto	src	= R"(
			bool f(int a, int b)
			{
				return	a == b;
			}
			return f(11, 12);
		)";
		OK_TEST("bool", false);
	}
	{
		auto	src	= R"(
			bool f(int a, int b)
			{
				return	a != b;
			}
			return f(11, 12);
		)";
		OK_TEST("bool", true);
	}
	// function nest call
	{
		auto	src	= R"(
			int f(int a, int b)
			{
				return	f1(a + 1, b + 2);
			}
			int f1(int a, int b)
			{
				return	a * b;
			}
			return f(11, 12);
		)";
		OK_TEST("int", ((11+1) * (12+2)));
	}
	{
		auto	src	= R"(
			int f1(int a, int b)
			{
				return	a * b;
			}
			int f(int a, int b)
			{
				return	f1(a + 1, b + 2);
			}
			return f(3, 4);
		)";
		OK_TEST("int", ((3+1) * (4+2)));
	}
	{
		auto	src	= R"(
			int f2(int a, int b, int c)
			{
				return	f1(a, b) + c;
			}
			int f(int a, int b)
			{
				return	a * b;
			}
			int f3(int a, int b, int c)
			{
				return	f2(a, b, c);
			}
			int f1(int a, int b)
			{
				return	f(a + 1, b + 2);
			}
			return f3(1, 2, 3);
		)";
		OK_TEST("int", 11);
	}
	// class
	{
		auto	src	= R"(
			class	C
			{
				int	a;
				int	b;
			};
			return	sizeof(C);
		)";
		OK_TEST("int", sizeof(int) * 2);
	}
	// class
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
				int	b;
			};

			C	c;
			c.a	= 1;
			c.b	= 2;
			return	c.a + c.b;
		)";
		OK_TEST("int", 3);
	}
	// class
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
				int	b;
			};

			int f(C& a)
			{
				return	3;
			}

			C	c;
			return	f(c);
		)";
		OK_TEST("int", 3);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
				int	b;
			};

			void f(C& a)
			{
				a.a	= 21;
			}

			C	c;
			f(c);
			return	c.a;
		)";
		OK_TEST("int", 21);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
				int	b;
			};
			C	c;
			c.a	= 1;
			c.b	= 2;
			return	c.a + c.b;
		)";
		OK_TEST("int", 3);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
				int	b;
			};

			int f(C& c)
			{
				c.a	= 21;
				return	0;
			}

			C	c;
			c.b	= 7;
			f(c);
			return	c.a + c.b;
		)";
		OK_TEST("int", 28);
	}
	{
		auto	src	= R"(
			void f(int* v)
			{
			}

			int	n	= 1;
			f(&n);
			return	n;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			void f(int* v)
			{
				*v	= 12;
			}

			int	n	= 1;
			f(&n);
			return	n;
		)";
		OK_TEST("int", 12);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
			};
			void f(int* v)
			{
				*v	= 1111;
			}

			C	c;
			f(&c.a);
			return	c.a;
		)";
		OK_TEST("int", 1111);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	a;
			};
			void f(C* v)
			{
				v->a	= 1234;
			}

			C	c;
			f(&c);
			return	c.a;
		)";
		OK_TEST("int", 1234);
	}
	// class member function
	{
		auto	src	= R"(
			class	C
			{
			public:
				int m(int x)
				{
					return	this->a + x;
				}

				int	a;
			};

			C	c;
			c.a	= 981;
			return	c.m(2);
		)";
		OK_TEST("int", 983);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C()
				{
					this->a	= 1;
				}

				int	a;
			};

			C	c;
			return	c.a;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C()
				{
					a	= 3;
				}

				int	a;
			};

			C	c;
			return	c.a;
		)";
		OK_TEST("int", 3);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C()
					: a(5)
				{
				}

				int	a;
			};
			C	c;
			return	c.a;
		)";
		OK_TEST("int", 5);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int operator () (int x)
				{
					return	1 + x;
				}

				int	a;
			};

			C	c;
			return	c(123);
		)";
		OK_TEST("int", 124);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	v;
			};

			int f(C& _l, C& _r)
			{
				return	_l.v + _r.v;
			}

			int operator + (C& _l, C& _r)
			{
				return	_l.v + _r.v;
			}

			C	a;	a.v = 1;
			C	b;	b.v = 2;
			
			return	f(a, b) + (a + b);
		)";
		OK_TEST("int", 6);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				int	v;

				int operator + (int x)
				{
					return	22 + x;
				}
			};

			int f(C* _l, C& _r)
			{
				return	_l->v + _r.v - 1;
			}

			int operator + (C& _l, C& _r)
			{
				return	f(&_l, _r) + 3;
			}

			C	a;	a.v = 4;
			C	b;	b.v = 5;
			
			return	(a + 11) + (a + b);
		)";
		OK_TEST("int", 44);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C(int arg)
					: v(arg)
				{
				}

				int	v;
			};

			C	c(71);
			return	c.v;

		)";
		OK_TEST("int", 71);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C(int arg)
					: v(arg)
				{
				}
				int	v;
			};
			C	c(71);
			return	c.v;
		)";
		OK_TEST("int", 71);
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C(int arg)
					: v(arg)
				{
				}
				int	v;
			};
			C	c(4432);
			return	(&c)->v;
		)";
		OK_TEST("int", 4432);
	}
	{
		auto	src	= R"(
			struct	C
			{
				C(int arg)
					: v(arg)
				{
					echo(arg+1);
				}
				int	v;
			};
			C	c(4432);
			return	c.v;
		)";
		OK_TEST("int", 4432);
		ECHO_TEST("4433")
	}
	{
		auto	src	= R"(
			struct	C
			{
				C(int arg)
					: v(arg)
				{
					echo(v);
				}
				~C()
				{
					echo(v+2);
				}
				int	v;
			};
			C	c(4432);
			return	c.v;
		)";
		OK_TEST("int", 4432);
		ECHO_TEST("44324434")
	}
	// increment, decrement
	{
		auto	src	= R"(
			int		a	= 0;
			int		b	= ++a;
			return	b;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			int		a	= 0;
			int		b	= ++a;
			return	a;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			int		a	= 0;
			int		b	= a++;
			return	b;
		)";
		OK_TEST("int", 0);
	}
	{
		auto	src	= R"(
			int		a	= 0;
			int		b	= a++;
			return	a;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			int		a	= 10;
			int		b	= --a;
			return	b;
		)";
		OK_TEST("int", 9);
	}
	{
		auto	src	= R"(
			int		a	= 10;
			int		b	= --a;
			return	a;
		)";
		OK_TEST("int", 9);
	}
	{
		auto	src	= R"(
			int		a	= 10;
			int		b	= a--;
			return	b;
		)";
		OK_TEST("int", 10);
	}
	{
		auto	src	= R"(
			int		a	= 0;
			int		b	= a--;
			return	a;
		)";
		OK_TEST("int", -1);
	}
	// constexpr f() が展開されていることを確認する
	{
		auto	src	= R"(
			constexpr int f(int a, int b)
			{
				return	a + b;
			}
			return	f(1, 2);
		)";
		TEST_VALIDATE_MAINONLY("(returnval (int 3)) (return)")
	}
	// new/delete のテスト
	{
		auto	src	= R"(
			class	C
			{
			public:
				C()
					: a(123)
				{
				}

				int	a;
			};

			C*	c;
			int	r	= 1;
			c		= new C();
			r		= c->a;
			delete	c;
			return	r;
		)";
		OK_TEST("int", 123)
	}
	{
		auto	src	= R"(
			class	C
			{
			public:
				C()
					: a(123)
				{
					return;
				}

				int	a;
			};

			C*	c;
			int	r	= 1;
			c		= new C();
			r		= c->a;
			delete	c;
			return	r;
		)";
		OK_TEST("int", 123)
	}
	// statement if
	{
		auto	src	= R"(
			int		a = 0;
			if (a)
			{
				a	= 1;
			}
			else
			{
				a	= 2;
			}
			return	a;
		)";
		OK_TEST("int", 2);
	}
	{
		auto	src	= R"(
			int		a = 1;
			if (a)
			{
				a	= 1;
			}
			else
			{
				a	= 2;
			}
			return	a;
		)";
		OK_TEST("int", 1);
	}
	{
		auto	src	= R"(
			int		a = 0;
			if (a)
			{
				return	99;
			}
			else
			{
				return	88;
			}
			return	33;
		)";
		OK_TEST("int", 88);
	}
	{
		auto	src	= R"(
			int		a = 1;
			if (a)
			{
				return	99;
			}
			else
			{
				return	88;
			}
			return	33;
		)";
		OK_TEST("int", 99);
	}
	// sattement for
	{
		auto	src	= R"(
			int	r	= 0;
			for (int i = 1; i <= 10; ++i)
			{
				r = r + i;
			}
			return	r;
		)";
		OK_TEST("int", 55)
	}
	// sattement while
	{
		auto	src	= R"(
			int	r	= 0;
			while (r < 100)
			{
				++r;
			}
			return	r;
		)";
		OK_TEST("int", 100)
	}
	// sattement do
	{
		auto	src	= R"(
			int	r	= 0;
			do
			{
				++r;
			} while (r < 100);
			return	r;
		)";
		OK_TEST("int", 100)
	}
	// sattement switch
	{
		auto	src	= R"(
			int	r	= 0;
			switch (12)
			{
				case 11 :	++r;
				case 12 :	++r;
				case 13 :	++r;
			}
			return	r;
		)";
		OK_TEST("int", 2)
	}
	{
		auto	src	= R"(
			int	r	= 0;
			switch (12)
			{
				case 11 :	++r;
				case 12 :	++r;	break;
				case 13 :	++r;
			}
			return	r;
		)";
		OK_TEST("int", 1)
	}
	{
		auto	src	= R"(
			int	r	= 0;
			switch (12)
			{
				default:	++r;
				case 11:	++r;
			}
			return	r;
		)";
		OK_TEST("int", 2)
	}
	{
		auto	src	= R"(
			int	r	= 0;
			switch (12)
			{
				case 11 :	++r;
				case 11+1 :	++r;	break;
				case 11+2 :	++r;
			}
			return	r;
		)";
		OK_TEST("int", 1)
	}
	{
		auto	src	= R"(
			int	r	= 11;
			switch (r)
			{
				case 11 :	++r;
				case 11+1 :	++r;	break;
				case 11+2 :	++r;	break;
				default:	r = 200;
			}
			return	r;
		)";
		OK_TEST("int", 13)
	}
	// initializer list
	{
		auto	src	= R"(
			struct	C
			{
				int	a;
				int	b;
			};
			C	c{ .a = 1, .b = 2 };
			return	c.a + c.b;
		)";
		OK_TEST("int", 3)
	}
	{
		auto	src	= R"(
			struct	C
			{
				int	a;
				int	b;
			};
			C	c{ 10, 9 };
			return	c.a + c.b;
		)";
		OK_TEST("int", 19)
	}

	// class constractor
	__MAKE_TESTCODE_INT(
		tmp.b,
		struct B     { int b; B() : b(99)   {} };
		B	tmp;
	)

	// class extend
	__MAKE_TESTCODE_INT(
		sizeof(C),
		struct B     { int a; };
		struct C : B { int b; };
	)
	__MAKE_TESTCODE_INT(
		tmp.b + tmp.c,
		struct B     { int b; B() : b(99)     {} };
		struct C : B { int c; C() : c(9900)   {} };
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.b + tmp.c,
		struct B     { int b; B() : b(99)     {} };
		struct C : B { int c; C() : c(9900)   {} };
		struct D : C { };
		D	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c,
		struct A        { int a; A() : a(1) {} };
		struct B        { int b; B() : b(2) {} };
		struct C : B    { int c; C() : c(3) {} };
		struct D : A, C { };
		D	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c,
		struct A        { int a; A() : a(1) {} };
		struct B        { int b; B() : b(2) {} };
		struct C : B    { int c; C() : c(3) {} };
		struct D : C, A { };
		D	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b,
		struct A     { int a, b; A(int x, int y) : a(x), b(y) {} };
		struct B : A {           B() : A(1, 2) {} };
		B	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c,
		struct A {
			A(int x, int y, int z)
				: a(x), b(y), c(z)
			{}
			int	a, b, c;
		};
		A	tmp(1, 2, 3);
	)
	__MAKE_TESTCODE_INT(
		(intmax_t)&tmp->a + (0x100*(intmax_t)&tmp->b) + (0x10000*(intmax_t)&tmp->c) + (0x1000000*(intmax_t)&tmp->p),
		struct	A
		{
			int	a;
		};
		struct	B : A
		{
			int	b;
			void*	p;
			int	c;
			virtual int get_a() { return 0; }
		};
		B*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->__) + (0x10000 * (intmax_t)&tmp->b),
		struct	Space
		{
			int32_t	__;
		};
		struct	A
		{
			int32_t	a;
			virtual int get_a() { return 1; }
		};
		struct	B : Space, A
		{
			int32_t	b;
			virtual int get_a() { return 2; }
		};
		B*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		((intmax_t)&tmp->__) + (0x100 * (intmax_t)&tmp->a) + (0x10000 * (intmax_t)&tmp->b),
		struct	A
		{
			int	__;
			int	a;
			virtual int get_a() { return 1; }
		};
		struct	B : A
		{
			int	b;
			virtual int get_a() { return 2; }
		};
		B*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		((intmax_t)&tmp->__) + (0x100 * (intmax_t)&tmp->a) + (0x10000 * (intmax_t)&tmp->b),
		struct	A
		{
			int	__;
			int	a;
		};
		struct	B : A
		{
			int	b;
		};
		B*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		tmp.a(),
		struct	Space
		{
			int	__;
		};
		struct	A : Space
		{
			virtual int a() { return 1; }
		};
		struct	B : A
		{
			virtual int a() { return 2; }
		};
		B	tmp;
	)
	// Base class memory map.
	__MAKE_TESTCODE_INT(
		(intmax_t)&tmp->a,
		struct	A
		{
			int	a	= 1;
		};
		struct	B
		{
			int	b	= 2;
		};
		struct	C : A, B
		{
			int	c	= 3;
			virtual int get() { return 0; }
		};
		C*	tmp	= 0;
	)
	// Base class memory map.
	__MAKE_TESTCODE_INT(
		(intmax_t)&tmp->a,
		struct	A
		{
			int	a	= 1;
		};
		struct	B
		{
			int	b	= 2;
			virtual int get() { return 0; }
		};
		struct	C : A, B
		{
			int	c	= 3;
		};
		C*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c,
		struct	A
		{
			int	a	= 0x001;
		};
		struct	B : A
		{
			int	b	= 0x010;
			virtual int get() { return 0; }
		};
		struct	C : B
		{
			int	c	= 0x100;
			virtual int get() { return 0; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		virtual int vtbl() { return 0; }
		int32_t	a;
	};
	struct	B : A
	{
		int32_t	b;
	};
	struct	C : B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		void*	vtbl;
		int32_t	a;
	};
	struct	B : A
	{
		int32_t	b;
	};
	struct	C : B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
		((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(C)),
		struct	A
		{
			int	a	= 0x001;
			virtual int get() { return 0; }
		};
		struct	B
		{
			int	b;
			virtual int get2() { return 0; }
		};
		struct	C : A, B
		{
			int	c	= 0x100;
			virtual int get() { return a; }
			virtual int get2() { return c; }
		};
		C*	tmp	= 0;
	)
	// member offset address & structure size.
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		virtual int vtbl() { return 0; };
		int32_t	a;
	};
	struct	B
	{
		int32_t	b;
	};
	struct	C : A, B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c),
	struct	A
	{
		void*	vtbl;
		int32_t	a;
	};
	struct	B
	{
		int32_t	b;
	};
	struct	C : A, B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(B)) + (0x100000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		int32_t	vtbl;
		int32_t	a;
	};
	struct	B : A
	{
		int32_t	b;
	};
	struct	C : B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(B)) + (0x100000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		void*	vtbl;
		int32_t	a;
	};
	struct	B
	{
		int32_t	b;
	};
	struct	C : A, B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		int32_t	a;
	};
	struct	B
	{
		int32_t	b;
	};
	struct	C : A, B
	{
		int32_t	c;
	};
	C*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		void*	vtbl;
		int32_t	a;
	};
	struct	B
	{
		int32_t	b;
	};
	struct	C : A, B
	{
		int32_t	c;
	};
	C*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		virtual int get_a() { return 1; }
		int32_t	a;
	};
	struct	B
	{
		int32_t	b;
	};
	struct	C : A, B
	{
		int32_t	c;
	};
	C*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->b) + (0x10000 * (intmax_t)&tmp->c) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		void*	vtbl;
		int32_t	a;
	};
	struct	B : A
	{
		int32_t	b;
	};
	struct	C : B
	{
		int32_t	c;
	};
	C*	tmp = 0;
	)
	__MAKE_TESTCODE_INT(
	((intmax_t)&tmp->a) + (0x100 * (intmax_t)&tmp->__) + (0x10000 * (intmax_t)&tmp->b) + (0x1000000 * (intmax_t)sizeof(A)) + (0x100000000 * (intmax_t)sizeof(B)) + (0x10000000000 * (intmax_t)sizeof(C)),
	struct	A
	{
		virtual int get_a() { return 1; }
		int32_t	a;
	};
	struct	B : A
	{
		int32_t	__;
	};
	struct	C : B
	{
		int32_t	b;
	};
	C*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		tmp.d,
		struct A     { int a; A() : a(1) {} };
		struct D : A { int d; D(int x) : A(), d(2) {} };
		D	tmp(1);
	)
	__MAKE_TESTCODE_INT(
		tmp.a,
		struct A       { int a, b; A(int x, int y) : a(x), b(y) {} };
		struct B : A   {           B(int v) : A(v, v) {} };
		struct C       { int c;    C(int x) : c(x) {} };
		struct D : B,C { int d;    D(int x, int y, int z) : C(x), B(y), d(z) {} };
		D	tmp(1, 2, 3);
	)
	__MAKE_TESTCODE_INT(
		tmp.geta() + tmp.getb() + tmp.getc(),
		struct	A
		{
			int	a;
			A(int v) : a(v) {}
			int geta() { return a; };
		};
		struct	B
		{
			int	b;
			B(int x) : b(x) {}
			int getb() { return b; };
		};
		struct	C : A, B
		{
			int	c;
			C(int x, int y, int z) : A(x), B(y), c(z) {}
			int getc() { return c; };
		};
		C	tmp(1, 10, 100);
	)
	__MAKE_TESTCODE_INT(
		tmp2->get() + tmp.get(),
		struct	A
		{
			int get() { return 1; };
		};
		struct	C : A
		{
			int get() { return 999; };
		};
		C	tmp;
		A*	tmp2	= &tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp2->a + tmp3->b,
		struct	A
		{
			int	a;
			A(int x) : a(x) {}
		};
		struct	B
		{
			int	b;
			B(int x) : b(x) {}
		};
		struct	C : A, B
		{
			C() : A(9), B(90) {}
		};
		C	tmp;
		A*	tmp2	= &tmp;
		B*	tmp3	= &tmp;
	)
	__MAKE_TESTCODE_INT(
		c.a + c.b + c.c,
		struct	A
		{
			int	a	= 1;
		};
		struct	B
		{
			int	b	= 2;

		};
		struct	C : A, B
		{
			int	c	= 3;
		};
		C	c;
	)
	__MAKE_TESTCODE_INT(
		c.a + c.b + c.c,
		struct	A
		{
			A(int x) : a(x) {}
			int	a	= 1;
		};
		struct	B
		{
			int	b	= 2;
		};
		struct	C : A, B
		{
			C()
				: A(9)
			{}

			int	c	= 3;
		};
		C	c;
	)
	__MAKE_TESTCODE_INT(
		tmp.a(),
		struct	A
		{
			virtual int a() { return 1; }
		};
		struct	B : A
		{
			virtual int a() { return 2; }
		};
		B	tmp;
	)
	__MAKE_TESTCODE_INT(
		(int)(tmp.a() + sizeof(A)),
		struct	A
		{
			virtual int a() { return 1; }
		};
		A	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a(),
		struct	A
		{
			virtual int a() { return 1; }
		};
		struct	B : A
		{
			virtual int a() { return 2; }
		};
		B	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a(),
		struct	A
		{
			virtual int a() { return 1; }
		};
		struct	B
		{
		};
		struct	C : A, B
		{
			virtual int a() { return 2; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a(),
		struct	A
		{
		};
		struct	B
		{
			virtual int a() { return 1; }
		};
		struct	C : A, B
		{
			virtual int a() { return 2; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c + tmp.get(),
		struct	A
		{
			int	a	= 1;
		};
		struct	B
		{
			int	b	= 2;
			virtual int get() { return b; }
		};
		struct	C : A, B
		{
			int	c	= 3;
			virtual int get() { return c; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c + tmp.get(),
		struct	A
		{
			int	a	= 1;
			virtual int get() { return a; }
		};
		struct	B
		{
			int	b	= 2;
		};
		struct	C : A, B
		{
			int	c	= 3;
			virtual int get() { return c; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c + tmp.get() + tmp.get2(),
		struct	A
		{
			int	a	= 1;
			virtual int get() { return a; }
		};
		struct	B
		{
			int	b	= 2;
			virtual int get2() { return b; }
		};
		struct	C : A, B
		{
			int	c	= 3;
			virtual int get() { return c; }
			virtual int get2() { return c; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a(),
		struct	A
		{
			virtual int a() { return 1; }
		};
		struct	B : A
		{
			virtual int a() { return 2; }
		};
		B	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.a + tmp.b + tmp.c,
		struct	A
		{
			int		a	= 900;
			virtual int get_a() { return 1; }
		};
		struct	B
		{
			int		b	= 9;
			int		b2;
			virtual int get_b() { return 2; }
		};
		struct	C : A, B
		{
			int		c	= 90;
			virtual int get_a() { return   5; }
			virtual int get_b() { return  70; }
			virtual int get_c() { return 900; }
		};
		struct	D
		{
			int		d	= 9000;
			virtual int get_d() { return  1000; }
		};
		struct	E : C, D
		{
			int		e	= 90000;
			virtual int get_e() { return 20000; }
		};
		E	tmp;
	)
	__MAKE_TESTCODE_INT(
		(intmax_t)&tmp->a,
		struct	A
		{
			int	a;
		};
		struct	B : A
		{
			int	b;
			virtual int get_a() { return 1; }
		};
		B*	tmp	= 0;
	)
	__MAKE_TESTCODE_INT(
		(intmax_t)&tmp->b,
		struct	A
		{
			int	a;
		};
		struct	B : A
		{
			int	b;
			virtual int get_a() { return 1; }
		};
		B*	tmp	= 0;
	)
/*
	__MAKE_TESTCODE_INT(
		tmp.get_a() + tmp.get_b(),
		struct	A
		{
			virtual int get_a() { return 1; }
		};
		struct	B
		{
			virtual int get_b() { return 2; }
		};
		struct	C : A, B
		{
			virtual int get_a() { return 99; }
			virtual int get_b() { return 9900; }
		};
		C	tmp;
	)
	__MAKE_TESTCODE_INT(
		tmp.get_a() + tmp.get_b(),
		struct	A
		{
			virtual int get_a() { return 1; }
		};
		struct	B
		{
			virtual int get_b() { return 2; }
		};
		struct	C : A, B
		{
			virtual int get_ax() { return 99; }
			virtual int get_bx() { return 9900; }
		};
		C	tmp;
	)
*/
}


namespace
{
	vmvariable __run(pvlnode& code, vmachine& vm, validator& vv, text _src)
	{
		vv.add("echo", "void", {"int"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
		{
			out << _argv[0];
			return	0;
		});
		vv.add("operator new", "void*", {"size_t"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
		{
			size_t	s	= _argv[0];
			void*	p	= operator new (s);
			vmachine::current()->add_memory(p, s);
			return	reinterpret_cast<intmax_t>(p);
		});
		vv.add("operator delete", "void", {"void*"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
		{
			void*	p	= reinterpret_cast<void*>(_argv[0]);
			if (vmachine::current()->remove_memory(p))
			{
				operator delete (p);
			}
			return	0;
		});

		code	= vv.validate(_src);
		if (vv.bad())
		{
			for (auto& e : vv.errors)
			{
				auto	m	= _TT(error_string(e.err));
				auto	fm	= format(m.data(), {(text)e.token1, (text)e.token2});
				std::cerr << console::fgcolor_light_red <<  "E" << PSNVM_ERRORCODE(e.err) << " : " << (text)c8_to_cr(fm) << console::fgcolor_reset << std::endl;
				for (auto& amb : e.ambitiouses)
				{
					std::cerr << console::fgcolor_light_magenta << (text)amb.token << console::fgcolor_reset << std::endl;
				}
			}
			return	{};
		}

		auto		ret		= vm.run(code);
		if (ret.datatype.is_reference())
		{
			ret.datatype	= ret.datatype.at<vdecl_reference>()->t;
			ret.value.i		= *static_cast<intmax_t*>(ret.value.p);
		}
		return	ret;
	}
}
