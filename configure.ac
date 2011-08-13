# initial information about the project
AC_INIT([natools],[0.1],[warlock.cc@gmail.com])

AC_CONFIG_AUX_DIR([build])
AC_DISABLE_STATIC

# automake initialization (mandatory) including a check for automake API version >= 1.9
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_GNU_SOURCE

# add tests for libtool script
LT_INIT

# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
AC_SUBST([LIBTOOL_DEPS])
AC_SUBST(ac_aux_dir)
AC_SUBST([abs_srcdir], ["`cd $srcdir && pwd`"])
AC_SUBST([abs_builddir], ["`pwd`"])

AM_CFLAGS="-Wall -Werror -Wno-uninitialized"
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

# check for C preprocessor and compiler, libtool, install, ...
OLD_CFLAGS=$CFLAGS
AC_PROG_CPP
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
# reset compiler flags to initial flags
CFLAGS=$OLD_CFLAGS

# use the C compiler for the following checks
AC_LANG([C])
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# files to generate via autotools (prepare .am or .in source files)
AC_CONFIG_FILES([
Makefile
libbaas/Makefile
libbaas/test/Makefile
libparser/Makefile
libparser/test/Makefile
libnatools/Makefile
bnatools/Makefile
])

# finally this generates the Makefiles etc. for the build
AC_OUTPUT
