.PHONY: all test clean sub_test sub_test_clean

CPPFLAGS += -I..
CXXFLAGS += -std=c++20 -O3 -Wall -pedantic -Werror

LIBS :=

LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

all: test

sub_test:
	$(MAKE) -C tests

sub_test_clean:
	$(MAKE) -C tests clean
