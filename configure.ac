# Initialization
AC_PREREQ([2.69])
AC_INIT([cc-wrapper], [0.1], [https://github.com/triton/cc-wrapper/issues])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([nostdinc foreign subdir-objects -Wall -Wno-portability -Werror dist-xz tar-ustar])

# Make sure the default CFLAGS of `-O2 -g` don't override CODE_COVERAGE_CFLAGS
# It is important that this comes before AC_PROG_C{C,XX}, as we are attempting
# to stop them from populating default CFLAGS and CXXFLAGS.
AS_IF([test "x$enable_tests" = "xno"], [enable_code_coverage=no])
AS_IF([test "x$enable_code_coverage" != "xno"], [
    AS_IF([test "x${CXXFLAGS+set}" != "xset"], [
        AC_SUBST(CXXFLAGS, [""])
    ])
    AS_IF([test "x${CFLAGS+set}" != "xset"], [
        AC_SUBST(CFLAGS, [""])
    ])
])

# Checks for programs.
AC_PROG_CXX
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libtool
LT_INIT

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
AX_APPEND_COMPILE_FLAGS([-Wall -Wextra], [CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wall -Wextra], [CXXFLAGS])

# RE2 requires pthreads
AX_PTHREAD

# Make it possible for users to choose if they want test support
# explicitly or not at all
AC_ARG_ENABLE([tests], AC_HELP_STRING([--disable-tests],
                                      [Build test cases]))

# Check for valgrind
AS_IF([test "x$enable_tests" = "xno"], [enable_valgrind=no])
m4_foreach([vgtool], [valgrind_tool_list],
    [AX_VALGRIND_DFLT(vgtool, [off])])
AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_CHECK
AM_EXTRA_RECURSIVE_TARGETS([check-valgrind])
m4_foreach([vgtool], [valgrind_tool_list],
    [AM_EXTRA_RECURSIVE_TARGETS([check-valgrind-]vgtool)])

# Code coverage
AX_CODE_COVERAGE
AS_IF([test "x$CODE_COVERAGE_ENABLED" = "xyes"], [
    AX_APPEND_COMPILE_FLAGS([-DHAVE_GCOV], [CODE_COVERAGE_CPPFLAGS])
])
m4_ifdef([_AX_CODE_COVERAGE_RULES],
    [AM_CONDITIONAL(AUTOCONF_CODE_COVERAGE_2019_01_06, [true])],
    [AM_CONDITIONAL(AUTOCONF_CODE_COVERAGE_2019_01_06, [false])])
AX_ADD_AM_MACRO_STATIC([])

# Append -Werror after doing autoconf compiler checks
# Otherwise some perfectly valid checks can fail and cause our
# final configuratin to be broken.
AC_ARG_ENABLE([werror], AC_HELP_STRING([--disable-werror], [Whether to automatically add -Werror CFLAGS]))
AS_IF([test "x$enable_tests" != "xno"], [
    AX_APPEND_COMPILE_FLAGS([-Werror], [CFLAGS])
    AX_APPEND_COMPILE_FLAGS([-Werror], [CXXFLAGS])
])

# Create configured output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
