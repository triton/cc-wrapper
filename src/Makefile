# Copyright 2017,2018 cc-wrapper authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include Makefile.gen

CLANG_FORMAT ?= clang-format
CLANG_TIDY ?= clang-tidy

DEPDIR ?= .deps
MAKEDEPEND ?= $(CPP) -M
BINDIR ?= $(PREFIX)/bin
CPPFLAGS += -g -Wall -Wpedantic -Wextra -Werror -I.. -I. \
	-DCC_WRAPPER_BIN=\"$(BINDIR)\"

LIB_SRCS = \
	arguments.c \
	array.c \
	avl.c \
	environment.c \
	execinfo.c \
	log.c \
	path.c \
	string-util.c

LIB_HDRS = \
	array.h \
	avl.h \
	execinfo.h \
	func.h \
	log.h \
	path.h \
	string-util.h

CC_WRAPPER_SRCS = \
	config.c \
	main.c \
	mod_cc.c \
	mod_common.c \
	mod_ld.c

CC_WRAPPER_HDRS = \
	config.h \
	mod_cc.h \
	mod_common.h \
	mod_ld.h

TESTS = \
	arguments \
	array \
	avl \
	environment \
	execinfo \
	log \
	mod_cc \
	mod_common \
	mod_ld \
	path \
	string-util

TEST_arguments_extra = \
	arguments_test_util.c

TEST_environment_extra = \
	environment_test_util.c

TEST_execinfo_extra = \
	execinfo_test_util.c

TEST_mod_cc_extra = \
	mod_cc.c \
	mod_common.c

TEST_mod_common_extra = \
	mod_common.c

TEST_mod_ld_extra = \
	mod_ld.c

TEST_EXTRAS = $(foreach test,$(TESTS),$(TEST_$(test)_extra))

.PHONY: all
all: cc-wrapper

cc-wrapper: $(CC_WRAPPER_SRCS:.c=.o) libccwrapper.a
	$(CC) $^ -o $@ $(LDFLAGS)

libccwrapper.a: $(LIB_SRCS:.c=.o)
	$(AR) rcs $@ $^

suppressions.gen: suppressions_test
	valgrind --leak-check=full --show-leak-kinds=all --gen-suppressions=all \
		-- ./$<
	valgrind --leak-check=full --show-leak-kinds=all --gen-suppressions=all \
		-- ./$< 2>&1 >/dev/null | grep -v '^=' >suppressions.gen || true

%_test_pass: %_test suppressions.gen
	valgrind --leak-check=full --show-leak-kinds=all \
		--error-exitcode=1 --child-silent-after-fork=yes \
		--suppressions=suppressions.gen -- \
		./$<
	touch $@

.SECONDARY: $(TESTS:=_test.o)
%_test.o: %_test.cc | $(DEPDIR)
	@$(MAKEDEPEND) $(CPPFLAGS) $(CXXFLAGS) $(GTEST_CFLAGS) \
		-std=c++17 $< >$(DEPDIR)/$(<:.cc=.d)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(GTEST_CFLAGS) \
		-std=c++17 -c $< -o $@

.SECONDARY: $(TESTS:=_test)
.SECONDARY: $(TEST_EXTRAS:.c=.o)
PERCENT = %
.SECONDEXPANSION:
%_test: %_test.o $$(patsubst $$(PERCENT).c,$$(PERCENT).o,$$(TEST_$$*_extra)) libccwrapper.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(GTEST_CFLAGS) \
		-std=c++17 $^ -o $@ $(LDFLAGS) $(GTEST_LIBS)

%.o: %.c | $(DEPDIR)
	@$(MAKEDEPEND) $(CPPFLAGS) $(CFLAGS) -std=c99 $< >$(DEPDIR)/$(<:.c=.d)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c99 -c $< -o $@

$(DEPDIR):
	@mkdir -p $(DEPDIR)

.PHONY: check
check: $(TESTS:=_test_pass)

.PHONY: format
format:
	cd ../build/format-c && pwd && $(CLANG_FORMAT) -i \
		../../src/*.c ../../src/*.h
	cd ../build/format-cc; $(CLANG_FORMAT) -i \
		../../src/*.cc

%_lint_pass: %
	$(CLANG_TIDY) $< -- \
		$(CPPFLAGS) $(CFLAGS) -std=c99 \
		2>&1 | tee $@.err
	stat -c '%s' $@.err | grep -q '^0$$'
	touch $@

.SECONDARY: $(TESTS:=_test_lint_pass)
%_test_lint_pass: %_test.cc
	$(CLANG_TIDY) $< -- \
		$(CPPFLAGS) $(CXXFLAGS) $(GTEST_CFLAGS) -std=c++17 \
		2>&1 | tee $@.err
	stat -c '%s' $@.err | grep -q '^0$$'
	touch $@

LINT_PASS = \
	$(LIB_HDRS:=_lint_pass) \
	$(LIB_SRCS:=_lint_pass) \
	$(CC_WRAPPER_HDRS:=_lint_pass) \
	$(CC_WRAPPER_SRCS:=_lint_pass) \
	$(TESTS:=_test_lint_pass) \
	$(TEST_EXTRAS:=_lint_pass)

.PHONY: lint
lint: $(LINT_PASS)

.PHONY: clean
clean:
	-rm -rf $(DEPDIR) suppressions.gen \
		*_lint_pass* *_test_pass *_test \
		*.d *.a *.o *.so* cc-wrapper

.PHONY: distclean
distclean: clean
	-rm -rf Makefile.gen config.h

INSTALL_EXES_EXT = $(basename $(INSTALL_EXES)) $(INSTALL_EXES)
INSTALL_EXES_ABS = $(INSTALL_EXES_EXT:%=$(BINDIR)/%)
.PHONY: install
install: $(INSTALL_EXES_ABS) $(BINDIR)/cc-wrapper

$(BINDIR)/%:
	@rm -f $@
	@mkdir -p $(shell dirname $@)
	@ln -sv cc-wrapper $@

$(BINDIR)/cc-wrapper: cc-wrapper
	@rm -f $@
	install -D -m0755 -s $< $(BINDIR)

include $(wildcard $(LIB_SRCS:%.c=$(DEPDIR)/%.d))
include $(wildcard $(CC_WRAPPER_SRCS:%.c=$(DEPDIR)/%.d))
include $(wildcard $(TESTS:%=$(DEPDIR)/%_test.d))
