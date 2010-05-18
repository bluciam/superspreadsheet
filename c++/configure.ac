## Learning GTKmm: how to use gtkmm, C++, autoconf and automake

AC_PREREQ([2.65])
AC_INIT([The Cartesian SpreadSheet], [0.0])

# Checks for programs.
AM_INIT_AUTOMAKE
AC_PROG_CXX

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
AC_CONFIG_FILES([Makefile src/Makefile src/examples/Makefile
                 src/examples/window/Makefile src/examples/infobar/Makefile])


AC_OUTPUT


