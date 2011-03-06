/* -*- coding: utf-8; tab-width: 2; indent-tabs-mode: t -*- */
/*
	lib_mysqludf_str - a library of functions to work with strings
	Copyright © 2011  Daniel Trebbien <dtrebbien@gmail.com>
	Copyright © 2007  Claudio Cherubino <claudiocherubino@gmail.com>
	web: http://www.claudiocherubino.it

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

/******************************************************************************
** instructions
******************************************************************************/
/*
	Compile with (adapt the include path to your environment):
	> gcc -Wall -I/usr/include/mysql -shared lib_mysqludf_str.c -olib_mysqludf_str.so

	Install the UDFs by sourcing installdb.sql.
*/

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#define DLLEXP __declspec(dllexport)
#else
#define DLLEXP
#endif

#include <my_global.h>
#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <my_sys.h>
#include <m_string.h>
#endif
#include <mysql.h>
#include <m_ctype.h>
#include <ctype.h>

#include "config.h"

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

#ifdef HAVE_DLOPEN

#define LIBVERSION ("lib_mysqludf_str version " PACKAGE_VERSION)
#define ROT_OFFSET 13

/******************************************************************************
** function declarations
******************************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

DLLEXP
my_bool str_translate_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void str_translate_deinit(UDF_INIT *initid);
DLLEXP
char *str_translate(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error);

DLLEXP
my_bool str_shuffle_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void str_shuffle_deinit(UDF_INIT *initid);
DLLEXP
char *str_shuffle(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error);

DLLEXP
my_bool str_ucwords_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void str_ucfirst_deinit(UDF_INIT *initid);
DLLEXP
char *str_ucfirst(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error);

DLLEXP
my_bool str_ucwords_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void str_ucwords_deinit(UDF_INIT *initid);
DLLEXP
char *str_ucwords(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error);

DLLEXP
my_bool str_rot13_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void str_rot13_deinit(UDF_INIT *initid);
DLLEXP
char *str_rot13(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error);

DLLEXP
my_bool str_xor_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void str_xor_deinit(UDF_INIT *initid);
DLLEXP
char *str_xor(UDF_INIT *initid, UDF_ARGS *args, char *__restrict result,
		unsigned long *res_length, char *null_value, char *error);

DLLEXP
my_bool lib_mysqludf_str_info_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
DLLEXP
void lib_mysqludf_str_info_deinit(UDF_INIT *initid);
DLLEXP
char *lib_mysqludf_str_info(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *res_length, char *null_value, char *error);

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
	my_bool status;
	if (args->arg_count != 0)
	{
		strcpy(message,	"No arguments allowed (udf: lib_mysqludf_str_info)");
		status = 1;
	}
	else
	{
		status = 0;
	}
	return status;
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
	strcpy(result,LIBVERSION);
	*res_length = strlen(LIBVERSION);
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
	/* make sure user has provided exactly one integer argument */
	if (args->arg_count != 1 || args->arg_type[0] != INT_RESULT || args->args[0] == NULL)
	{
		strcpy(message,"str_numtowords requires one integer argument");
		return 1;
	}

	/* str_numtowords() will not be returning null */
	initid->maybe_null=0;

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
void str_numtowords_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
}

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
	static const char *const powers[] = {"thousand", "million", "billion"};

	static const char *const ones[] = {"one", "two", "three", "four", "five",
																"six", "seven", "eight", "nine", "ten",
																"eleven", "twelve", "thirteen", "fourteen", "fifteen",
																"sixteen", "seventeen", "eighteen", "nineteen"};

	static const char *const tens[] = {"twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};

	char *cp = result;

	// value will contain the user-supplied argument, cast as integer
	int value = *((long long*) args->args[0]);

	int part_stack[4];
	int *part_ptr = part_stack;

	// check for negative values or zero
	if (value < 0)
	{
		cp += sprintf (cp, "negative ");
		value = -value;
	}
	else if (value == 0)
	{
		strcpy (result, "zero");
		*res_length = strlen(result);
		return result;
	}

	// splitting the number into four parts
	for (; value; value /= 1000)
		*part_ptr++ = value % 1000;

	while (part_ptr > part_stack)
	{
		int p = *--part_ptr;

		if (p >= 100)
		{
			cp += sprintf (cp, "%s hundred ", ones[p / 100 - 1]);
			p %= 100;
		}

		if (p >= 20)
		{
			if (p % 10)
				cp += sprintf (cp, "%s-%s ", tens[p / 10 - 2], ones[p % 10 - 1]);
			else
				cp += sprintf (cp, "%s ", tens[p / 10 - 2]);
		}
		else if (p > 0)
			cp += sprintf (cp, "%s ", ones[p - 1]);

		if (p && part_ptr > part_stack)
			cp += sprintf (cp, "%s ", powers[part_ptr - part_stack - 1]);
	}

	cp[-1] = 0;
	*res_length = strlen(result);
	return result;
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
	/* make sure user has provided exactly one string argument */
	if (args->arg_count != 1 || args->arg_type[0] != STRING_RESULT || args->args[0] == NULL)
	{
		strcpy(message,"str_rot13 requires one string argument");
		return 1;
	}

	/* str_rot13() will not be returning null */
	initid->maybe_null=0;

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
void str_rot13_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
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

	// s will contain the user-supplied argument
	const char *s = args->args[0];

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
	/* make sure user has provided exactly one string argument */
	if (args->arg_count != 1 || args->arg_type[0] != STRING_RESULT || args->args[0] == NULL)
	{
		strcpy(message,"str_shuffle requires one string argument");
		return 1;
	}

	/* str_shuffle() will not be returning null */
	initid->maybe_null=0;

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

	// copy the argument string into result
	strncpy(result, args->args[0], args->lengths[0]);
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
	/* make sure user has provided exactly three string arguments */
	if (args->arg_count != 3 || args->arg_type[0] != STRING_RESULT || args->arg_type[1] != STRING_RESULT || args->arg_type[2] != STRING_RESULT ||
		args->args[0] == NULL || args->args[1] == NULL || args->args[2] == NULL)
	{
		strcpy(message,"str_translate requires three string arguments (subject, srcchar, dstchar)");
		return 1;
	}
	else if (strlen(args->args[1]) != strlen(args->args[2]))
	{
		strcpy(message,"str_translate(subject, srcchar, dstchar) requires srcchar and dstchar to have the same length");
		return 1;
	}

	/* str_translate() will not be returning null */
	initid->maybe_null=0;

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
void str_translate_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
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

	// subject will contain the string to be translated
	const char *subject = args->args[0];

	// src will contain the string to be translated
	const char *src = args->args[1];

	// dst will contain the string to be translated
	const char *dst = args->args[2];

	*res_length = args->lengths[0];

	for (i = 0; i < *res_length; i++)
	{
		// suppose that the character will not be changed
		result[i] = subject[i];

		// for each character of the subject string we check if it is present in src
		for (j = 0; j < strlen(src); j++)
		{
			if (subject[i] == src[j])
			{
				// replace the character with the corresponding one from dst
				result[i] = dst[j];
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
	/* make sure user has provided exactly one string argument */
	if (args->arg_count != 1 || args->arg_type[0] != STRING_RESULT || args->args[0] == NULL)
	{
		strcpy(message,"str_ucfirst requires one string argument");
		return 1;
	}

	/* str_ucfirst() will not be returning null */
	initid->maybe_null=0;

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
	// copy the argument string into result
	strncpy(result, args->args[0], args->lengths[0]);

	*res_length = args->lengths[0];

	// capitalize the first character of the string
	*result = my_toupper(&my_charset_latin1, *result);

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
	/* make sure user has provided exactly one string argument */
	if (args->arg_count != 1 || args->arg_type[0] != STRING_RESULT || args->args[0] == NULL)
	{
		strcpy(message,"str_ucwords requires one string argument");
		return 1;
	}

	/* str_ucwords() will not be returning null */
	initid->maybe_null=0;

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
void str_ucwords_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
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

	// copy the argument string into result
	strncpy(result, args->args[0], args->lengths[0]);

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
	if (args->arg_count != 2 || args->arg_type[0] != STRING_RESULT ||
			args->arg_type[1] != STRING_RESULT || args->args[0] == NULL ||
			args->args[1] == NULL)
	{
		strncpy(message, "str_xor requires exactly two non-NULL string arguments", MYSQL_ERRMSG_SIZE);
		return 1;
	}

	initid->maybe_null = 0;
	initid->max_length = args->lengths[0] >= args->lengths[1] ? args->lengths[0] : args->lengths[1];
	return 0;
}

void str_xor_deinit(UDF_INIT *initid ATTRIBUTE_UNUSED)
{
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
char *str_xor(UDF_INIT *initid, UDF_ARGS *args, char *__restrict result,
		unsigned long *res_length, char *null_value, char *error)
{
	assert(args->arg_count == 2);
	assert(args->arg_type[0] == STRING_RESULT && args->arg_type[1] == STRING_RESULT);
	assert(args->args[0] != NULL && args->args[1] != NULL);

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

#endif /* HAVE_DLOPEN */
