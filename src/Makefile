# Copyright 2017 cc-wrapper authors
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

DEPDIR ?= .deps
MAKEDEPEND ?= $(CPP) -M
CFLAGS += -g -Wall -Wpedantic -Wextra -Werror -I.. -I.

LIB_SRCS = \
	array.c \
	avl.c \
	execinfo.c \
	log.c \
	path.c

CC_WRAPPER_SRCS = \
	main.c

TESTS = \
	array \
	avl \
	log \
	path

.PHONY: all
all: cc-wrapper

cc-wrapper: $(CC_WRAPPER_SRCS:.c=.o) libccwrapper.a
	$(CC) $(CFLAGS) $^ -o $@ -static $(LDFLAGS)

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

.SECONDARY: $(TESTS:=_test)
%_test: %_test.cc libccwrapper.a | $(DEPDIR)
	@$(MAKEDEPEND) $(CFLAGS) $(CXXFLAGS) $(GTEST_CFLAGS) \
		-std=c++17 $< >$(DEPDIR)/$(<:.cc=.d)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(GTEST_CFLAGS) \
		-std=c++17 $^ -o $@ $(LDFLAGS) $(GTEST_LIBS)

%.o: %.c | $(DEPDIR) config.h
	@$(MAKEDEPEND) $(CFLAGS) -std=c99 $< >$(DEPDIR)/$(<:.c=.d)
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@

$(DEPDIR):
	@mkdir -p $(DEPDIR)

.PHONY: check
check: $(TESTS:=_test_pass)

.PHONY: clean
clean:
	-rm -rf $(DEPDIR) suppressions.gen *_test_pass *_test_log *_test \
		*.d *.a *.o *.so* cc-wrapper

.PHONY: distclean
distclean: clean
	-rm -rf Makefile.gen config.h

INSTALL_EXES_ABS = $(INSTALL_EXES:%=$(PREFIX)/bin/%)
.PHONY: install
install: $(INSTALL_EXES_ABS) $(PREFIX)/bin/cc-wrapper

$(PREFIX)/bin/%:
	@rm -f $@
	@mkdir -p $(shell dirname $@)
	@ln -sv cc-wrapper $@

$(PREFIX)/bin/cc-wrapper: cc-wrapper
	@rm -f $@
	install -D -m0755 -s $< $(PREFIX)/bin

include $(wildcard $(LIB_SRCS:%.c=$(DEPDIR)/%.d))
include $(wildcard $(CC_WRAPPER_SRCS:%.c=$(DEPDIR)/%.d))
include $(wildcard $(TESTS:%=$(DEPDIR)/%_test.d))
