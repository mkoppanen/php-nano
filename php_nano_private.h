/*
+-----------------------------------------------------------------------------------+
|  nanomsg extension for PHP                                                        |
|  Copyright (c) 2014, Mikko Koppanen <mkoppanen@php.net>                           |
|  All rights reserved.                                                             |
+-----------------------------------------------------------------------------------+
|  Redistribution and use in source and binary forms, with or without               |
|  modification, are permitted provided that the following conditions are met:      |
|     * Redistributions of source code must retain the above copyright              |
|       notice, this list of conditions and the following disclaimer.               |
|     * Redistributions in binary form must reproduce the above copyright           |
|       notice, this list of conditions and the following disclaimer in the         |
|       documentation and/or other materials provided with the distribution.        |
|     * Neither the name of the copyright holder nor the                            |
|       names of its contributors may be used to endorse or promote products        |
|       derived from this software without specific prior written permission.       |
+-----------------------------------------------------------------------------------+
|  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  | 
|  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    |   
|  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           |
|  DISCLAIMED. IN NO EVENT SHALL MIKKO KOPPANEN OR CONTRIBUTORS BE LIABLE FOR       |
|  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES   |   
|  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     |
|  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      |
|  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       |
|  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    |   
|  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     |
+-----------------------------------------------------------------------------------+
*/

#ifndef _PHP_NANO_PRIVATE_H_
# define _PHP_NANO_PRIVATE_H_

/*
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
*/
#include <nn.h>
#include <pubsub.h>

#include "main/php_network.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/info.h"

/*
  Definition for the main class (holds constants)
*/
typedef struct {
    zend_object zo;
} php_nano_object;

/*
  Definition for the socket object
*/
typedef struct {
    zend_object zo;
    int s;
    HashTable eid;
} php_nano_socket_object;

#if ZEND_MODULE_API_NO > 20060613

#define PHP_NANO_ERROR_HANDLING_INIT() zend_error_handling error_handling;

#define PHP_NANO_ERROR_HANDLING_THROW() zend_replace_error_handling(EH_THROW, php_nano_exception_sc_entry, &error_handling TSRMLS_CC);

#define PHP_NANO_ERROR_HANDLING_RESTORE() zend_restore_error_handling(&error_handling TSRMLS_CC);

#else

#define PHP_NANO_ERROR_HANDLING_INIT()

#define PHP_NANO_ERROR_HANDLING_THROW() php_set_error_handling(EH_THROW, php_nano_exception_sc_entry TSRMLS_CC);

#define PHP_NANO_ERROR_HANDLING_RESTORE() php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);

#endif

#if PHP_MAJOR_VERSION >= 7
static inline php_nano_socket_object *php_nano_fetch_object(zend_object *obj) {
	return (php_nano_socket_object *)((char*)(obj) - XtOffsetOf(php_nano_socket_object, zo));
}

#define Z_NANO_P(zv) php_nano_fetch_object(Z_OBJ_P((zv)))

#endif

#endif /* _PHP_NANO_PRIVATE_H_ */
