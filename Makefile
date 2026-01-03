.PHONY: all test clean

CXXFLAGS += -std=c++20 -O3 -Wall -pedantic -Werror -I..

all: .mdp_run
	@$(MAKE) test

.mdp_run: $(wildcard *.md)
	@[ -x "$$(command -v mdp)" ] || echo "mdp not installed" 1>&2
	@[ -x "$$(command -v mdp)" ] && mdp README.md
	@date >$@

test: t_ostream
	@./t_ostream

t_ostream: t_ostream.o ostream.o
	@echo build $@
	@$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	@echo c++ $@
	@$(CXX) $(CXXFLAGS) -c $<
