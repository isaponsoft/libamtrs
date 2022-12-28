/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__lson__hpp
#define	__libamtrs__lang__lson__hpp
/* Little Serialize Object Notation */
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN namespace lson {

using	integer_type	= uintmax_t;


/* destination buffer overfllow. */
#define	LSON_RESULT_OVERFLLOW			-1

/* source data format error. */
#define	LSON_RESULT_UNFORMAT			-2


/*
 * json string to lson string
 *	dest, dest_end
 *		destination buffer.
 *
 *	json_start, json_end
 *		source buffer.
 *
 *	flags
 *		0 reserved.
 *
 *	return
 *		>= 0	success. write size.
 *		< 0		error. 	LSON_RESULT_xxxxx
 */
static inline int json_to_lson(char** dest, char* dest_end, char const** json_start, char const* json_end);


/*
 * lson string to json string
 *	dest, dest_end
 *		destination buffer.
 *
 *	lson_start, lson_end
 *		source buffer.
 *
 *	return
 *		>= 0	success. write size.
 *		< 0		error. 	LSON_RESULT_xxxxx
 */
static inline int lson_to_json(char** dest, char* dest_end, char const** lson_start, char const* lson_end);


/*
 * Compute line number.
 */
static inline int lson_compute_location(int* line_number, char const** line_start, char const* start, char const* end, char const* locate);

/*
 * Find not blunk.
 *	return = null, not blank
 */
static inline char const* lson_find_not_blunk(char const* start, char const* end)
{
	while (start != end)
	{
		if (*start != ' ' && *start != '\t' && *start != '\r' && *start != '\n')
		{
			return	start;
		}
		++start;
	}
	return	NULL;
}



#define	LSON_SYM_NUMERIC				'+'
#define	LSON_SYM_NUMERIC_INV			'-'
#define	LSON_SYM_E32BASE				'*'
#define	LSON_SYM_E32BASE_INV			'|'
#define	LSON_SYM_STRING					'\''
#define	LSON_SYM_STRING_START			'"'
#define	LSON_SYM_STRING_END				'"'
#define	LSON_SYM_TRUE					'<'
#define	LSON_SYM_FALSE					'>'
#define	LSON_SYM_NULL					'~'
#define	LSON_SYM_ARRAY_START			'['
#define	LSON_SYM_ARRAY_END				']'
#define	LSON_SYM_ASSOC_START			'{'
#define	LSON_SYM_ASSOC_END				'}'
#define	LSON_SYM_TYPE					'`'
#define	LSON_SYM_RESERVE1				'^'
#define	LSON_SYM_RESERVE2				'='
#define	LSON_SYM_RESERVE3				'_'
#define	LSON_INTERNAL_FLAG_OMITSYMBOL	(1<<0)
#define	LSON_INTERNAL_FLAG_ASSOCKEY		(1<<1)
#define	LSON_SKIPSPACE()				if (lson_json_skip_comment(&s, se)) { *json_start = epos; return LSON_RESULT_UNFORMAT; };


static inline int lson_is_vchar(char c)
{
	return	(c >= 'a' && c <= 'z')
	 	||	(c >= 'A' && c <= 'Z')
	 	||	(c >= '0' && c <= '9')
		||	(c == '_')
		?	1 : 0;
}



static inline int lson_is_markchar(char c)
{
	return
		c == LSON_SYM_NUMERIC
	||	c == LSON_SYM_NUMERIC_INV
	||	c == LSON_SYM_E32BASE
	||	c == LSON_SYM_E32BASE_INV
	||	c == LSON_SYM_STRING
	||	c == LSON_SYM_STRING_START
	||	c == LSON_SYM_STRING_END
	||	c == LSON_SYM_TRUE
	||	c == LSON_SYM_FALSE
	||	c == LSON_SYM_NULL
	||	c == LSON_SYM_ARRAY_START
	||	c == LSON_SYM_ARRAY_END
	||	c == LSON_SYM_ASSOC_START
	||	c == LSON_SYM_ASSOC_END
	||	c == LSON_SYM_TYPE
	||	c == LSON_SYM_RESERVE1
	||	c == LSON_SYM_RESERVE2
	||	c == LSON_SYM_RESERVE3
	;
}

static inline int lson_json_skip_comment(char const** src, char const* end)
{
	char const*	s	= *src;
	for (;;)
	{
		while (s != end && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')) { ++s; }
		if (end - s < 2)
		{
			*src	= s;
			return	0;
		}
		if (s[0] == '/' && s[1] == '/')
		{
			s += 2;
			for (;;)
			{
				if (s == end)
				{
					return	LSON_RESULT_UNFORMAT;
				}
				if (*s == '\n' || *s == '\r')
				{
					++s;
					break;
				}
				++s;
			}
			continue;
		}
		if (s[0] == '/' && s[1] == '*')
		{
			s += 2;
			for (;;)
			{
				if ((end - s) < 2)
				{
					return	LSON_RESULT_UNFORMAT;
				}
				if (s[0] == '*' && s[1] == '/')
				{
					s += 2;
					break;
				}
				++s;
			}
			continue;
		}
		*src	= s;
		return	0;
	}
}


static inline int ___json_to_lson(int* has_term, char** dest, char* de, char const** json_start, char const* json_end, int flags)
{
	char		c;
	char const*	s		= *json_start;
	char const*	se		= json_end;
	char const*	beg		= s;
	char const*	epos	= s;
	char*		d		= *dest;
	char*		ds		= d;
	LSON_SKIPSPACE();

	*has_term	= 0;
	c			= *s;
	beg			= s;

	/* check boolean */
	if (c == 't' || c == 'f')
	{
		if ((c == 't' && s+4 > se) || (c == 'f' && s+5 > se))
		{
			*json_start	= epos;
			return	LSON_RESULT_UNFORMAT;
		}
		if ((de - d) == 0)
		{
			*json_start	= epos;
			return	LSON_RESULT_OVERFLLOW;
		}
		*d++ = c == 't' ? LSON_SYM_TRUE : LSON_SYM_FALSE;
		s += c == 't' ? 4 : 5;
		*dest		= d;
		*json_start	= s;
		*has_term	= 1;
		return	(int)(d - ds);
	}

	/* check null */
	if (c == 'n')
	{
		if (c == 'n' && s+4 > se)
		{
			*json_start	= epos;
			return	LSON_RESULT_UNFORMAT;
		}
		if ((de - d) == 0)
		{
			*json_start	= epos;
			return	LSON_RESULT_OVERFLLOW;
		}
		*d++ = LSON_SYM_NULL;
		s += 4;
		*dest		= d;
		*json_start	= s;
		*has_term	= 1;
		return	(int)(d - ds);
	}

	/* check numeric */
	if ((c == '-') || (c >= '0' && c <= '9'))
	{
		int				minus	= 0;
		integer_type	i		= 0;
		*has_term	= 0;
		if (c == '-')
		{
			minus	= 1;
			++s;
			if (s == se || *s < '0' || *s > '9')
			{
				*json_start	= epos;
				return	LSON_RESULT_UNFORMAT;
			}
			beg	= s;
		}

		/* read integer */
		while (s != se && *s >= '0' && *s <= '9')
		{
			i	= i * 10 + *s++ - '0';
		}

		/* real number */
		if (s != se && *s == '.')
		{
			char*	send	= (char*)se;
			strtod(beg, &send);
			if (((send - s) + 1) > (de - d))
			{
				*json_start	= epos;
				return	LSON_RESULT_OVERFLLOW;
			}
			if (!(flags&LSON_INTERNAL_FLAG_OMITSYMBOL) || minus)
			{
				*d++	= minus ? LSON_SYM_NUMERIC_INV : LSON_SYM_NUMERIC;
			}
			while (beg != send)
			{
				*d++ = *beg++;
			}
			s	= send;
		}
		/* not real number */
		else
		{
			if (((beg - s) + 1) > (de - d))
			{
				*json_start	= epos;
				return	LSON_RESULT_OVERFLLOW;
			}
			if (s - beg >= 4)
			{
				static const char	tableA[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234+";
				static const char	tableB[] = "abcdefghijklmnopqrstuvwxyz56789-";
				if ((de - d) == 0) { *json_start = epos; return LSON_RESULT_OVERFLLOW; }
				*d++ = minus ? LSON_SYM_E32BASE_INV : LSON_SYM_E32BASE;
				for (;;)
				{
					if ((de - d) == 0) { *json_start = epos; printf("dest empty\n"); return LSON_RESULT_OVERFLLOW; }
					if (i < 32)
					{
						*d++ = tableA[i];
						break;
					}
					*d++ = tableB[i % 32];
					i /= 32;
				}
				*has_term	= 1;
			}
			else
			{
				if (!(flags&LSON_INTERNAL_FLAG_OMITSYMBOL) || minus)
				{
					*d++	= minus ? LSON_SYM_NUMERIC_INV : LSON_SYM_NUMERIC;
				}
				while (beg != s)
				{
					*d++ = *beg++;
				}
			}
		}
		*dest		= d;
		*json_start	= s;
		return	(int)(d - ds);
	}

	/* check string */
	if (c == '"' || c == '\''
	 || ((flags&LSON_INTERNAL_FLAG_ASSOCKEY) && ((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || (*s == '_')))
	)
	{
		char*	markpos	= d;
		int		sz		= 0;
		int		mode	= 0;
		char	fst		= 0;
		*has_term		= 0;
		if (c == '"' || c == '\'')
		{
			fst	= c;
			++s;
		}
		if (s == se) { *json_start = epos; return LSON_RESULT_UNFORMAT; }
		beg	= s;
		if (!(flags&LSON_INTERNAL_FLAG_OMITSYMBOL) || (*s >= '0' && *s <= '9') || (*s == '_'))
		{
			if ((de - d) == 0)
			{
				*json_start	= epos;
				return	LSON_RESULT_OVERFLLOW;
			}
			*d++ = LSON_SYM_STRING;
		}

		for (;;)
		{
			if (s == se)
			{
				*json_start	= epos;
				return	LSON_RESULT_UNFORMAT;
			}
			if (*s == '\\')
			{
				++s;
				if (s == se)
				{
					*json_start	= epos;
					return	LSON_RESULT_UNFORMAT;
				}
				++s;
				continue;
			}
			if (fst)
			{
				if (*s == fst)
				{
					++s;
					break;
				}
			}
			else if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || (*s >= '0' && *s <= '9') || (*s == '_')))
			{
				break;
			}

			if (lson_is_markchar(*s) && *markpos != LSON_SYM_STRING_START)
			{
				*markpos	= LSON_SYM_STRING_START;
				s			= beg;
				d			= markpos+1;
				*has_term	= 1;
				continue;
			}
			*d++ = *s++;
		}
		if (*markpos == LSON_SYM_STRING_START)
		{
			if ((de - d) == 0)
			{
				*json_start	= epos;
				return	LSON_RESULT_OVERFLLOW;
			}
			*d++ = LSON_SYM_STRING_END;
		}
		*dest		= d;
		*json_start	= s;
		return	(int)(d - ds);
	}

	/* check array */
	if (c == '[')
	{
		int	sz;
		++s;
		if ((de - d) == 0)
		{
			*json_start	= epos;
			return	LSON_RESULT_OVERFLLOW;
		}
		*d++ = LSON_SYM_ARRAY_START;
		LSON_SKIPSPACE();
		if (*s != ']')
		{
			/* firt value */
			sz	= ___json_to_lson(has_term, &d, de, &s, se, LSON_INTERNAL_FLAG_OMITSYMBOL);
			if (sz < 0)
			{
				*json_start	= s;
				return	sz;
			}
			for (;;)
			{
				LSON_SKIPSPACE();
				c	= *s;
				if (c != ',' && c != ']')
				{
					*json_start	= epos;
					return	LSON_RESULT_UNFORMAT;
				}
				if (c == ',')
				{
					++s;
					LSON_SKIPSPACE();
					c	= *s;
				}
				if (c == ']')
				{
					if (d[-1] == '}')
					{
						--d;
					}
					++s;
					break;
				}
				LSON_SKIPSPACE();
				if (d[-1] == '}' && (*s == '[' || *s == '{'))
				{
					--d;
				}
				sz	= ___json_to_lson(has_term, &d, de, &s, se, *has_term ? LSON_INTERNAL_FLAG_OMITSYMBOL : 0);
				if (sz < 0)
				{
					*json_start	= s;
					return	sz;
				}
			}
		}
		else
		{
			++s;
		}
		if ((de - d) == 0)
		{
			*json_start	= epos;
			return	LSON_RESULT_OVERFLLOW;
		}
		*d++ = LSON_SYM_ARRAY_END;
		*dest		= d;
		*json_start	= s;
		*has_term	= 1;
		return	(int)(d - ds);
	}

	/* check assoc */
	if (c == '{')
	{
		++s;
		if ((de - d) == 0) { *json_start = epos; return LSON_RESULT_OVERFLLOW; }
		*d++ = LSON_SYM_ASSOC_START;
		LSON_SKIPSPACE();
		if (*s != '}')
		{
			int	sz;
			/* read first key */
			sz	= ___json_to_lson(has_term, &d, de, &s, se, LSON_INTERNAL_FLAG_OMITSYMBOL|LSON_INTERNAL_FLAG_ASSOCKEY);
			if (sz < 0)
			{
				*json_start	= s;
				return	sz;
			}
			for (;;)
			{
				LSON_SKIPSPACE();
				if (*s !=':') { *json_start	= epos; return LSON_RESULT_UNFORMAT; } ++s;

				/* read value */
				sz	= ___json_to_lson(has_term, &d, de, &s, se, *has_term ? LSON_INTERNAL_FLAG_OMITSYMBOL : 0);
				if (sz < 0)
				{
					*json_start	= s;
					return	sz;
				}
				LSON_SKIPSPACE();
				if (*s != ',' && *s != '}')
				{
					*json_start	= epos;
					return	LSON_RESULT_UNFORMAT;
				}
				if (*s == ',')
				{
					++s;
					LSON_SKIPSPACE();
				}
				if (*s == '}')
				{
					++s;
					break;
				}
				/* read key */
				sz	= ___json_to_lson(has_term, &d, de, &s, se, (*has_term ? LSON_INTERNAL_FLAG_OMITSYMBOL : 0) | LSON_INTERNAL_FLAG_ASSOCKEY);
				if (sz < 0)
				{
					*json_start	= s;
					return	sz;
				}
			}
		}
		else
		{
			++s;
		}

		if ((de - d) == 0)
		{
			*json_start	= epos;
			return	LSON_RESULT_OVERFLLOW;
		}
		*d++ = LSON_SYM_ASSOC_END;
		*dest		= d;
		*json_start	= s;
		*has_term	= 1;
		return	(int)(d - ds);
	}
	*json_start	= epos;
	return	LSON_RESULT_UNFORMAT;
}

static inline int json_to_lson(char** dest, char* de, char const** json_start, char const* json_end)
{
	int	has_term;
	return	___json_to_lson(&has_term, dest, de, json_start, json_end, LSON_INTERNAL_FLAG_OMITSYMBOL);
}


static inline int lson_to_json(char** dst, char* dend, char const** src, char const* send)
{
	char const*	beg;
	char*		d	= *dst;
	char const*	s	= *src;
	char*		ds	= d;
	if (s == send)
	{
		return	LSON_RESULT_UNFORMAT;
	}

	if (*s == LSON_SYM_ASSOC_START)
	{
		++s;
		if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = '{';
		if (s == send) { return LSON_RESULT_UNFORMAT; }
		if (!(*s == LSON_SYM_ASSOC_END || *s == LSON_SYM_ARRAY_END || *s == LSON_SYM_ARRAY_START || *s == LSON_SYM_ASSOC_START))
		{
			int	ret;
			for (;;)
			{
				ret	= lson_to_json(&d, dend, &s, send);
				if (ret < 0) { return ret; }
				if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = ':';
				ret	= lson_to_json(&d, dend, &s, send);
				if (ret < 0) { return ret; }
				if (s == send) { return LSON_RESULT_UNFORMAT; }
				if (*s == LSON_SYM_ASSOC_END || *s == LSON_SYM_ARRAY_END || *s == LSON_SYM_ARRAY_START || *s == LSON_SYM_ASSOC_START) { break; }
				if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = ',';
			}
		}
		if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = '}';
		if (*s == LSON_SYM_ASSOC_END) { ++s; }
		*dst	= d;
		*src	= s;
		return	(int)(d - ds);
	}

	if (*s == LSON_SYM_ARRAY_START)
	{
		int	ret;
		++s;
		if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = '[';
		for (;;)
		{
			ret	= lson_to_json(&d, dend, &s, send);
			if (ret < 0) { return ret; }
			if (s == send) { return LSON_RESULT_UNFORMAT; }
			if (*s == LSON_SYM_ARRAY_END) { break; }
			if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = ',';
		}
		if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = ']';
		*dst	= d;
		*src	= ++s;
		return	(int)(d - ds);
	}

	if (*s == LSON_SYM_TRUE)
	{
		if ((dend - d) < 4) { return LSON_RESULT_OVERFLLOW; }
		*d++	= 't';
		*d++	= 'r';
		*d++	= 'u';
		*d++	= 'e';
		*dst	= d;
		*src	= s+1;
		return	(int)(d - ds);
	}
	if (*s == LSON_SYM_FALSE)
	{
		if ((dend - d) < 5) { return LSON_RESULT_OVERFLLOW; }
		*d++	= 'f';
		*d++	= 'a';
		*d++	= 'l';
		*d++	= 's';
		*d++	= 'e';
		*dst	= d;
		*src	= s+1;
		return	(int)(d - ds);
	}
	if (*s == LSON_SYM_NULL)
	{
		if ((dend - d) < 4) { return LSON_RESULT_OVERFLLOW; }
		*d++	= 'n';
		*d++	= 'u';
		*d++	= 'l';
		*d++	= 'l';
		*dst	= d;
		*src	= s+1;
		return	(int)(d - ds);
	}


	if (*s == LSON_SYM_STRING_START)
	{
		beg		= s++;
		for (;;)
		{
			if (s == send) { return LSON_RESULT_UNFORMAT; }
			if (*s == '\\')
			{
				++s;	if (s == send) { return LSON_RESULT_UNFORMAT; }
				++s;	if (s == send) { return LSON_RESULT_UNFORMAT; }
				continue;
			}
			else if (*s == LSON_SYM_STRING_END)
			{
				++s;
				break;	
			}
			++s;
		}
		if ((s - beg) > (dend - d)) { return LSON_RESULT_OVERFLLOW; }
		while (beg != s) { *d++ = *beg++; }
		*dst	= d;
		*src	= s;
		return	(int)(d - ds);
	}

	if (*s == LSON_SYM_TYPE)
	{
		beg		= s++;
		for (;;)
		{
			if (s == send) { return LSON_RESULT_UNFORMAT; }
			if (*s == '\\')
			{
				++s;	if (s == send) { return LSON_RESULT_UNFORMAT; }
				++s;	if (s == send) { return LSON_RESULT_UNFORMAT; }
				continue;
			}
			else if (*s == LSON_SYM_TYPE)
			{
				++s;
				break;	
			}
			++s;
		}
		if ((s - beg) > (dend - d)) { return LSON_RESULT_OVERFLLOW; }
		while (beg != s) { *d++ = *beg++; }
		*dst	= d;
		*src	= s;
		return	(int)(d - ds);
	}

	if ((*s >= '0' && *s <= '9') || (*s == LSON_SYM_NUMERIC_INV) || (*s == LSON_SYM_NUMERIC))
	{
		beg		= s++;
		if ((*beg == LSON_SYM_NUMERIC_INV || *beg == LSON_SYM_NUMERIC) && s == send) { return LSON_RESULT_UNFORMAT; }
		if (*beg == LSON_SYM_NUMERIC) { beg = s; }
		for (;;)
		{
			if (s == send) { break; }
			if ((*s < '0' || *s > '9') && (*s != '.'))
			{
				break;	
			}
			++s;
		}
		if ((s - beg) > (dend - d)) { return LSON_RESULT_OVERFLLOW; }
		while (beg != s) { *d++ = *beg++; }
		*dst	= d;
		*src	= s;
		return	(int)(d - ds);
	}

	if (*s == LSON_SYM_E32BASE || *s == LSON_SYM_E32BASE_INV)
	{
		int				minus	= *s == LSON_SYM_E32BASE_INV;
		integer_type	n		= 0;
		integer_type	dig		= 1;
		int				len;
		char*			des;
		++s;
		static const int8_t	table[] = {
		/*  x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* 00- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* 10- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31, -1, 95, -1, -1,		/* 20- */
			26, 27, 28, 29, 30, 90, 91, 92, 93, 94, -1, -1, -1, -1, -1, -1,		/* 30- */
			-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,		/* 40- */
			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,		/* 50- */
			-1, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,		/* 60- */
			79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, -1, -1, -1, -1, -1,		/* 70- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* 80- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* 90- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* a0- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* b0- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* c0- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* d0- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* e0- */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,		/* f0- */
		};
		for (;;)
		{
			if (s == send) { return LSON_RESULT_UNFORMAT; }
			char	v	= (char)table[(unsigned char)*s++];
			if (v < 0)
			{
				*src	= s;
				return	LSON_RESULT_UNFORMAT;
			}
			n	+= (v&0x1f) * dig;
			if (v < 32)
			{
				break;
			}
			dig	*= 32;
		}
		if (minus)
		{
			if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; } *d++ = '-';
		}
		len	= 0;
		des	= d;
		do
		{
			if ((dend - d) == 0) { return LSON_RESULT_OVERFLLOW; }
			*d++ = (n % 10) + '0';
			n /= 10;
			++len;
		} while (n > 0);
		for (n = 0; n < len/2; ++n)
		{
			char	tmp	= des[n];
			des[n]	= des[len-n-1];
			des[len-n-1]	= tmp;
		}
		*dst	= d;
		*src	= s;
		return	(int)(d - ds);
	}

	if (*s == LSON_SYM_STRING)
	{
		++s;
	}
	beg		= s;
	for (;;)
	{
		if (s == send) { break; }
		if (*s == '\\')
		{
			++s;	if (s == send) { return LSON_RESULT_UNFORMAT; }
			++s;	if (s == send) { return LSON_RESULT_UNFORMAT; }
			continue;
		}
		else if (lson_is_markchar(*s) || (*s == '"'))
		{
			break;	
		}
		++s;
	}
	if (((s - beg) + 2) > (dend - d)) { return LSON_RESULT_OVERFLLOW; }
	*d++	= '"';
	while (beg != s) { *d++ = *beg++; }
	*d++	= '"';
	*dst	= d;
	*src	= s;
	return	(int)(d - ds);
}


static inline int lson_compute_location(int* line_number, char const** line_start, char const* start, char const* end, char const* locate)
{
	char const*	pos	= start;
	*line_number= 1;
	*line_start	= pos;
	while (pos != end)
	{
		if (pos == locate)
		{
			return	0;
		}
		if (*pos == '\n' || *pos == '\r')
		{
			char	c	= *pos++;
			if ((pos != end) && (*pos == '\n' || *pos == '\r') && (*pos != c))
			{
				++pos;
			}
			++(*line_number);
			*line_start	= pos;
			continue;
		}
		++pos;
	}
	return	1;
}

} AMTRS_NAMESPACE_END
#endif
