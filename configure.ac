# initial information about the project
AC_INIT([cnash-tools],[0.1],[warlock.cc@gmail.com])

AC_CONFIG_AUX_DIR([build])
AC_DISABLE_STATIC

# automake initialization (mandatory) including a check for automake API version >= 1.9
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# check for C preprocessor and compiler, libtool, install, ...
AC_PROG_LIBTOOL
AC_PROG_INSTALL
# store current user given compiler flags to avoid default setup via AC_PROG_CXX
OLD_CFLAGS=$CXXFLAGS
# check for C++ preprocessor and compiler and the library compiler
# (might change the compiler flags)
AC_PROG_CPP
AC_PROG_CC
# reset compiler flags to initial flags
CFLAGS=$OLD_CXXFLAGS
# set debbugging
AM_CFLAGS="$AM_CFLAGS -g -Wall -Werror -Wno-uninitialized -O0"

# use the C++ compiler for the following checks
AC_LANG([C])
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

# files to generate via autotools (prepare .am or .in source files)
AC_CONFIG_FILES([
Makefile
libbaas/Makefile
libbaas/test/Makefile
libparser/Makefile
libcnash/Makefile
bnatools/Makefile
])

# finally this generates the Makefiles etc. for the build
AC_OUTPUT
