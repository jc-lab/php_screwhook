#ifndef __MOD_SCREWHOOK_H__
#define __MOD_SCREWHOOK_H__

PHP_MINIT_FUNCTION(screwhook);
PHP_MSHUTDOWN_FUNCTION(screwhook);

ZEND_BEGIN_MODULE_GLOBALS(screwhook)
	zend_bool enabled;
ZEND_END_MODULE_GLOBALS(screwhook)


#endif /* __MOD_SCREWHOOK_H__ */

