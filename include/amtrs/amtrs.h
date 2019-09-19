/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__amtrs__h
#define	__libamtrs__amtrs__h


#ifdef	__cplusplus
#define	AMTRS_EXTERN_C_BEGIN	extern "C" {
#define	AMTRS_EXTERN_C_END	}
#else
#define	AMTRS_EXTERN_C_BEGIN
#define	AMTRS_EXTERN_C_END
#endif

typedef	char		amtrs_bool;
#define	amtrs_false	0
#define	amtrs_true	(!amtrs_false)


#ifdef	__cplusplus
#define	amtrs_null	nullptr
#else
#define	amtrs_null	(NULL)
#endif

#endif
