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

export AM_CPPFLAGS = -Isrc -I$(srcdir)/src \
                     -I$(srcdir)/third_party/fmt/include \
                     -DFMT_HEADER_ONLY=1 -DFMT_STRING_ALIAS=1 \
                     -I$(srcdir)/third_party/parallel-hashmap \
                     -I$(srcdir)/third_party/re2 \
                     $(PTHREAD_CFLAGS) $(CODE_COVERAGE_CPPFLAGS)
export AM_CFLAGS = $(PTHREAD_CFLAGS) $(CODE_COVERAGE_CFLAGS)
export AM_CXXFLAGS = $(PTHREAD_CFLAGS) $(CODE_COVERAGE_CXXFLAGS)

export COMMON_LIBS = $(PTHREAD_LIBS) $(CODE_COVERAGE_LIBS)

bin_PROGRAMS = cc-wrapper
cc_wrapper_SOURCES = src/main.cpp
cc_wrapper_LDADD = $(COMMON_LIBS) $(srcdir)/third_party/re2/obj/libre2.a

$(srcdir)/third_party/re2/obj/libre2.a:
	$(MAKE) -C $(srcdir)/third_party/re2 CXXFLAGS="$(CXXFLAGS)" obj/libre2.a
clean-local: clean-re
.PHONY: clean-re
clean-re:
	$(MAKE) -C $(srcdir)/third_party/re2 clean

noinst_HEADERS = third_party/fmt/include \
                 third_party/parallel-hashmap/parallel_hashmap \
                 third_party/re2/re2 \
                 third_party/re2/util
EXTRA_DIST = LICENSE NOTICE README.md third_party/re2/Makefile
