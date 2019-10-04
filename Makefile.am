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

if AUTOCONF_CODE_COVERAGE_2019_01_06
include $(top_srcdir)/aminclude_static.am
else
@CODE_COVERAGE_RULES@
endif
clean-local: code-coverage-clean

tooldir = $(pkglibdir)/$(target_alias)/bin

export AM_CPPFLAGS = -I. -Isrc -I$(srcdir)/src -DTOOLDIR="\"$(tooldir)\"" \
                     -I$(srcdir)/third_party/fmt/include \
                     -DFMT_HEADER_ONLY=1 -DFMT_STRING_ALIAS=1 \
                     -I$(srcdir)/third_party/optional-lite/include \
                     -I$(srcdir)/third_party/parallel-hashmap \
                     -I$(srcdir)/third_party/span-lite/include \
                     -I$(srcdir)/third_party/string-view-lite/include \
                     $(CODE_COVERAGE_CPPFLAGS)
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

MAINTAINERCLEANFILES = src/gcc/args.cpp src/gcc/harden_filter.cpp
%.cpp: %.cpp.rl
	$(RAGEL) -G2 -C -o $@ $<

BUILT_SOURCES = clang-format
.PHONY: clang-format
clang-format:
	$(CLANG_FORMAT) -i $(filter-out $(MAINTAINERCLEANFILES),$(DIST_SOURCES)) $(HEADERS)

noinst_LIBRARIES = libccwrapper.a libccwrapper_gen.a
libccwrapper_a_SOURCES = src/flags.cpp src/gcc.cpp src/gcc/harden.cpp \
                         src/generic.cpp src/linker.cpp src/main.cpp \
                         src/strings.cpp src/util.cpp
libccwrapper_a_LIBADD = libccwrapper_gen.a
libccwrapper_gen_a_SOURCES = src/gcc/args.cpp src/gcc/harden_filter.cpp
libccwrapper_gen_a_CPPFLAGS = $(AM_CPPFLAGS) -Wno-implicit-fallthrough

bin_PROGRAMS = cc-wrapper
cc_wrapper_SOURCES = src/realmain.cpp
nodist_cc_wrapper_SOURCES = src/bins.cpp
cc_wrapper_LDADD = libccwrapper.a libccwrapper_gen.a

TEST_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/third_party/Catch2/single_include
check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_PROGRAMS += test/flags
test_flags_SOURCES = test/flags.cpp
test_flags_LDADD = libccwrapper.a libccwrapper_gen.a
test_flags_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/args
test_gcc_args_SOURCES = test/gcc/args.cpp
test_gcc_args_LDADD = libccwrapper.a libccwrapper_gen.a
test_gcc_args_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/gcc/harden
test_gcc_harden_SOURCES = test/gcc/harden.cpp
test_gcc_harden_LDADD = libccwrapper.a libccwrapper_gen.a
test_gcc_harden_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/main
test_main_SOURCES = test/main.cpp
test_main_LDADD = libccwrapper.a libccwrapper_gen.a
test_main_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/strings
test_strings_SOURCES = test/strings.cpp
test_strings_LDADD = libccwrapper.a libccwrapper_gen.a
test_strings_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += test/util
test_util_SOURCES = test/util.cpp
test_util_LDADD = libccwrapper.a libccwrapper_gen.a
test_util_CPPFLAGS = $(TEST_CPPFLAGS)

install-exec-local: install-links
.PHONY: install-links
install-links: $(srcdir)/scripts/install-links.sh bins.txt
	TARGET="$(target_alias)" TOOLDIR="$(tooldir)" BINDIR="$(bindir)" \
		$< bins.txt


EXTRA_DIST = LICENSE NOTICE README.md \
             src/gcc/args.cpp.rl src/gcc/harden_filter.cpp.rl \
             scripts/generate-bins.sh scripts/install-links.sh \
             third_party/fmt/include \
             third_party/optional-lite/include \
             third_party/parallel-hashmap/parallel_hashmap \
             third_party/span-lite/include \
             third_party/string-view-lite/include

noinst_HEADERS = src/bins.hpp src/flags.hpp src/fmt_sv.hpp src/gcc.hpp \
                 src/gcc/args.hpp src/gcc/harden.hpp src/generic.hpp \
                 src/linker.hpp src/main.hpp src/strings.hpp src/util.hpp \
                 test/mock_exec.hpp
