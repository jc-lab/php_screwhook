dnl config.m4 for extension mod_screwhook

PHP_ARG_WITH(screwhook, for screw decrypt support by hooking,
dnl Make sure that the comment is aligned:
[  --with-screwhook          Include screw hook support])

PHP_NEW_EXTENSION(screwhook, php_screwhook.c, $ext_shared)


