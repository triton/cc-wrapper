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

PREFIX ?=

DEPDIR ?= .deps
MAKEDEPEND ?= $(CPP) -M
CFLAGS += -Wall -Wpedantic -Wextra -Werror -I.. -I.

LIB_SRCS = \
	array.c \
	execinfo.c \
	log.c

CC_WRAPPER_SRCS = main.c
TESTS = \
	array \
	log

.PHONY: all
all: cc-wrapper

cc-wrapper: $(CC_WRAPPER_SRCS:.c=.o) libccwrapper.a
	$(CC) $(CFLAGS) $^ -o $@ -static $(LDFLAGS)

libccwrapper.a: $(LIB_SRCS:.c=.o)
	$(AR) rcs $@ $^

%_test_pass: %_test
	valgrind --leak-check=full --error-exitcode=1 -- ./$<
	touch $@

.SECONDARY: $(TESTS:=_test)
%_test: %_test.cc libccwrapper.a | $(DEPDIR)
	@$(MAKEDEPEND) $(CFLAGS) $< >$(DEPDIR)/$(<:.cc=.d)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(shell pkg-config gtest --cflags) \
		-std=c++14 $^ -o $@ $(LDFLAGS) $(shell pkg-config gtest --libs)

%.o: %.c | $(DEPDIR) config.h
	@$(MAKEDEPEND) $(CFLAGS) $< >$(DEPDIR)/$(<:.c=.d)
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@

config.h:
	../build/gen_config.sh

$(DEPDIR):
	@mkdir -p $(DEPDIR)

.PHONY: check
check: $(TESTS:=_test_pass)

.PHONY: clear
clean:
	-rm -rf $(DEPDIR) config.h *_test_pass *_test_log *_test *.a *.o *.so* cc-wrapper

INSTALL_EXES = $(shell ../build/compiler_exes.sh --exe-only)
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
