#ifndef	__passion__pp_stringinter__hpp
#define	__passion__pp_stringinter__hpp
PASSION_NAMESPACE_BEGIN


// 文字列の補間
template<class Compiler>
class	string_interpolation
{
public:
	using	value_type		= tokenreader::value_type;
	using	token_type		= tokenreader::token_type;
	using	compiler_type	= Compiler;

	void operator () (compiler_type& _cmplr, tokenreader& _in)
	{
		if (_in == token_type::STRING)
		{
			// 式埋め込みがあるか調べる
			bool		hasExpression	= false;
			std::size_t	begin			= 1;
			std::size_t	pos				= 0;
			auto&		text			= _in.str();
			tokenbuffer	expression;
			for ( ; pos < text.size(); )
			{
				if (text[pos++] != '$')
				{
					continue;
				}
				if (pos < text.size())
				{
					if (text[pos++] == '{')
					{
						// ${ の前までを文字列として追加
						if (hasExpression)
						{
							expression.push_back(value_type(token_type::CODE, "+", _in));
						}
						expression.push_back(value_type(token_type::STRING, "\"" + text.substr(begin, (pos-2) - begin) + "\"", _in));
						hasExpression	= true;

						std::size_t	exppos	= pos;
						for (;;)
						{
							++pos;

							if (pos == text.size())
							{
								THROW_ERROR(err_not_close_string_expression, _in);
							}

							if (text[pos] == '}')
							{
								expression.push_back(value_type(token_type::CODE, "+", _in));
								expression.push_back(value_type(token_type::CODE, "(", _in));
								expression.push_back(tokenbuffer::create({text.substr(exppos, pos-exppos), _in.sourcename(), _in.sourceline()}));
								expression.push_back(value_type(token_type::CODE, ")", _in));
								begin	= ++pos;
								break;
							}
						}
					}
				}
			}
			if (hasExpression)
			{
				if (begin < text.size() - 1)
				{
					expression.push_back(value_type(token_type::CODE, "+", _in));
					expression.push_back(value_type(token_type::STRING, "\"" + text.substr(begin, text.size() - 1 - begin) + "\"", _in));
				}
				++_in;
				_in.push(std::move(expression));
			}
		}
	}
};


PASSION_NAMESPACE_END
#endif

