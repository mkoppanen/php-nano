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

#include "php_nano.h"
#include "php_nano_private.h"

static
    zend_class_entry *php_nano_sc_entry,
                     *php_nano_socket_sc_entry,
                     *php_nano_exception_sc_entry;

static
    zend_object_handlers nano_object_handlers,
                         nano_socket_object_handlers;

/* {{{ proto void NanoMsg\Nano::__construct()
    Private constructor
*/
PHP_METHOD(nano, __construct)
{
}
/* }}} */

static
void s_symbol_info_to_zval (zval *zv, struct nn_symbol_properties *buffer TSRMLS_DC)
{
    /**
     * remove by recoye
    object_init (zv);
    */
    zend_update_property_string (NULL, zv, "name", sizeof ("name") - 1, buffer->name TSRMLS_CC);
    zend_update_property_long (NULL, zv, "value", sizeof ("value") - 1, (long) buffer->value TSRMLS_CC);
    zend_update_property_long (NULL, zv, "ns", sizeof ("ns") - 1, (long) buffer->ns TSRMLS_CC);
    zend_update_property_long (NULL, zv, "type", sizeof ("type") - 1, (long) buffer->type TSRMLS_CC);
    zend_update_property_long (NULL, zv, "unit", sizeof ("unit") - 1, (long) buffer->unit TSRMLS_CC);
}

/* {{{ proto void NanoMsg\Nano::symbolInfo([integer symbol])
    Symbol info
*/
PHP_METHOD(nano, symbolinfo)
{
    int i = 0, rc = 0;
    zval *symbol = NULL;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "|z!/", &symbol) == FAILURE) {
        return;
    }

    if (symbol) {
        struct nn_symbol_properties buffer;

        convert_to_long (symbol);
        rc = nn_symbol_info (Z_LVAL_P(symbol), &buffer, sizeof (buffer));

        if (!rc) {
            zend_throw_exception_ex (php_nano_exception_sc_entry, 0 TSRMLS_CC, "Symbol out of range");
            return;
        }

        object_init (return_value);
        s_symbol_info_to_zval (return_value, &buffer TSRMLS_CC);
        return;
    }

    array_init (return_value);

    while (1) {
#if PHP_MAJOR_VERSION >= 7
        zval zv;
#else
        zval *zv;
#endif
        struct nn_symbol_properties buffer;
        rc = nn_symbol_info (i, &buffer, sizeof (buffer));

        if (!rc) {
            break;
        }

#if PHP_MAJOR_VERSION >= 7
        ZVAL_UNDEF(&zv);
        object_init (&zv);

        s_symbol_info_to_zval (&zv, &buffer TSRMLS_CC);
        add_next_index_zval (return_value, &zv);
        ZVAL_UNDEF(&zv);
#else
        MAKE_STD_ZVAL (zv);
        object_init (zv);

        s_symbol_info_to_zval (zv, &buffer TSRMLS_CC);
        add_next_index_zval (return_value, zv);
#endif
        ++i;
    }
    return;
}
/* }}} */

/* {{{ proto void NanoMsg\Nano::device(int s1, int s2)
    Runs a device
*/
PHP_METHOD(nano, device)
{
    int rc;
    long s1, s2;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "ll", &s1, &s2) == FAILURE) {
        return;
    }

    rc = nn_device (s1, s2);

    if (rc < 0)
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error creating nano device: %s", nn_strerror (errno));
}
/* }}} */

/* {{{ proto void NanoMsg\Socket::__construct(int $domain, int $protocol)
    Construct a new nano socket
*/
PHP_METHOD(socket, __construct)
{
    php_nano_socket_object *intern;
    long domain, protocol;
    int rc;

    PHP_NANO_ERROR_HANDLING_INIT()
    PHP_NANO_ERROR_HANDLING_THROW()

    rc = zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "ll", &domain, &protocol);

    PHP_NANO_ERROR_HANDLING_RESTORE()

    if (rc == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern    = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif
    intern->s = nn_socket (domain, protocol);

    if (intern->s < 0)
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error creating nano socket: %s", nn_strerror (errno));
}
/* }}} */

/* {{{ proto int NanoMsg\Socket::bind(string $endpoint)
    Bind the socket to an endpoint
*/
PHP_METHOD(socket, bind)
{
    php_nano_socket_object *intern;
    char *endpoint;
#if PHP_MAJOR_VERSION >= 7
    size_t endpoint_len;
#else
    int endpoint_len;
#endif
    int eid;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s", &endpoint, &endpoint_len) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif

    eid = nn_bind (intern->s, endpoint);

    if (eid < 0) {
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error binding nano socket: %s", nn_strerror (errno));
        return;
    }
    // Return the endpoint ID
    RETURN_LONG (eid);
}
/* }}} */

/* {{{ proto int NanoMsg\Socket::connect(string $endpoint)
    Connect the socket to an endpoint
*/
PHP_METHOD(socket, connect)
{
    php_nano_socket_object *intern;
    char *endpoint;
#if PHP_MAJOR_VERSION >= 7
    size_t endpoint_len;
#else
    int endpoint_len;
#endif
    int eid;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s", &endpoint, &endpoint_len) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif
    eid = nn_connect (intern->s, endpoint);

    if (eid < 0) {
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error connecting nano socket: %s", nn_strerror (errno));
        return;
    }
    // Return the endpoint ID
    RETURN_LONG (eid);
}
/* }}} */

/* {{{ proto boolean NanoMsg\Socket::shutdown(int $endpoint_id)
    Remove an endpoint from a socket
*/
PHP_METHOD(socket, shutdown)
{
    php_nano_socket_object *intern;
    long eid, rc;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "l", &eid) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif
    rc = nn_shutdown (intern->s, eid);

    if (rc < 0) {
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error removing endpoint: %s", nn_strerror (errno));
        return;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean NanoMsg\Socket::send(string $message[, int $flags])
    send a message
*/
PHP_METHOD(socket, send)
{
    php_nano_socket_object *intern;
    char *message;
#if PHP_MAJOR_VERSION >= 7
    int rc;
    size_t message_len;
#else
    int rc, message_len;
#endif
    long flags = 0;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s|l", &message, &message_len, &flags) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif
    rc = nn_send (intern->s, message, message_len, flags);

    if (rc < 0) {
        if (flags & NN_DONTWAIT && errno == EAGAIN) {
            RETURN_FALSE;
        }
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error sending message: %s", nn_strerror (errno));
        return;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto string NanoMsg\Socket::recv([int $flags])
    receive a message
*/
PHP_METHOD(socket, recv)
{
    php_nano_socket_object *intern;
    void *buffer;
    int len;
    long flags = 0;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "|l", &flags) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif
    len = nn_recv (intern->s, &buffer, NN_MSG, flags);

    if (len < 0) {
        if ((flags & NN_DONTWAIT) && errno == EAGAIN) {
            RETURN_FALSE;
        }
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Error receiving message: %s", nn_strerror (errno));
        return;
    }

    // Create return value
#if PHP_MAJOR_VERSION >= 7
    ZVAL_STRINGL (return_value, buffer, len);
#else
    ZVAL_STRINGL (return_value, buffer, len, 1);
#endif

    nn_freemsg (buffer);
    return;
}
/* }}} */

/* {{{ proto int NanoMsg\Socket::setSockOpt(int $level, int $option, mixed $value)
    set a socket option
*/
PHP_METHOD(socket, setsockopt)
{
    php_nano_socket_object *intern;
    long option, level;
    zval *value;
    int rc = 0;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "llz/", &level, &option, &value) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif

    // Handle string options 
    if (level == NN_SUB && (option == NN_SUB_SUBSCRIBE || option == NN_SUB_UNSUBSCRIBE)) {
        // Add topic
        convert_to_string (value);

        if (Z_STRVAL_P (value)) {
            rc = nn_setsockopt (intern->s, level, option, Z_STRVAL_P (value), Z_STRLEN_P (value));
        }
    }
    else {
        int v = Z_LVAL_P (value);
        rc = nn_setsockopt (intern->s, level, option, &v, sizeof (int));
    }
    if (rc < 0) {
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Failed to set socket option: %s", nn_strerror (errno));
        return;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed NanoMsg\Socket::getSockOpt(int $level, int $option)
    send a message
*/
PHP_METHOD(socket, getsockopt)
{
    php_nano_socket_object *intern;
    long option, level;
    int v, rc;
    size_t sz;

    if (zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "ll", &level, &option) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    intern = Z_NANO_P(getThis());
#else
    intern = (php_nano_socket_object *) zend_object_store_get_object (getThis () TSRMLS_CC);
#endif

    sz = sizeof (v);
    rc = nn_getsockopt (intern->s, level, option, &v, &sz);

    if (rc < 0) {
        zend_throw_exception_ex (php_nano_exception_sc_entry, errno TSRMLS_CC, "Failed to get socket option: %s", nn_strerror (errno));
        return;
    }

    // Wrap this as a socket
    if (level == NN_SOL_SOCKET && (option == NN_SNDFD || option == NN_RCVFD)) {
        // Create PHP socket resource
        php_stream *stream = php_stream_sock_open_from_socket (v, 0);
        if (!stream) {
            zend_throw_exception_ex (php_nano_exception_sc_entry, 0 TSRMLS_CC, "Failed to convert the handle to PHP stream");
            return;
        }
        stream->flags |= PHP_STREAM_FLAG_NO_CLOSE;
        php_stream_to_zval (stream, return_value);
        return;
    } else {
        RETURN_LONG(v);
    }
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(nano_construct_args, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_symbolinfo_args, 0, 0, 0)
    ZEND_ARG_INFO(0, symbol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_device_args, 0, 0, 2)
    ZEND_ARG_INFO(0, s1)
    ZEND_ARG_INFO(0, s2)
ZEND_END_ARG_INFO()

static zend_function_entry php_nano_class_methods [] = {
    PHP_ME (nano,    __construct, nano_construct_args,  ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
    PHP_ME (nano,    symbolinfo,  nano_symbolinfo_args, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME (nano,    device,      nano_device_args,     ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};

ZEND_BEGIN_ARG_INFO_EX(nano_socket_construct_args, 0, 0, 2)
    ZEND_ARG_INFO(0, domain)
    ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_bind_args, 0, 0, 1)
    ZEND_ARG_INFO(0, endpoint)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_connect_args, 0, 0, 1)
    ZEND_ARG_INFO(0, endpoint)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_shutdown_args, 0, 0, 1)
    ZEND_ARG_INFO(0, endpoint_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_send_args, 0, 0, 1)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_recv_args, 0, 0, 0)
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_setsockopt_args, 0, 0, 3)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, option)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nano_socket_getsockopt_args, 0, 0, 3)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

static zend_function_entry php_nano_socket_class_methods [] = {
    PHP_ME (socket, __construct, nano_socket_construct_args,  ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME (socket, bind,        nano_socket_bind_args,       ZEND_ACC_PUBLIC)
    PHP_ME (socket, connect,     nano_socket_connect_args,    ZEND_ACC_PUBLIC)
    PHP_ME (socket, shutdown,    nano_socket_shutdown_args,   ZEND_ACC_PUBLIC)
    PHP_ME (socket, send,        nano_socket_send_args,       ZEND_ACC_PUBLIC)
    PHP_ME (socket, recv,        nano_socket_recv_args,       ZEND_ACC_PUBLIC)
    PHP_ME (socket, setsockopt,  nano_socket_setsockopt_args, ZEND_ACC_PUBLIC)
    PHP_ME (socket, getsockopt,  nano_socket_getsockopt_args, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry nano_functions[] = {
    {NULL, NULL, NULL}
};

static
void s_register_constants (TSRMLS_D)
{
#define PHP_NANO_REGISTER_CONST_LONG(const_name, value) \
    zend_declare_class_constant_long (php_nano_sc_entry, const_name, strlen (const_name), (long) value TSRMLS_CC);

    // Register the symbols
    int value, i;
    for (i = 0; ; ++i) {
        const char *name = nn_symbol (i, &value);
        if (name == NULL)
            break;

        PHP_NANO_REGISTER_CONST_LONG (name, value);
    }
#undef PHP_NANO_REGISTER_CONST_LONG
}

static
void s_nano_socket_object_free_storage (void *object TSRMLS_DC)
{
    php_nano_socket_object *intern = (php_nano_socket_object *) object;

    if (!intern) {
        return;
    }

    if (intern->s >= 0) {
        int rc = nn_close (intern->s);
        // TODO: error checking
    }

    zend_object_std_dtor (&intern->zo TSRMLS_CC);
    efree (intern);
}

/* PHP 5.4 */
#if PHP_VERSION_ID < 50399
# define object_properties_init(zo, class_type) { \
            zval *tmp; \
            zend_hash_copy((*zo).properties, \
                            &class_type->default_properties, \
                            (copy_ctor_func_t) zval_add_ref, \
                            (void *) &tmp, \
                            sizeof(zval *)); \
         }
#endif

static
#if PHP_MAJOR_VERSION >= 7
zend_object* s_nano_socket_object_new_ex (zend_class_entry *class_type, php_nano_socket_object **ptr TSRMLS_DC)
#else
zend_object_value s_nano_socket_object_new_ex (zend_class_entry *class_type, php_nano_socket_object **ptr TSRMLS_DC)
#endif
{
#if PHP_MAJOR_VERSION < 7
    zval *tmp;
    zend_object_value retval;
#endif
    php_nano_socket_object *intern;

    /* Allocate memory for it */
    intern = (php_nano_socket_object *) emalloc (sizeof (php_nano_socket_object));
    memset (&intern->zo, 0, sizeof (zend_object));

    intern->s = -1;

    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init (&intern->zo, class_type TSRMLS_CC);
    object_properties_init (&intern->zo, class_type);

#if PHP_MAJOR_VERSION >= 7
	intern->zo.handlers = &nano_socket_object_handlers;

	return &intern->zo;
#else
    retval.handle = zend_objects_store_put (intern, NULL, (zend_objects_free_object_storage_t) s_nano_socket_object_free_storage, NULL TSRMLS_CC);
    retval.handlers = (zend_object_handlers *) &nano_socket_object_handlers;
    return retval;
#endif
}

static
#if PHP_MAJOR_VERSION >= 7
zend_object* s_nano_socket_object_new (zend_class_entry *class_type TSRMLS_DC)
#else
zend_object_value s_nano_socket_object_new (zend_class_entry *class_type TSRMLS_DC)
#endif
{
    return
        s_nano_socket_object_new_ex (class_type, NULL TSRMLS_CC);
}

PHP_MINIT_FUNCTION(nano)
{
    zend_class_entry ce, ce_socket, ce_exception;

    // Register nanomsg class and the exception class
    memcpy (&nano_object_handlers, zend_get_std_object_handlers (), sizeof (zend_object_handlers));
    memcpy (&nano_socket_object_handlers, zend_get_std_object_handlers (), sizeof (zend_object_handlers));

    INIT_NS_CLASS_ENTRY (ce, "NanoMsg", "Nano", php_nano_class_methods);
    ce.create_object = NULL;
    nano_object_handlers.clone_obj = NULL;
    php_nano_sc_entry = zend_register_internal_class (&ce TSRMLS_CC);

    INIT_NS_CLASS_ENTRY (ce_socket, "NanoMsg", "Socket", php_nano_socket_class_methods);
    ce_socket.create_object = s_nano_socket_object_new;
    nano_socket_object_handlers.clone_obj = NULL;
    php_nano_socket_sc_entry = zend_register_internal_class (&ce_socket TSRMLS_CC);

    INIT_NS_CLASS_ENTRY (ce_exception, "NanoMsg", "Exception", NULL);

#if PHP_MAJOR_VERSION >= 7
    php_nano_exception_sc_entry = zend_register_internal_class_ex (&ce_exception, zend_exception_get_default (TSRMLS_C) TSRMLS_CC);
    php_nano_exception_sc_entry->ce_flags &= ~ZEND_ACC_FINAL;
#else
    php_nano_exception_sc_entry = zend_register_internal_class_ex (&ce_exception, zend_exception_get_default (TSRMLS_C), NULL TSRMLS_CC);
    php_nano_exception_sc_entry->ce_flags &= ~ZEND_ACC_FINAL_CLASS;
#endif

    // Register all symbols as class constants
    s_register_constants (TSRMLS_C);
    return SUCCESS;
}


PHP_MINFO_FUNCTION(nano)
{
    php_info_print_table_start();
        php_info_print_table_header(2, "nanomsg extension", "enabled");
        php_info_print_table_row(2, "PHP extension version", PHP_NANO_EXTVER);
    php_info_print_table_end();
}

zend_module_entry nano_module_entry =
{
    STANDARD_MODULE_HEADER,
    PHP_NANO_EXTNAME,
    nano_functions,         /* Functions */
    PHP_MINIT(nano),        /* MINIT */
    NULL,                   /* MSHUTDOWN */
    NULL,                   /* RINIT */
    NULL,                   /* RSHUTDOWN */
    PHP_MINFO(nano),        /* MINFO */
    PHP_NANO_EXTVER,        /* version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_NANO
ZEND_GET_MODULE(nano)
#endif /* COMPILE_DL_NANO */
