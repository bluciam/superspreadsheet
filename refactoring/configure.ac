## configure.ac for The SpreadSheet project refactoring

AC_PREREQ([2.65])
AC_INIT([The Cartesian SpreadSheet], [0.0])

# Checks for programs.
AM_INIT_AUTOMAKE
AC_PROG_CXX
AC_LANG([C++])

# check for the correct version of gcc
AC_MSG_CHECKING([for gcc >= 4.5.0])
GCC_CHECK_TEXT=`cat $srcdir/checks/version.cpp`
AC_COMPILE_IFELSE($GCC_CHECK_TEXT,
[AC_MSG_RESULT([yes])],
[AC_MSG_ERROR([gcc version 4.5.0 or greater is required])]
)

# Checks for libraries.
PKG_CHECK_MODULES(GTKMM, gtkmm-2.4,
[
  have_gtkmm=true,
  LDFLAGS="$LDFLAGS $GTKMM_LIBS"
  CPPFLAGS="$CPPFLAGS $GTKMM_CFLAGS"
],
[
  have_gtkmm=false
  AC_MSG_ERROR([Could not locate gtkmm-2.4.])
])



# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile src/Makefile ])


AC_OUTPUT


