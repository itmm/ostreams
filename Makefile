.PHONY: all test clean

CXXFLAGS += -std=c++20 -O3 -Wall -pedantic -Werror -I..

all: .mdp_run
	@$(MAKE) test

.mdp_run: $(wildcard *.md)
	@[ -x "$$(command -v mdp)" ] || echo "mdp not installed" 1>&2
	@[ -x "$$(command -v mdp)" ] && mdp README.md
	@date >$@

test: t_marked-files
	./t_marked-files

t_marked-files.o: marked-files.h

t_marked-files: t_marked-files.o
	@echo build $@
	@$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	@echo c++ $@
	@$(CXX) $(CXXFLAGS) -c $<

clean:
	@rm -f .mdp_run *.o t_marked-files
