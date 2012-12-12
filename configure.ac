AC_INIT([natools], [0.15], [warlock.cc@gmail.com])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([config.h])
AC_DISABLE_STATIC
AC_CONFIG_MACRO_DIR([m4])

# automake initialization (mandatory)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_GNU_SOURCE

# Checks for programs.
AM_PROG_AR
AC_PROG_CC([clang gcc cc])
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# let automake know we're overriding flags for custom targets
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([readline], [read_history])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h unistd.h readline/readline.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset pow strchr strstr])

AC_CONFIG_FILES([Makefile
                 tools/Makefile
                 libbaas/Makefile
                 libbaas/test/Makefile
                 libna/Makefile
                 libna/test/Makefile
                 libparser/Makefile
                 libparser/test/Makefile])
AC_OUTPUT
