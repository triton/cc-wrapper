# Initialization
AC_PREREQ([2.69])
AC_INIT([cc-wrapper], [0.1], [https://github.com/triton/cc-wrapper/issues])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([nostdinc foreign subdir-objects -Wall -Wno-portability -Werror dist-xz tar-ustar])
AM_MAINTAINER_MODE([enable])

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

AC_DEFUN([WRAPPER_WITH], [
    m4_define([escaped], m4_translit([$1], [-+.], [___]))
    m4_define([var], [with_]escaped())
    AC_ARG_WITH([$1], AC_HELP_STRING([--with-$1=<$1>], [$2]), [], var()[=no])
    AS_IF([test "x$]var()[" = "xyes"], [AC_MSG_FAILURE("--with-$1 requires argument")])
])
AC_DEFUN([WRAPPER_WITH_DEFINE], [
    WRAPPER_WITH([$1], [$2])
    AS_IF([test "x$]var()[" = "xno"], var()[=])
    AC_DEFINE_UNQUOTED([WRAPPER_]m4_toupper(escaped()), ["$]var()["], [$2])
])

WRAPPER_WITH([tooldirs], [Directories providing tools to wrap separated by PATHSEP])
AS_IF([test "x$with_tooldirs" != "xno"],
      [AC_SUBST([TOOLDIRS], [$with_tooldirs])],
      [AC_MSG_FAILURE("--with-tooldirs is required")])
WRAPPER_WITH_DEFINE([cflags], [C compiler flags to provide for all invocations])
WRAPPER_WITH_DEFINE([cflags-link], [C compiler flags to provide if recursing to linking])
WRAPPER_WITH_DEFINE([cxxflags], [C++ compiler flags to provide for all invocations])
WRAPPER_WITH_DEFINE([cxxflags-link], [C++ compiler flags to provide if recursing to linking])
WRAPPER_WITH_DEFINE([ldflags], [Linker flags to provide after passed arguments])
WRAPPER_WITH_DEFINE([ldflags-before], [Linker flags to provide before any passed arguments])
WRAPPER_WITH_DEFINE([ldflags-dynamic], [Linker flags to provide if working with shared libraries])

AC_ARG_WITH([preferred-compiler], AC_HELP_STRING(
            [--with-preferred-compiler=<no|gcc|clang>],
            [The compiler to prefer when detecting and linking]),
            [],
            [with_preferred_compiler=])
AS_IF([test "x$with_preferred_compiler" != "xno" && test "x$with_preferred_compiler" != "xgcc" && test "x$with_preferred_compiler" != "xclang"], [
    AC_MSG_FAILURE("--with-preferred-compiler requires a valid compiler")
])
AC_DEFUN([PREFIX_MAP_FLAG], [
    AC_ARG_WITH([prefix-map-flag-$1], AC_HELP_STRING(
                [--with-prefix-map-flag-$1=<flagname>],
                [The flag to pass to $1 for setting the prefix map]),
                [],
                [with_prefix_map_flag_$1=auto])
    AS_IF([test "x$with_preferred_compiler" = "xno" || test "$with_preferred_compiler" = "x$1"], [
        AS_IF([test "x$with_prefix_map_flag_$1" = "xyes"], [
            AC_MSG_FAILURE("Please specify --with-prefix-map-flag-$1=<flag>")
        ])
        AS_IF([test "x$with_prefix_map_flag_$1" = "xno"], [
            with_prefix_map_flag_$1=
        ])
        AS_IF([test "x$with_prefix_map_flag_$1" = "xauto"], [
            m4_define([var], m4_translit([$1], [a-z], [A-Z]))
            AC_CHECK_TARGET_TOOL(var()_TO_WRAP, [$1], [], [$with_tooldirs])
            AS_IF([test "x$]var()[_TO_WRAP" != "x"], [
                AC_MSG_CHECKING([whether $]var()[_TO_WRAP accepts -ffile-prefix-map])
                AS_IF([echo "" | $]var()[_TO_WRAP -ffile-prefix-map=/usr=/no-such-path -E - >/dev/null 2>&1], [
                    with_prefix_map_flag_$1=file-prefix-map
                    AC_MSG_RESULT([yes])
                ], [
                    with_prefix_map_flag_$1=debug-prefix-map
                    AC_MSG_RESULT([no])
                ])
            ], [
                with_prefix_map_flag_$1=
            ])
        ])
    ])
])
PREFIX_MAP_FLAG([gcc])
PREFIX_MAP_FLAG([clang])

AC_SUBST([PREFERRED_COMPILER], [$with_preferred_compiler])
AC_SUBST([PREFIX_MAP_FLAG_GCC], [$with_prefix_map_flag_gcc])
AC_SUBST([PREFIX_MAP_FLAG_CLANG], [$with_prefix_map_flag_clang])

AC_ARG_WITH([var-prefix], AC_HELP_STRING(
            [--with-var-prefix=<prefix>],
            [The prefix to use for variables inside of the wrapper, Ex. `WRAPPER_FOR_BUILD` -> WRAPPER_FOR_BUILD_CFLAGS_COMPILE]),
            [], [with_var_prefix=CC_WRAPPER])
AS_IF([test "x$with_var_prefix" = "xyes"], [
    AC_MSG_FAILURE("Var prefix needs to be specified")
])
AS_IF([test "x$with_var_prefix" = "xno"], [
    AC_MSG_FAILURE("Var prefix needs to be specified")
])
AC_DEFINE_UNQUOTED([VAR_PREFIX], ["$with_var_prefix"], [VAR Prefix])

AS_IF([test "x$enable_maintainer_mode" != "xno"], [
    AC_CHECK_PROGS([CLANG_FORMAT], [clang-format], [@true])
], [
    CLANG_FORMAT=@true
])
AC_SUBST([CLANG_FORMAT], [$CLANG_FORMAT])

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
