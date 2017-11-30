#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "php_screwhook.h"

ZEND_DECLARE_MODULE_GLOBALS(screwhook)

PHP_MINIT_FUNCTION(screwhook);
PHP_MSHUTDOWN_FUNCTION(screwhook);
PHP_MINFO_FUNCTION(screwhook);

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle * file_handle, int type TSRMLS_DC);

PHP_FUNCTION(screwhook_dummy);

const zend_function_entry screwhook_functions[] = {
	PHP_FE(screwhook_dummy, NULL)
	{NULL, NULL, NULL}
};

static void php_screwhook_init_globals(zend_screwhook_globals *screwhook_globals) {
	screwhook_globals->enabled = 0;
}

int checkScrewFile(char *buf, int len)
{
	const char sig_screwim[] = "\tSCREWIM DATA\t";
	int sig_screwim_len = 14;
	const char sig_pm9screw[] = "\tPM9SCREW\t";
	int sig_pm9screw_len = 10;
	if(len >= sig_pm9screw_len)
	{
		if(memcmp(buf, sig_pm9screw, sig_pm9screw_len) == 0)
			return 1;
	}
	if(len >= sig_screwim_len)
	{
		if(memcmp(buf, sig_screwim, sig_screwim_len) == 0)
			return 1;
	}
	
	return 0;
}


ZEND_API zend_op_array *screwhook_compile_file(zend_file_handle * file_handle, int type TSRMLS_DC)
{
	do {
		FILE *fp;
		int len;
		char sigbuf[14];
		char filepathbuf[512];
		if (file_handle->filename == NULL)
			break;
		sprintf(filepathbuf, "/tmp/%s.dec", basename(file_handle->filename));
		fp = fopen(file_handle->filename, "rb");
		if (!fp)
			break;
		len = fread(sigbuf, 1, sizeof(sigbuf), fp);
		fclose(fp);
		if (!checkScrewFile(sigbuf, len))
			break;

		fp = fopen(filepathbuf, "wb");
		if (!fp)
			break;

		if (file_handle->handle.stream.mmap.buf)
		{
			fwrite(file_handle->handle.stream.mmap.buf, 1, file_handle->handle.stream.mmap.len, fp);
		}

		fclose(fp);
	} while (0);
	
	return org_compile_file (file_handle, type TSRMLS_CC);
}

PHP_FUNCTION(screwhook_dummy) {

}

zend_module_entry screwhook_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"screwhook",
	screwhook_functions,
	PHP_MINIT(screwhook),
	PHP_MSHUTDOWN(screwhook),
	NULL,
	NULL,
	PHP_MINFO(screwhook),
#if ZEND_MODULE_API_NO >= 20010901
	"1.0.0", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SCREWHOOK
ZEND_GET_MODULE(screwhook);
#endif

PHP_INI_BEGIN()
STD_PHP_INI_BOOLEAN("screwhook.enable", "0", PHP_INI_ALL, OnUpdateBool, enabled, zend_screwhook_globals, screwhook_globals)
PHP_INI_END()


PHP_MINIT_FUNCTION(screwhook)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	ZEND_INIT_MODULE_GLOBALS(screwhook, php_screwhook_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	org_compile_file = zend_compile_file;
	zend_compile_file = screwhook_compile_file;

	{
		FILE *fp = fopen("/tmp/screwhook.test", "w");
		if (fp)
			fclose(fp);
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(screwhook)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	zend_compile_file = org_compile_file;
	return SUCCESS;
}

PHP_MINFO_FUNCTION(screwhook)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "php_screwhook support", "enabled");
	php_info_print_table_end();
}
