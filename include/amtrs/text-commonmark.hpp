/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__text__commonmark__hpp
#define	__libamtrs__text__commonmark__hpp
#include <map>
#include <string>
#include <string_view>
#include <list>
#include "amtrs.hpp"
#include "string.hpp"
AMTRS_NAMESPACE_BEGIN



template<class CharT, class Traits = std::char_traits<CharT>>
class	basic_commonmark
{
public:
	using	char_type		= typename std::remove_const<CharT>::type;
	using	pointer			= typename std::add_const<char_type>::type*;
	using	const_pointer	= pointer;
	using	traits_type		= Traits;
	using	view_type		= std::basic_string_view<char_type>;
	using	indent_type		= size_t;
	using	position_type	= size_t;
	using	linenumber_type	= int;
	using	size_type		= size_t;

	enum class	nodetype
	{
		none,						// no initialized
		blockquote,					// <blockquote>
		list,						// <ul>
		item,						// <li>
		codeblock,					// <pre><code>
		htmlblock,					// "<"
		rawblock,					// 
		paragraph,					// <p>
		heading,					// <h[1-6]>
		thematic_break,				// <hr>
		table,						// <table>
		table_row,					// <tr>
		table_cell,					// <td>

		// inline
		text,						//
		softbreak,					// \n
		linebreak,					// <br />
		code,						// <code>   * without <pre>
		htmlinline,
		custominline,
		emph,						// <em>
		strong,						// <strong>
		link,						// <a>
		image,						// <img>
	};

	struct	node
	{
		nodetype			type		= nodetype::none;
		bool				entering;

		//! HTML化されたテキストをコールバックに返します。
		//! ひとつのノードでも複数回コールバックが呼ばれることがあります。
		template<class Callback>
		void to_html(Callback&& _callback);

		union
		{
			// nodetype::list
			struct
			{
					bool		bullet;
					int			start;
			}	list;

			// nodetype::heading
			struct
			{
					int			level;
			}	heading;

			// nodetype::codeblock
			struct
			{
					view_type	language;
					view_type	text;
			}	codeblock;

			// nodetype::htmlblock
			struct
			{
					view_type	text;
			}	htmlblock;

			// nodetype::rawblock
			struct
			{
					view_type	text;
			}	rawblock;

			// nodetype::text
			struct
			{
					view_type	text;
			}	text;

			// nodetype::code
			struct
			{
					view_type	text;
			}	code;

			// nodetype::htmlinline
			struct
			{
					view_type	text;
			}	htmlinline;

			// nodetype::custominline
			struct
			{
					view_type	text;
			}	custominline;

			// nodetype::link
			struct
			{
					view_type	url;
					view_type	title;
			}	link;

			// nodetype::image
			struct
			{
					view_type	url;
					view_type	title;
			}	image;

			// nodetype::table_row
			struct
			{
					bool		header;
					int			line_number;
			}	table_row;

			// nodetype::table_cell
			struct
			{
					bool		header;
					int			line_number;
					int			col_number;
					char		alignments;		// 0x00 or 'l' or 'c' or 'r'
			}	table_cell;
		};
	};


	using	callback_func	= void(node& n, void* userparam);


	basic_commonmark() = default;
	basic_commonmark(basic_commonmark const&) = default;
	basic_commonmark(basic_commonmark&&) = default;
	basic_commonmark& operator = (basic_commonmark const&) = default;
	basic_commonmark& operator = (basic_commonmark&&) = default;


	


	void parse(view_type _source, callback_func _callback, void* _userparam);


	template<class Callback>
	void parse(view_type _source, Callback _callback)
	{
		parse(_source, [](node& n, void* param)
		{
			Callback*	cb	= reinterpret_cast<Callback*>(param);
			(*cb)(n);
		}, &_callback);
	}
};



template<class CharT, class Traits>
template<class Callback>
void basic_commonmark<CharT, Traits>::node::to_html(Callback&& _callback)
{
	auto	intto	= [](char* _buff, int _n)
	{
		char*	cur	= _buff;
		do
		{
			*cur++	= (_n%10) + '0';
			_n	= _n / 10;
		} while (_n > 0);
		*cur	= '\0';
		--cur;
		while (cur > _buff)
		{
			std::swap(*cur--, *_buff++);
		}
	};

	switch (type)
	{
		case nodetype::blockquote :
		{
			_callback(view_type(entering ? "<blockquote>\n" : "</blockquote>\n"));
			break;
		}

		case nodetype::list :
		{
			if (list.bullet)
			{
				_callback(entering ? "<ul>\n" : "</ul>\n");
			}
			else
			{
				if (list.start == 1)
				{
					_callback(entering ? "<ol>\n" : "</ol>\n");
				}
				else
				{
					if (entering)
					{
						char	buff[20];
						_callback("<ol start=\"");
						intto(buff, list.start);
						_callback(buff);
						_callback("\">\n");
					}
					else
					{
						_callback("</ol>\n");
					}
				}
			}
			break;
		}

		case nodetype::item :
		{
			_callback(entering ? "<li>" : "</li>\n");
			break;
		}

		case nodetype::heading :
		{
			char	buff[20];
			_callback(entering ? "<h" : "</h");
			intto(buff, heading.level);
			_callback(buff);
			_callback(">");
			if (!entering)
			{
				_callback("\n");
			}
			break;
		}

		case nodetype::codeblock :
		{
			_callback("<pre><code");
			if (!codeblock.language.empty())
			{
				_callback(" class=\"language-");
				_callback(codeblock.language);
				_callback("\"");
			}
			_callback(">");
			_callback(codeblock.text);
			_callback("</code></pre>\n");
			break;
		}

		case nodetype::htmlblock :
		{
			_callback(htmlblock.text);
			break;
		}

		case nodetype::rawblock :
		{
			_callback(rawblock.text);
			break;
		}

		case nodetype::thematic_break :
		{
			_callback("<hr />\n");
			break;
		}

		case nodetype::paragraph :
		{
			_callback(entering ? "\n<p>" : "</p>\n");
			break;
		}

		case nodetype::text :
		{
			_callback(text.text);
			break;
		}

		case nodetype::linebreak :
		{
			_callback("<br />\n");
			break;
		}

		case nodetype::softbreak :
		{
			_callback("\n");
			break;
		}

		case nodetype::code :
		{
			_callback("<code>");
			_callback(code.text);
			_callback("</code>");
			break;
		}

		case nodetype::htmlinline :
		{
			_callback(htmlinline.text);
			break;
		}

		case nodetype::custominline :
		{
			_callback(custominline.text);
			break;
		}

		case nodetype::strong :
		{
			_callback(entering ? "<strong>" : "</strong>");
			break;
		}

		case nodetype::emph :
		{
			_callback(entering ? "<em>" : "</em>");
			break;
		}

		case nodetype::link :
		{
			if (entering)
			{
				_callback("<a href=\"");
				_callback(link.url);
				_callback("\"");
				if (!link.title.empty())
				{
					_callback(" title=\"");
					_callback(link.title);
					_callback("\"");
				}
				_callback(">");
			}
			else
			{
				_callback("</a>");
			}
			break;
		}

		case nodetype::image :
		{
			if (entering)
			{
				_callback("<img src=\"");
				_callback(image.url);
				_callback("\" alt=\"");
			}
			else
			{
				_callback("\"");	// close alt=
				if (!image.title.empty())
				{
					_callback(" title=\"");
					_callback(image.title);
					_callback("\"");
				}
				_callback(" />");
			}
			break;
		}

		case nodetype::table :
		{
			_callback(view_type(entering ? "<table>\n" : "</tbody>\n</table>\n"));
			break;
		}

		case nodetype::table_row :
		{
			if (table_row.header)
			{
				_callback(view_type(entering ? "<tr>\n<thead>\n" : "</thead>\n</tr>\n"));
			}
			else if (table_row.line_number == 0)
			{
				_callback(view_type(entering ? "<tbody>\n<tr>\n" : "\n</tr>\n"));
			}
			else
			{
				_callback(view_type(entering ? "<tr>\n" : "\n</tr>\n"));
			}
			break;
		}

		case nodetype::table_cell :
		{
			if (table_cell.header)
			{
				_callback(view_type(entering ? "<th>\n" : "\n</th>\n"));
			}
			else if (entering)
			{
				_callback("<td");
				if (table_cell.alignments)
				{
					_callback(" style=\"text-align: ");
					 switch (table_cell.alignments)
					{
						case 'l': _callback("left");	break;
						case 'c': _callback("center");	break;
						case 'r': _callback("right");	break;
					 }
					_callback("\"");
				}
				_callback(">");
			}
			else
			{
				_callback("\n</td>\n");
			}
			break;
		}

		default :
		{
			break;
		}
	}
}




using	commonmark	= basic_commonmark<char>;
using	markdown	= commonmark;


AMTRS_NAMESPACE_END
#endif
