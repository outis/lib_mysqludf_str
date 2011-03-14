This is a build of version 0.2 of lib_mysqludf_str for MySQL 5.1+ and 32-bit
Windows XP SP3 or later. It is licensed under the terms of the LGPL
version 2.1 or later, the exact text of which is located in COPYING.

All source code for this project is downloadable from GitHub:
https://github.com/dtrebbien/lib_mysqludf_str


INSTALLATION
================================================================================
To install the lib_mysqludf_str UDFs:

 1. Make sure that you are running 32-bit Windows and that you have installed
    the 32-bit build of MySQL 5.1 or later.

    Note: The lib_mysqludf_str.dll binary has only been tested with MySQL versions
    5.5 and 5.1, but should work with MySQL 5.0 as well. It is, however,
    incompatible with MySQL 3.23/4.0/4.1.

 2. Look for a file named msvcr100.dll in your System32 folder. If this file is
    not present, then install the Microsoft Visual C++ 2010 Redistributable Package (x86):
    https://www.microsoft.com/downloads/en/details.aspx?FamilyID=a7b7a05e-6de6-4d3a-a423-37bf0912db84

 3. Copy lib_mysqludf_str.dll to the MySQL installation's lib\plugin folder.

    With a default installation, this is C:\Program Files\MySQL\MySQL Server 5.5\lib\plugin

    Note: You do NOT need to re-start MySQL after copying lib_mysqludf_str.dll
    into the plugin directory.

 4. As root, source installdb.sql or simply paste the contents into the CLI client.

 5. Test the setup by executing:
    SELECT lib_mysqludf_str_info() FROM DUAL;

    The result should be:
    +------------------------------+
    | lib_mysqludf_str_info()      |
    +------------------------------+
    | lib_mysqludf_str version 0.2 |
    +------------------------------+


TROUBLESHOOTING
================================================================================
  * ERROR 1126 (HY000): Can't open shared library 'lib_mysqludf_str' (errno: 0 )

    This either means that the Microsoft Visual C++ 2010 Redistributable Package
    is not installed or that MySQL could not find lib_mysqludf_str.dll in the
    plugin directory.

    Look for a file named msvcr100.dll in your System32 folder. If this file is
    not present, then install the latest Microsoft Visual C++ 2010 Redistributable Package (x86).

    Verify that lib_mysqludf_str.dll was copied into the plugin directory by
    executing:
    SHOW VARIABLES LIKE 'plugin_dir';

    The result should be:
    +---------------+----------------------------------------------------+
    | Variable_name | Value                                              |
    +---------------+----------------------------------------------------+
    | plugin_dir    | C:\Program Files\MySQL\MySQL Server 5.5\lib/plugin |
    +---------------+----------------------------------------------------+

    If it's not, then copy lib_mysqludf_str.dll to the listed directory.

  * ERROR 1127 (HY000): Can't find symbol in library

    This means that MySQL was able to find lib_mysqludf_str.dll, but for some
    reason, it could not "see" the UDF in the DLL module.

    If you are typing the CREATE FUNCTION statements manually, make sure that
    you have typed the UDF name exactly as listed in installdb.sql. For example,
    instead of "STR_XOR", you must type "str_xor" (all lowercase).


If you encounter any other problem, please feel free to contact me at:
    dtrebbien@gmail.com
