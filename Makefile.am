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

tooldir = $(pkglibdir)/$(target_alias)/bin

export AM_CPPFLAGS = -I. -Isrc -I$(srcdir)/src -DTOOLDIR="\"$(tooldir)\"" \
                     -I$(srcdir)/third_party/fmt/include \
                     -DFMT_HEADER_ONLY=1 -DFMT_STRING_ALIAS=1 \
                     -I$(srcdir)/third_party/optional-lite/include \
                     -I$(srcdir)/third_party/parallel-hashmap \
                     -I$(srcdir)/third_party/span-lite/include \
                     -I$(srcdir)/third_party/string-view-lite/include \
                     -Wno-implicit-fallthrough $(CODE_COVERAGE_CPPFLAGS)
export AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
export AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

export LDADD = $(CODE_COVERAGE_LIBS)

CLEANFILES = src/bins.cpp
.INTERMEDIATE: src/bins.cpp.tmp bins.txt.tmp
src/bins.cpp bins.txt: $(srcdir)/scripts/generate-bins.sh
	TARGET="$(target_alias)" TOOLDIRS="$(TOOLDIRS)" \
		PREFERRED_COMPILER="$(PREFERRED_COMPILER)" \
		PREFIX_MAP_FLAG_GCC="$(PREFIX_MAP_FLAG_GCC)" \
		PREFIX_MAP_FLAG_CLANG="$(PREFIX_MAP_FLAG_CLANG)" \
		$< src/bins.cpp.tmp bins.txt.tmp
	mv src/bins.cpp.tmp src/bins.cpp
	mv bins.txt.tmp bins.txt

MAINTAINERCLEANFILES = src/gcc/args.cpp src/gcc/harden_filter.cpp \
                       src/gcc/path_filter.cpp src/linker/path_filter.cpp
%.cpp: %.cpp.rl
	$(RAGEL) -G2 -C -o $@ $<

BUILT_SOURCES = clang-format
.PHONY: clang-format
clang-format:
	$(CLANG_FORMAT) -i $(filter-out $(MAINTAINERCLEANFILES),$(DIST_SOURCES)) $(HEADERS)

noinst_LIBRARIES = libccwrapper.a
libccwrapper_a_SOURCES = src/env.cpp src/flags.cpp src/gcc.cpp \
                         src/gcc/args.cpp src/gcc/harden.cpp \
                         src/gcc/harden_filter.cpp src/gcc/path_filter.cpp \
                         src/generic.cpp src/linker.cpp \
                         src/linker/path_filter.cpp src/main.cpp src/path.cpp \
                         src/strings.cpp src/util.cpp

bin_PROGRAMS = cc-wrapper
cc_wrapper_SOURCES = src/realmain.cpp
nodist_cc_wrapper_SOURCES = src/bins.cpp
cc_wrapper_LDADD = libccwrapper.a

TEST_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/third_party/Catch2/single_include
check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_PROGRAMS += test/flags_test
test_flags_test_SOURCES = test/flags.cpp
test_flags_test_LDADD = libccwrapper.a
test_flags_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/args_test
test_gcc_args_test_SOURCES = test/gcc/args.cpp
test_gcc_args_test_LDADD = libccwrapper.a
test_gcc_args_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/harden_test
test_gcc_harden_test_SOURCES = test/gcc/harden.cpp
test_gcc_harden_test_LDADD = libccwrapper.a
test_gcc_harden_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/path_test
test_gcc_path_test_SOURCES = test/gcc/path.cpp
test_gcc_path_test_LDADD = libccwrapper.a
test_gcc_path_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/main_test
test_main_test_SOURCES = test/main.cpp
test_main_test_LDADD = libccwrapper.a
test_main_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker_test
test_linker_test_SOURCES = test/linker.cpp
test_linker_test_LDADD = libccwrapper.a
test_linker_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/linker/path_test
test_linker_path_test_SOURCES = test/linker/path.cpp
test_linker_path_test_LDADD = libccwrapper.a
test_linker_path_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/path_test
test_path_test_SOURCES = test/path.cpp
test_path_test_LDADD = libccwrapper.a
test_path_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/strings_test
test_strings_test_SOURCES = test/strings.cpp
test_strings_test_LDADD = libccwrapper.a
test_strings_test_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/util_test
test_util_test_SOURCES = test/util.cpp
test_util_test_LDADD = libccwrapper.a
test_util_test_CPPFLAGS = $(TEST_CPPFLAGS)

install-exec-local: install-links
.PHONY: install-links
install-links: $(srcdir)/scripts/install-links.sh bins.txt
	TARGET="$(target_alias)" TOOLDIR="$(tooldir)" BINDIR="$(bindir)" \
		$< bins.txt


EXTRA_DIST = LICENSE NOTICE README.md \
             src/gcc/args.cpp.rl src/gcc/harden_filter.cpp.rl \
             src/gcc/path_filter.cpp.rl src/linker/path_filter.cpp.rl \
             scripts/generate-bins.sh scripts/install-links.sh \
             third_party/fmt/include \
             third_party/optional-lite/include \
             third_party/parallel-hashmap/parallel_hashmap \
             third_party/span-lite/include \
             third_party/string-view-lite/include

noinst_HEADERS = src/bins.hpp src/env.hpp src/flags.hpp src/fmt_sv.hpp \
                 src/gcc.hpp src/gcc/args.hpp src/gcc/harden.hpp \
                 src/gcc/path.hpp src/generic.hpp src/linker.hpp \
                 src/linker/path.hpp src/main.hpp src/path.hpp src/strings.hpp \
                 src/util.hpp test/mock_exec.hpp
