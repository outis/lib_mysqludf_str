use mysql;

drop function if exists lib_mysqludf_str_info;
drop function if exists str_numtowords;
drop function if exists str_rot13;
drop function if exists str_shuffle;
drop function if exists str_translate;
drop function if exists str_ucfirst;
drop function if exists str_ucwords;
drop function if exists str_xor;
drop function if exists str_srand;

create function lib_mysqludf_str_info returns string soname 'lib_mysqludf_str.so';
create function str_numtowords returns string soname 'lib_mysqludf_str.so';
create function str_rot13 returns string soname 'lib_mysqludf_str.so';
create function str_shuffle returns string soname 'lib_mysqludf_str.so';
create function str_translate returns string soname 'lib_mysqludf_str.so';
create function str_ucfirst returns string soname 'lib_mysqludf_str.so';
create function str_ucwords returns string soname 'lib_mysqludf_str.so';
create function str_xor returns string soname 'lib_mysqludf_str.so';
create function str_srand returns string soname 'lib_mysqludf_str.so';
