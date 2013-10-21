PHP_ARG_WITH(nano,     whether to enable nanomsg support,
[  --with-nano[=DIR]   Enable nanomsg support. DIR is the prefix to libnanomsg installation directory.], yes)

if test "$PHP_NANO" != "no"; then

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  if test "x$PKG_CONFIG" = "xno"; then
    AC_MSG_RESULT([pkg-config not found])
    AC_MSG_ERROR([Please reinstall the pkg-config distribution])
  fi

  ORIG_PKG_CONFIG_PATH=$PKG_CONFIG_PATH

  AC_MSG_CHECKING(nanomsg installation)
  if test "x$PHP_NANO" = "xyes"; then
    if test "x${PKG_CONFIG_PATH}" != "x"; then
      export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:/usr/lib/pkgconfig:/usr/local/lib/pkgconfig:/opt/lib/pkgconfig:/opt/local/lib/pkgconfig"
    else
      export PKG_CONFIG_PATH="/usr/lib/pkgconfig:/usr/local/lib/pkgconfig:/opt/lib/pkgconfig:/opt/local/lib/pkgconfig"
    fi
  else
    export PKG_CONFIG_PATH="$PHP_NANO:$PHP_NANO/lib/pkgconfig"
  fi

  if $PKG_CONFIG --exists nanomsg; then
    PHP_NANO_VERSION=`$PKG_CONFIG nanomsg --modversion`
    PHP_NANO_PREFIX=`$PKG_CONFIG nanomsg --variable=prefix`

    AC_MSG_RESULT([found version $PHP_NANO_VERSION, under $PHP_NANO_PREFIX])
    PHP_NANO_LIBS=`$PKG_CONFIG nanomsg --libs`
    PHP_NANO_INCS=`$PKG_CONFIG nanomsg --cflags`

    PHP_EVAL_LIBLINE($PHP_NANO_LIBS, NANO_SHARED_LIBADD)
    PHP_EVAL_INCLINE($PHP_NANO_INCS)
  else
    AC_MSG_ERROR(Unable to find usable nanomsg installation)
  fi

  PHP_ADD_EXTENSION_DEP(nano, spl)

  PHP_SUBST(NANO_SHARED_LIBADD)
  PHP_NEW_EXTENSION(nano, nano.c, $ext_shared)
  PKG_CONFIG_PATH="$ORIG_PKG_CONFIG_PATH"
fi

