AC_PREREQ([2.65])
AC_INIT([basic], [Basic Cartesian Interfaces], [0.0])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_INIT_AUTOMAKE
AC_PROG_CXX

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

AC_LANG([C++])

# check for the correct version of gcc
AC_MSG_CHECKING([for gcc >= 4.5.0])
GCC_CHECK_TEXT=`cat $srcdir/checks/version.cpp`
AC_COMPILE_IFELSE($GCC_CHECK_TEXT, 
[AC_MSG_RESULT([yes])],
[AC_MSG_ERROR([gcc version 4.5.0 or greater is required])]
)
# Checks for libraries

AX_BOOST_BASE([1.43])
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_THREAD

TL_LDFLAGS="-std=gnu++0x"
TL_CFLAGS="-std=gnu++0x -fno-strict-aliasing -DBOOST_SPIRIT_UNICODE"

AC_SUBST(TL_LDFLAGS)
AC_SUBST(TL_CFLAGS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_SIZEOF([wchar_t],,[#include <wchar.h>])
if test "0$ac_cv_sizeof_wchar_t" -lt "4"; then
   AC_MSG_ERROR([Sizeof wchar_t < 4, your system is braindead, I refuse to work]
)
fi

# Checks for library functions.
AC_CONFIG_FILES([Makefile src/Makefile ])

AC_OUTPUT
