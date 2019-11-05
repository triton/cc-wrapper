ACLOCAL_AMFLAGS = -I m4

# Ignore system headers
CODE_COVERAGE_IGNORE_PATTERN = '/include/*' '/usr/include/*' '$(includedir)/*'
# Ignore the real implementation sources for sd_event
CODE_COVERAGE_IGNORE_PATTERN += \
    '$(abs_builddir)/src/sdeventplus/internal/sdevent.*'
export CODE_COVERAGE_IGNORE_PATTERN

CODE_COVERAGE_LCOV_SHOPTS = $(CODE_COVERAGE_LCOV_SHOPTS_DEFAULT)
# Use our configuration file for lcov
CODE_COVERAGE_LCOV_SHOPTS += --config-file $(abs_srcdir)/.lcovrc
export CODE_COVERAGE_LCOV_SHOPTS

CODE_COVERAGE_LCOV_OPTIONS = $(CODE_COVERAGE_LCOV_OPTIONS_DEFAULT)
# Use our configuration file for lcov
CODE_COVERAGE_LCOV_OPTIONS += --config-file $(abs_srcdir)/.lcovrc
export CODE_COVERAGE_LCOV_OPTIONS

CODE_COVERAGE_LCOV_RMOPTS = $(CODE_COVERAGE_LCOV_RMOPTS_DEFAULT)
# Use our configuration file for lcov
CODE_COVERAGE_LCOV_RMOPTS += --config-file $(abs_srcdir)/.lcovrc
export CODE_COVERAGE_LCOV_RMOPTS

CODE_COVERAGE_GENHTML_OPTIONS = $(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT)
# Use our configuration file for genhtml
CODE_COVERAGE_GENHTML_OPTIONS += --config-file $(abs_srcdir)/.lcovrc
# Don't generate the absolute path for each file in the HTML output
CODE_COVERAGE_GENHTML_OPTIONS += --prefix $(abs_srcdir) --prefix $(abs_builddir)
export CODE_COVERAGE_GENHTML_OPTIONS

include $(top_srcdir)/aminclude_static.am
@CODE_COVERAGE_RULES@
clean-local: code-coverage-clean

@VALGRIND_CHECK_RULES@

tooldir = $(pkglibdir)/$(target)/bin
linkdir = $(pkglibdir)/unwrapped/bin

export AM_CPPFLAGS = -I. -Isrc -I$(srcdir)/src -DTOOLDIR="\"$(tooldir)\"" \
                     -DLINKDIR="\"$(linkdir)\"" \
                     -I$(srcdir)/third_party/fmt/include \
                     -DFMT_STRING_ALIAS=1 \
                     -I$(srcdir)/third_party/optional-lite/include \
                     -I$(srcdir)/third_party/parallel-hashmap \
                     -Dspan_CONFIG_INDEX_TYPE=std::size_t \
                     -I$(srcdir)/third_party/span-lite/include \
                     -I$(srcdir)/third_party/string-view-lite/include \
                     -Wall -Wextra $(if $(WERROR) != no,-Werror) \
                     -Wno-implicit-fallthrough -Wno-unused-const-variable \
                     $(CODE_COVERAGE_CPPFLAGS)
export AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
export AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

export LDADD = $(CODE_COVERAGE_LIBS)

CLEANFILES = src/bins.cpp
.INTERMEDIATE: src/bins.cpp.tmp bins.txt.tmp
src/bins.cpp bins.txt: $(srcdir)/scripts/generate-bins.sh
	TARGET="$(target)" TARGET_ALIAS="$(target_alias)" TOOLDIRS="$(TOOLDIRS)" \
		LINKDIR="$(linkdir)" \
		PREFERRED_COMPILER="$(PREFERRED_COMPILER)" \
		PREFIX_MAP_FLAG_GCC="$(PREFIX_MAP_FLAG_GCC)" \
		PREFIX_MAP_FLAG_CLANG="$(PREFIX_MAP_FLAG_CLANG)" \
		$< src/bins.cpp.tmp bins.txt.tmp
	mv src/bins.cpp.tmp src/bins.cpp
	mv bins.txt.tmp bins.txt

MAINTAINERCLEANFILES = src/gcc/args.cpp src/gcc/harden_filter.cpp \
                       src/gcc/path_filter.cpp src/linker/args_dyld.cpp \
                       src/linker/args_dynamic.cpp src/linker/args_libs.cpp \
                       src/linker/args_linking.cpp \
                       src/linker/args_filter_linking.cpp \
                       src/linker/compiler_filter.cpp \
                       src/linker/harden_filter.cpp src/linker/path_filter.cpp \
                       src/linker/script_libs.cpp src/path_canonicalize.cpp
%.cpp: %.cpp.rl
	$(RAGEL) -G2 -C -o $@ $<

BUILT_SOURCES = clang-format
.PHONY: clang-format
clang-format:
	$(CLANG_FORMAT) -i $(addprefix $(srcdir)/,$(filter-out $(MAINTAINERCLEANFILES),$(DIST_SOURCES)) $(HEADERS))

noinst_LIBRARIES = libccwrapper.a
libccwrapper_a_SOURCES = src/env.cpp src/file.cpp src/flags.cpp src/fmt.cpp \
                         src/gcc.cpp src/gcc/args.cpp src/gcc/harden.cpp \
                         src/gcc/harden_filter.cpp src/gcc/path.cpp \
                         src/gcc/path_filter.cpp src/generic.cpp \
                         src/linker.cpp src/linker/args_dyld.cpp \
                         src/linker/args_dynamic.cpp src/linker/args_libs.cpp \
                         src/linker/args_linking.cpp \
                         src/linker/args_filter_linking.cpp \
                         src/linker/compiler_filter.cpp \
                         src/linker/harden.cpp src/linker/harden_filter.cpp \
                         src/linker/path_filter.cpp src/linker/script_libs.cpp \
                         src/linker/state.cpp src/linker/script.cpp \
                         src/main.cpp src/path.cpp src/path_canonicalize.cpp \
                         src/strings.cpp src/util.cpp

bin_PROGRAMS = cc-wrapper
cc_wrapper_SOURCES = src/realmain.cpp
nodist_cc_wrapper_SOURCES = src/bins.cpp
cc_wrapper_LDADD = libccwrapper.a

TEST_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/third_party/Catch2/single_include \
                -DSRCDIR=\"$(abs_srcdir)\" -I$(srcdir)/test
check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_LIBRARIES = libcatch.a
libcatch_a_SOURCES = test/catch_main.cpp
libcatch_a_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/file_test
test_file_test_SOURCES = test/file.cpp
test_file_test_LDADD = libccwrapper.a libcatch.a
test_file_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/flags_test
test_flags_test_SOURCES = test/flags.cpp
test_flags_test_LDADD = libccwrapper.a libcatch.a
test_flags_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/graph_test
test_graph_test_SOURCES = test/graph.cpp
test_graph_test_LDADD = libccwrapper.a libcatch.a
test_graph_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/args_test
test_gcc_args_test_SOURCES = test/gcc/args.cpp
test_gcc_args_test_LDADD = libccwrapper.a libcatch.a
test_gcc_args_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/harden_test
test_gcc_harden_test_SOURCES = test/gcc/harden.cpp
test_gcc_harden_test_LDADD = libccwrapper.a libcatch.a
test_gcc_harden_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/path_test
test_gcc_path_test_SOURCES = test/gcc/path.cpp
test_gcc_path_test_LDADD = libccwrapper.a libcatch.a
test_gcc_path_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/main_test
test_main_test_SOURCES = test/main.cpp
test_main_test_LDADD = libccwrapper.a libcatch.a
test_main_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/args_test
test_linker_args_test_SOURCES = test/linker/args.cpp
test_linker_args_test_LDADD = libccwrapper.a libcatch.a
test_linker_args_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/compiler_test
test_linker_compiler_test_SOURCES = test/linker/compiler.cpp
test_linker_compiler_test_LDADD = libccwrapper.a libcatch.a
test_linker_compiler_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/harden_test
test_linker_harden_test_SOURCES = test/linker/harden.cpp
test_linker_harden_test_LDADD = libccwrapper.a libcatch.a
test_linker_harden_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/path_test
test_linker_path_test_SOURCES = test/linker/path.cpp
test_linker_path_test_LDADD = libccwrapper.a libcatch.a
test_linker_path_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/script_test
test_linker_script_test_SOURCES = test/linker/script.cpp
test_linker_script_test_LDADD = libccwrapper.a libcatch.a
test_linker_script_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/state_test
test_linker_state_test_SOURCES = test/linker/state.cpp
test_linker_state_test_LDADD = libccwrapper.a libcatch.a
test_linker_state_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/path_test
test_path_test_SOURCES = test/path.cpp
test_path_test_LDADD = libccwrapper.a libcatch.a
test_path_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/strings_test
test_strings_test_SOURCES = test/strings.cpp
test_strings_test_LDADD = libccwrapper.a libcatch.a
test_strings_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/util_test
test_util_test_SOURCES = test/util.cpp
test_util_test_LDADD = libccwrapper.a libcatch.a
test_util_test_CPPFLAGS = $(TEST_CPPFLAGS)

install-exec-local: install-links
.PHONY: install-links
install-links: $(srcdir)/scripts/install-links.sh bins.txt
	TARGET="$(target)" TOOLDIR="$(tooldir)" BINDIR="$(bindir)" \
		LINKDIR="$(linkdir)" TARGET_ALIAS="$(target_alias)" $< bins.txt


EXTRA_DIST = LICENSE NOTICE README.md \
             src/gcc/args.cpp.rl src/gcc/harden_filter.cpp.rl \
             src/gcc/path_filter.cpp.rl src/linker/args_dyld.cpp.rl \
             src/linker/args_dynamic.cpp.rl src/linker/args_libs.cpp.rl \
             src/linker/args_linking.cpp.rl \
             src/linker/args_filter_linking.cpp.rl \
             src/linker/harden_filter.cpp.rl \
             src/linker/path_filter.cpp.rl src/path_canonicalize.cpp.rl \
             src/linker/script_libs.cpp.rl \
             scripts/generate-bins.sh scripts/install-links.sh \
             third_party/fmt/include third_party/fmt/src \
             third_party/optional-lite/include \
             third_party/parallel-hashmap/parallel_hashmap \
             third_party/span-lite/include \
             third_party/string-view-lite/include

noinst_HEADERS = src/bins.hpp src/env.hpp src/file.hpp src/flags.hpp \
                 src/fmt_sv.hpp src/gcc.hpp src/gcc/args.hpp \
                 src/gcc/harden.hpp src/gcc/path.hpp src/generic.hpp \
                 src/graph.hpp src/linker.hpp src/linker/args.hpp \
                 src/linker/compiler.hpp src/linker/harden.hpp \
                 src/linker/path.hpp src/linker/script.hpp \
                 src/linker/state.hpp src/main.hpp src/path.hpp \
                 src/strings.hpp src/util.hpp \
                 test/mock_exec.hpp
