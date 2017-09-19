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

DEPDIR ?= .deps
MAKEDEPEND ?= $(CPP) -M
CFLAGS += -Wall -Wpedantic -Wextra -Werror

LIB_SRCS = log.c
CC_WRAPPER_SRCS = main.c libccwrapper.a
TESTS = log

all: cc-wrapper
check: $(TESTS:=_test_log)

cc-wrapper: $(CC_WRAPPER_SRCS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@ -static $(LDFLAGS)

libccwrapper.a: $(LIB_SRCS:.c=.o)
	$(AR) rcs $@ $^

%_test_log: %_test
	@./$< 2>&1 | tee $@

.SECONDARY: $(TESTS:=_test)
%_test: %_test.cc libccwrapper.a | $(DEPDIR)
	@$(MAKEDEPEND) $(CFLAGS) $< >$(DEPDIR)/$(<:.cc=.d)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(shell pkg-config gtest --cflags) \
		-std=c++14 $^ -o $@ $(LDFLAGS) $(shell pkg-config gtest --libs)

%.o: %.c | $(DEPDIR)
	@$(MAKEDEPEND) $(CFLAGS) $< >$(DEPDIR)/$(<:.c=.d)
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@

$(DEPDIR):
	@mkdir -p $(DEPDIR)

clean:
	-rm -rf $(DEPDIR) *_test_log *_test *.a *.o *.so* cc-wrapper

.PHONY: all check clean

include $(wildcard $(SRCS:%.c=$(DEPDIR)/%.d))
