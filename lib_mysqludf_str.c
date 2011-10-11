/* -*- coding: utf-8; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: t -*- */
/*
	lib_mysqludf_str - a library of functions to work with strings
	Copyright © 2011  Daniel Trebbien <dtrebbien@gmail.com>
	Copyright © 2007  Claudio Cherubino <claudiocherubino@gmail.com>
	web: http://www.mysqludf.org/lib_mysqludf_str/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <ctype.h>
#include <limits.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>
#include <m_ctype.h>

#ifdef __WIN__
#ifndef _WIN64
#define SystemFunction036 NTAPI SystemFunction036
#endif

#include <ntsecapi.h>
#undef SystemFunction036
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "config.h"
#include "char_vector.h"

#ifdef __WIN__
#define DLLEXP __declspec(dllexport)
#else
#define DLLEXP
#endif

#ifdef __GNUC__
/* From Check:  http://check.svn.sourceforge.net/viewvc/check/trunk/src/check.h.in?revision=HEAD
   License: LGPL 2.1 or later */
#ifdef __GNUC_MINOR__
#define GCC_VERSION_AT_LEAST(major, minor) \
		((__GNUC__ > (major)) || \
		 (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#else
#define GCC_VERSION_AT_LEAST(major, minor) 0
#endif /* __GNUC_MINOR__ */

#if GCC_VERSION_AT_LEAST(2, 95)
#define ATTRIBUTE_UNUSED __attribute__ ((unused))
#else
#define ATTRIBUTE_UNUSED 
#endif
#else
#define ATTRIBUTE_UNUSED 
#endif

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) -1)
#endif

#ifdef HAVE_DLOPEN

#define LIBVERSION ("lib_mysqludf_str version " PACKAGE_VERSION)
#define ROT_OFFSET 13

#define ARGCOUNTCHECK(typestr)	\
	if (args->arg_count != 1) { \
		snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument count: %s requires one " typestr " argument, got %d arguments", funcname, args->arg_count); \
		return 1; \
	}


#define ARGTYPECHECK(arg, type, typestr)	\
	if (arg != type) { \
		snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument type: %s requires one " typestr " argument. Expected type %d, got type %d.", funcname, type, arg); \
		return 1; \
	}

#define STRARGCHECK ARGTYPECHECK(args->arg_type[0], STRING_RESULT, "string")
#define INTARGCHECK ARGTYPECHECK(args->arg_type[0], INT_RESULT, "integer")

/******************************************************************************
** function declarations
******************************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

#define DECLARE_UDF_INIT_DEINIT(name_id) \
	DLLEXP my_bool name_id ## _init(UDF_INIT *, UDF_ARGS *, char *); \
	DLLEXP void name_id ## _deinit(UDF_INIT *);
#define DECLARE_STRING_UDF(name_id) \
	DECLARE_UDF_INIT_DEINIT(name_id) \
	DLLEXP char *name_id(UDF_INIT *, UDF_ARGS *, char *, unsigned long *, char *, char *);

DECLARE_STRING_UDF(lib_mysqludf_str_info)
DECLARE_STRING_UDF(str_numtowords)
DECLARE_STRING_UDF(str_rot13)
DECLARE_STRING_UDF(str_shuffle)
DECLARE_STRING_UDF(str_translate)
DECLARE_STRING_UDF(str_ucfirst)
DECLARE_STRING_UDF(str_ucwords)
DECLARE_STRING_UDF(str_xor)
DECLARE_STRING_UDF(str_srand)

#ifdef	__cplusplus
}
#endif

/******************************************************************************
** function definitions
******************************************************************************/

/******************************************************************************
** purpose:	called once for each SQL statement which invokes lib_mysqludf_str_info_init();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main lib_mysqludf_str_info_init() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (lib_mysqludf_str_info_init() and lib_mysqludf_str_info_init_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of lib_mysqludf_str_info_init(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/
my_bool lib_mysqludf_str_info_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 0)
	{
		strncpy(message, "No arguments allowed (udf: lib_mysqludf_str_info)", MYSQL_ERRMSG_SIZE);
		return 1;
	}

	initid->maybe_null = 0;
	initid->max_length = (sizeof LIBVERSION) - 1;
	initid->const_item = 1;
	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by lib_mysqludf_str_info_init();
**					this function is called once for each query which invokes
**					lib_mysqludf_str_info(), it is called after all of the calls to
**					lib_mysqludf_str_info() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					lib_mysqludf_str_info_init() and lib_mysqludf_str_info())
** returns:	nothing
******************************************************************************/
void lib_mysqludf_str_info_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
}

/******************************************************************************
** purpose:	obtain information about the currently installed version
**					of lib_mysqludf_str.
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the library version number
******************************************************************************/
char *lib_mysqludf_str_info(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	strcpy(result, LIBVERSION);
	*res_length = (sizeof LIBVERSION) - 1;
	return result;
}


/******************************************************************************
** purpose:	called once for each SQL statement which invokes str_numtowords();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main str_numtowords() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (str_numtowords() and str_numtowords_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of str_numtowords(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/
my_bool str_numtowords_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_numtowords";
	st_char_vector *vec;

	/* make sure user has provided exactly one integer argument */
	ARGCOUNTCHECK("integer");
	INTARGCHECK;

	vec = char_vector_alloc();
	if (vec == NULL)
	{
		strncpy(message, "char_vector_alloc() failed", MYSQL_ERRMSG_SIZE);
		return 1;
	}

	initid->ptr = (char *) vec;

	initid->maybe_null=1;

	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by str_numtowords_init(); this func
**					is called once for each query which invokes str_numtowords(),
**					it is called after all of the calls to str_numtowords() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					str_numtowords_init() and str_numtowords())
** returns:	nothing
******************************************************************************/
void str_numtowords_deinit(UDF_INIT *initid)
{
	st_char_vector *vec = (st_char_vector *) initid->ptr;
	char_vector_free(vec);
}

#define STR_LENGTH(str) ((sizeof (str)) -1)
#define STR_COMMA_LENGTH(str_lit) str_lit, STR_LENGTH(str_lit)

/******************************************************************************
** purpose:	convert numbers written in arabic digits to an english word.
**					Works for positive and negative numbers up to 9 digits long.
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the string spelling the given number in English
******************************************************************************/
char *str_numtowords(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	static const char *const powers[] = {"thousand", "million", "billion", "trillion", "quadrillion", "quintillion", "sextillion", "septillion", "octillion", "nonillion", "decillion", "undecillion", "duodecillion"};

	static const char *const ones[] = {"one", "two", "three", "four", "five",
																"six", "seven", "eight", "nine", "ten",
																"eleven", "twelve", "thirteen", "fourteen", "fifteen",
																"sixteen", "seventeen", "eighteen", "nineteen"};

	static const char *const tens[] = {"twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};

	if (args->args[0] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	st_char_vector *vec = (st_char_vector *) initid->ptr;

	long long value = *((long long *) args->args[0]);

	int part_stack[14];
	int *part_ptr = part_stack;

	assert(char_vector_length(vec) == 0);

	// check for negative values or zero
	if (value < 0)
	{
		char_vector_append(vec, STR_COMMA_LENGTH("negative "));
		value = -value;
	}
	else if (value == 0)
	{
		char *tmp = "zero";
		*res_length = STR_LENGTH("zero");
		return tmp;
	}

	// splitting the number into its parts
	for (; value; value /= 1000)
		*part_ptr++ = value % 1000;

	while (part_ptr > part_stack)
	{
		int p = *--part_ptr;

		if (p >= 100)
		{
			char_vector_strcat(vec, ones[p / 100 - 1]);
			char_vector_append(vec, STR_COMMA_LENGTH(" hundred "));
			p %= 100;
		}

		if (p >= 20)
		{
			if (p % 10)
			{
				char_vector_strcat(vec, tens[p / 10 - 2]);
				char_vector_append(vec, STR_COMMA_LENGTH("-"));
				char_vector_strcat(vec, ones[p % 10 - 1]);
				char_vector_append(vec, STR_COMMA_LENGTH(" "));
			}
			else
			{
				char_vector_strcat(vec, tens[p / 10 - 2]);
				char_vector_append(vec, STR_COMMA_LENGTH(" "));
			}
		}
		else if (p > 0)
		{
			char_vector_strcat(vec, ones[p - 1]);
			char_vector_append(vec, STR_COMMA_LENGTH(" "));
		}

		if (p && part_ptr > part_stack)
		{
			char_vector_strcat(vec, powers[part_ptr - part_stack - 1]);
			char_vector_append(vec, STR_COMMA_LENGTH(" "));
		}
	}

	*res_length = char_vector_length(vec) - 1;
	return char_vector_get_ptr(vec);
}


/******************************************************************************
** purpose:	called once for each SQL statement which invokes str_rot13();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main str_rot13() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (str_rot13() and str_rot13_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of str_rot13(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/
my_bool str_rot13_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_rot13";
	unsigned long res_length;

	/* make sure user has provided exactly one string argument */
	ARGCOUNTCHECK("string");
	STRARGCHECK;

	res_length = args->lengths[0];

	if (SIZE_MAX < res_length)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "res_length (%lu) cannot be greater than SIZE_MAX (%zu)", res_length, (size_t) (SIZE_MAX));
		return 1;
	}

	initid->ptr = NULL;

	if (res_length > 255)
	{
		char *tmp = (char *) malloc((size_t) res_length); /* This is a safe cast because res_length <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) res_length);
			return 1;
		}
		initid->ptr = tmp;
	}

	initid->maybe_null = 1;
	initid->max_length = res_length;
	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by str_rot13_init(); this func
**					is called once for each query which invokes str_rot13(),
**					it is called after all of the calls to str_rot13() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					str_rot13_init() and str_rot13())
** returns:	nothing
******************************************************************************/
void str_rot13_deinit(UDF_INIT *initid)
{
	if (initid->ptr != NULL)
		free(initid->ptr);
}

/******************************************************************************
** purpose:	perform the rot13 transform on a string, shifting each
**					character	by 13 places in the alphabet, and wrapping back
**					to the beginning if necessary.
**					Non-alphabetic character will not be modified.
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the string transformed by str_rot13
******************************************************************************/
char *str_rot13(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	int i, cod_ascii;

	if (args->args[0] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	// s will contain the user-supplied argument
	const char *s = args->args[0];

	if (initid->ptr != NULL)
	{
		result = initid->ptr;
	}

	*res_length = args->lengths[0];

	for (i = 0; i < *res_length; i++)
	{
		// cod_ascii is an integer containing the ascii code of a single character
		cod_ascii = s[i];

		if(cod_ascii >= 97 && cod_ascii <= 122)	// lower case character
		{
			cod_ascii += ROT_OFFSET;

			if(cod_ascii>122)
				cod_ascii = 96 + (cod_ascii-122);
		}
		else if(cod_ascii >= 65 && cod_ascii <= 90)	// upper case character
		{
			cod_ascii += ROT_OFFSET;

			if(cod_ascii>90)
				cod_ascii = 64 + (cod_ascii-90);
		}

		result[i] = cod_ascii;
	}

	return result;
}


/******************************************************************************
** purpose:	called once for each SQL statement which invokes str_shuffle();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main str_shuffle() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (str_shuffle() and str_shuffle_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of str_shuffle(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/

my_bool str_shuffle_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_shuffle";
	unsigned long res_length;

	/* make sure user has provided exactly one string argument */
	ARGCOUNTCHECK("string");
	STRARGCHECK;

	res_length = args->lengths[0];

	if (SIZE_MAX < res_length)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "res_length (%lu) cannot be greater than SIZE_MAX (%zu)", res_length, (size_t) (SIZE_MAX));
		return 1;
	}

	initid->ptr = NULL;

	if (res_length > 255)
	{
		char *tmp = (char *) malloc((size_t) res_length); /* This is a safe cast because res_length <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) res_length);
			return 1;
		}
		initid->ptr = tmp;
	}

	initid->maybe_null = 1;
	initid->max_length = res_length;
	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by str_shuffle_init(); this func
**					is called once for each query which invokes str_shuffle(),
**					it is called after all of the calls to str_shuffle() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					str_shuffle_init() and str_shuffle())
** returns:	nothing
******************************************************************************/
void str_shuffle_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
	if (initid->ptr != NULL)
		free(initid->ptr);
}

/******************************************************************************
** purpose:	randomly shuffle the characters of a string
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	one of the possible permutations of the original string
******************************************************************************/
char *str_shuffle(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	int i, j;
	char swp;

	if (args->args[0] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	if (initid->ptr != NULL)
	{
		result = initid->ptr;
	}

	// copy the argument string into result
	memcpy(result, args->args[0], args->lengths[0]);
	*res_length = args->lengths[0];

	for (i = 0; i < *res_length; i++)
	{
		// select a random position to swap result[i]
		j = i + rand() / (RAND_MAX / (*res_length - i) + 1);

		// swap the two characters
		swp = result[j];
		result[j] = result[i];
		result[i] = swp;
	}

	return result;
}


/******************************************************************************
** purpose:	called once for each SQL statement which invokes str_translate();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main str_translate() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (str_translate() and str_translate_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of str_translate(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/

my_bool str_translate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_translate";
	unsigned long res_length;

	/* make sure user has provided exactly three string arguments */
	if (args->arg_count != 3) {
		snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument count: str_translate requires three string arguments (subject, srcchar, dstchar), got %d argument%s.", args->arg_count, (args->arg_count == 1 ? "" : "s"));
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT 
			|| args->arg_type[1] != STRING_RESULT
			|| args->arg_type[2] != STRING_RESULT)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument type: str_translate requires three string arguments (subject, srcchar, dstchar).");
		return 1;
	} else if (args->lengths[1] != args->lengths[2]) {
		strncpy(message,"str_translate(subject, srcchar, dstchar) requires srcchar and dstchar to have the same length", MYSQL_ERRMSG_SIZE);
		return 1;
	}

	res_length = args->lengths[0];

	if (SIZE_MAX < res_length)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "res_length (%lu) cannot be greater than SIZE_MAX (%zu)", res_length, (size_t) (SIZE_MAX));
		return 1;
	}

	initid->ptr = NULL;

	if (res_length > 255)
	{
		char *tmp = (char *) malloc((size_t) res_length); /* This is a safe cast because res_length <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) res_length);
			return 1;
		}
		initid->ptr = tmp;
	}

	initid->maybe_null = 1;
	initid->max_length = res_length;
	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by str_translate_init(); this func
**					is called once for each query which invokes str_translate(),
**					it is called after all of the calls to str_translate() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					str_translate_init() and str_translate())
** returns:	nothing
******************************************************************************/
void str_translate_deinit(UDF_INIT *initid)
{
	if (initid->ptr != NULL)
		free(initid->ptr);
}

/******************************************************************************
** purpose:	scan each char in subject, and replace every occurrence of
**					a char that is contained in srcchar with the corresponding char
**					in dstchar
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the string transformed by str_translate
******************************************************************************/
char *str_translate(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	int i, j;

	if (args->args[0] == NULL || args->args[1] == NULL || args->args[2] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	// subject will contain the string to be translated
	const char *subject = args->args[0];

	// src will contain the string to be translated
	const char *src = args->args[1];
	unsigned long src_length = args->lengths[1];

	// dst will contain the string to be translated
	const char *dst = args->args[2];

	if (initid->ptr != NULL)
	{
		result = initid->ptr;
	}

	*res_length = args->lengths[0];

	memcpy(result, subject, *res_length);

	{
		char *__restrict p = result;

		for (i = 0; i < *res_length; ++i, ++p)
		{
			// for each character of the subject string we check if it is present in src
			for (j = 0; j < src_length; j++)
			{
				if (subject[i] == src[j])
				{
					// replace the character with the corresponding one from dst
					*p = dst[j];
				}
			}
		}
	}

	return result;
}


/******************************************************************************
** purpose:	called once for each SQL statement which invokes str_ucfirst();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main str_ucfirst() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (str_ucfirst() and str_ucfirst_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of str_ucfirst(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/

my_bool str_ucfirst_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_ucfirst";
	unsigned long res_length;

	/* make sure user has provided exactly one string argument */
	ARGCOUNTCHECK("string");
	STRARGCHECK;

	res_length = args->lengths[0];

	if (SIZE_MAX < res_length)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "res_length (%lu) cannot be greater than SIZE_MAX (%zu)", res_length, (size_t) (SIZE_MAX));
		return 1;
	}

	initid->ptr = NULL;

	if (res_length > 255)
	{
		char *tmp = (char *) malloc((size_t) res_length); /* This is a safe cast because res_length <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) res_length);
			return 1;
		}
		initid->ptr = tmp;
	}

	initid->maybe_null = 1;
	initid->max_length = res_length;
	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by str_ucfirst_init(); this func
**					is called once for each query which invokes str_ucfirst(),
**					it is called after all of the calls to str_ucfirst() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					str_ucfirst_init() and str_ucfirst())
** returns:	nothing
******************************************************************************/
void str_ucfirst_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
	if (initid->ptr != NULL)
		free(initid->ptr);
}

/******************************************************************************
** purpose:	make a string's first character uppercase
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the original string with the first character capitalized
******************************************************************************/
char *str_ucfirst(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	if (args->args[0] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	if (initid->ptr != NULL) {
		result = initid->ptr;
	}

	// copy the argument string into result
	memcpy(result, args->args[0], args->lengths[0]);

	// capitalize the first character of the string
	*result = my_toupper(&my_charset_latin1, *result);

	*res_length = args->lengths[0];
	*null_value = 0;
	*error = 0;
	return result;
}


/******************************************************************************
** purpose:	called once for each SQL statement which invokes str_ucwords();
**					checks arguments, sets restrictions, allocates memory that
**					will be used during the main str_ucwords() function
** receives:	pointer to UDF_INIT struct which is to be shared with all
**					other functions (str_ucwords() and str_ucwords_deinit()) -
**					the components of this struct are described in the MySQL manual;
**					pointer to UDF_ARGS struct which contains information about
**					the number, size, and type of args the query will be providing
**					to each invocation of str_ucwords(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message
**					can be stored if necessary
** returns:	1 => failure; 0 => successful initialization
******************************************************************************/

my_bool str_ucwords_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_ucwords";
	unsigned long res_length;

	/* make sure user has provided exactly one string argument */
	ARGCOUNTCHECK("string");
	STRARGCHECK;

	res_length = args->lengths[0];

	if (SIZE_MAX < res_length)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "res_length (%lu) cannot be greater than SIZE_MAX (%zu)", res_length, (size_t) (SIZE_MAX));
		return 1;
	}

	initid->ptr = NULL;

	if (res_length > 255)
	{
		char *tmp = (char *) malloc((size_t) res_length); /* This is a safe cast because res_length <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) res_length);
			return 1;
		}
		initid->ptr = tmp;
	}

	initid->maybe_null = 1;
	initid->max_length = res_length;
	return 0;
}

/******************************************************************************
** purpose:	deallocate memory allocated by str_ucwords_init(); this func
**					is called once for each query which invokes str_ucwords(),
**					it is called after all of the calls to str_ucwords() are done
** receives:	pointer to UDF_INIT struct (the same which was used by
**					str_ucwords_init() and str_ucwords())
** returns:	nothing
******************************************************************************/
void str_ucwords_deinit(UDF_INIT *initid)
{
	if (initid->ptr != NULL)
		free(initid->ptr);
}

/******************************************************************************
** purpose:	transform to uppercase the first character of each word in a string.
**					Any string of characters that is immediately after a whitespace
**					is considered a word
** receives:	pointer to UDF_INIT struct which contains pre-allocated memory
**					in which work can be done; pointer to UDF_ARGS struct which
**					contains the functions arguments and data about them; pointer
**					to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the original string with the first character of each word capitalized
******************************************************************************/
char *str_ucwords(UDF_INIT *initid, UDF_ARGS *args,
			char *result, unsigned long *res_length,
			char *null_value, char *error)
{
	int i;
	int new_word = 0;

	if (args->args[0] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	if (initid->ptr != NULL) {
		result = initid->ptr;
	}

	// copy the argument string into result
	memcpy(result, args->args[0], args->lengths[0]);

	*res_length = args->lengths[0];

	// capitalize the first character of each word in the string
	for (i = 0; i < *res_length; i++)
	{
		if (my_isalpha(&my_charset_latin1, result[i]))
		{
			if (!new_word)
			{
				new_word = 1;
				result[i] = my_toupper(&my_charset_latin1, result[i]);
			}
		}
		else
		{
			new_word = 0;
		}
	}

	return result;
}

/******************************************************************************
** purpose:	called once for each invocation of str_xor();
**					checks arguments, sets restrictions
** receives:	pointer to UDF_INIT struct; pointer to UDF_ARGS struct which contains information about
**					the number, length, and type of args that were passed to str_xor(); pointer to a char
**					array of size MYSQL_ERRMSG_SIZE in which an error message can be stored if necessary
** returns:	1 => failure; 0 => success
******************************************************************************/
my_bool str_xor_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_xor";
	unsigned long res_length;

	if (args->arg_count != 2)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument count: str_xor requires exactly two string arguments, got %d arguments.", MYSQL_ERRMSG_SIZE, args->arg_count);
		return 1;
	}
	if (args->arg_type[0] != STRING_RESULT 
			|| args->arg_type[1] != STRING_RESULT)
	{
		strncpy(message, "wrong argument type: str_xor requires two string arguments", MYSQL_ERRMSG_SIZE);
		return 1;
	}

	res_length = args->lengths[0];
	if (args->lengths[1] > res_length)
		res_length = args->lengths[1];

	if (SIZE_MAX < res_length)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "res_length (%lu) cannot be greater than SIZE_MAX (%zu)", res_length, (size_t) (SIZE_MAX));
		return 1;
	}

	initid->ptr = NULL;

	if (res_length > 255)
	{
		char *tmp = (char *) malloc((size_t) res_length); /* This is a safe cast because res_length <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) res_length);
			return 1;
		}
		initid->ptr = tmp;
	}

	initid->maybe_null = 1;
	initid->max_length = res_length;
	return 0;
}

void str_xor_deinit(UDF_INIT *initid)
{
	if (initid->ptr != NULL)
		free(initid->ptr);
}

/******************************************************************************
** purpose:	exclusive OR (XOR) each byte of the two string arguments.
**					If one string argument is longer than the other, the shorter string
**					is considered to be padded with enough trailing NUL bytes that the
**					arguments would have the same length.
** receives:	pointer to UDF_INIT struct; pointer to UDF_ARGS struct which
**					contains the two string arguments and their lengths;
**					pointer to the result buffer; pointer to ulong that stores the result length;
**					pointer to mem which can be set to 1 if the result is NULL; pointer
**					to mem which can be set to 1 if the calculation resulted in an
**					error
** returns:	the bytewise XOR of the two strings
******************************************************************************/
char *str_xor(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error)
{
	assert(args->arg_count == 2);
	assert(args->arg_type[0] == STRING_RESULT && args->arg_type[1] == STRING_RESULT);
	//assert(args->args[0] != NULL && args->args[1] != NULL);
	if (args->args[0] == NULL || args->args[1] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}


	if (initid->ptr != NULL)
	{
		result = initid->ptr;
	}

	{
		char *__restrict p = result,
				*arg0 = args->args[0],
				*arg1 = args->args[1];
		const unsigned long arg0_length = args->lengths[0],
				arg1_length = args->lengths[1];
		char *const arg0_end = arg0 + arg0_length;
		char *const arg1_end = arg1 + arg1_length;

		if (arg0_length <= arg1_length)
		{
			for (; arg0 != arg0_end; ++arg0, ++arg1)
				*p++ = (*arg0) ^ (*arg1);
			//for (; arg1 != arg1_end; ++arg1)
			//	*p++ = *arg1 /* '\x00' ^ (*arg1) */;
			memcpy(p, arg1, (arg1_end - arg1));

			*res_length = arg1_length;
		}
		else
		{
			for (; arg1 != arg1_end; ++arg0, ++arg1)
				*p++ = (*arg0) ^ (*arg1);
			//for (; arg0 != arg0_end; ++arg0)
			//	*p++ = *arg0 /* (*arg0) ^ '\x00' */;
			memcpy(p, arg0, (arg0_end - arg0));

			*res_length = arg0_length;
		}
	}

	*null_value = 0;
	*error = 0;
	return result;
}

#ifndef __WIN__
typedef struct st_str_srand_data {
	/* Readable file descriptor of /dev/urandom */
	int fd;

	/* If non-NULL, a buffer where the random bytes are stored */
	char *buf;
} st_str_srand_data;
#endif

my_bool str_srand_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	static const char funcname[] = "str_srand_init";
	long long *arg0;

	ARGCOUNTCHECK("non-negative integer");
	ARGTYPECHECK(args->arg_type[0], INT_RESULT, "non-negative integer");

	arg0 = (long long *) args->args[0];
	if (arg0 < 0) {
		snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument type: str_srand requires one non-negative integer argument; argument was negative (%d)", MYSQL_ERRMSG_SIZE, arg0);
		return 1;
	}
	else if (MAX_RANDOM_BYTES < *arg0)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "str_srand is limited to generating at most %lld bytes each execution", (long long) (MAX_RANDOM_BYTES));
		return 1;
	}
	else if (SIZE_MAX < *arg0)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "%lld cannot be greater than SIZE_MAX (%zu)", *arg0, (size_t) (SIZE_MAX));
		return 1;
	}
	else if (ULONG_MAX < *arg0)
	{
		snprintf(message, MYSQL_ERRMSG_SIZE, "%lld cannot be greater than ULONG_MAX (%lu)", *arg0, (unsigned long) (ULONG_MAX));
		return 1;
	}

#ifdef __WIN__
	initid->ptr = NULL;

	if (*arg0 > 255)
	{
		char *tmp = (char *) malloc((size_t) *arg0); /* This is a safe cast because *arg0 <= SIZE_MAX. */
		if (tmp == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) *arg0);
			return 1;
		}
		initid->ptr = tmp;
	}
#else
	{
		st_str_srand_data *p;

		p = (st_str_srand_data *) malloc(sizeof (st_str_srand_data));
		if (p == NULL)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (sizeof (st_str_srand_data)));
			return 1;
		}

		p->buf = NULL;

		if (*arg0 > 255)
		{
			char *tmp = (char *) malloc((size_t) *arg0); /* This is a safe cast because *arg0 <= SIZE_MAX. */
			if (tmp == NULL)
			{
				snprintf(message, MYSQL_ERRMSG_SIZE, "malloc() failed to allocate %zu bytes of memory", (size_t) *arg0);
				free(p);
				return 1;
			}
			p->buf = tmp;
		}

		p->fd = open("/dev/urandom", O_RDONLY);
		if (p->fd == -1)
		{
			snprintf(message, MYSQL_ERRMSG_SIZE, "failed to open /dev/urandom for reading: %s", strerror(errno));
			if (p->buf != NULL)
				free(p->buf);
			free(p);
			return 1;
		}

		initid->ptr = (char *) p;
	}
#endif

	initid->maybe_null = 1;
	initid->max_length = (unsigned long) *arg0; /* This is a safe cast because 0 ≤ *arg0 ≤ ULONG_MAX */
	return 0;
}

void str_srand_deinit(UDF_INIT *initid)
{
#ifdef __WIN__
	if (initid->ptr != NULL)
	{
		free(initid->ptr);
	}
#else
	st_str_srand_data *p = (st_str_srand_data *) initid->ptr;

	close(p->fd);
	if (p->buf != NULL)
		free(p->buf);
	free(p);
#endif
}

char *str_srand(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error)
{
	long long *arg0;

	assert(args->arg_count == 1 && args->arg_type[0] == INT_RESULT);

	if (args->args[0] == NULL) {
		result = NULL;
		*res_length = 0;
		*null_value = 1;
		return result;
	}

	arg0 = (long long *) args->args[0];
	assert(0 <= *arg0 && *arg0 <= MAX_RANDOM_BYTES && *arg0 <= SIZE_MAX && *arg0 <= ULONG_MAX);

	*error = 1;

#ifdef __WIN__
	if (initid->ptr != NULL)
	{
		result = initid->ptr;
	}

	if (TRUE == RtlGenRandom(result, (unsigned long) *arg0))
	{
		*error = 0;
	}
#else
	{
		st_str_srand_data *p = (st_str_srand_data *) initid->ptr;
		ssize_t ss = 0, tmp;

		if (p->buf != NULL)
		{
			result = p->buf;
		}

		do
		{
			tmp = read(p->fd, result + ss, (size_t) (*arg0 - ss));

			if (tmp != -1)
			{
				ss += tmp;
			}
		} while (tmp > 0 && ss < *arg0);

		if (tmp != -1 && ss >= *arg0)
		{
			*error = 0;
		}
	}
#endif

	*res_length = (unsigned long) *arg0; /* This is a safe cast because 0 ≤ *arg0 ≤ ULONG_MAX. */
	*null_value = 0;
	return result;
}

#endif /* HAVE_DLOPEN */
