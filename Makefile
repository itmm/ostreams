include ../Makefile.base
include Makefile.lib

t_marked-files.o: ../marked-files/marked-files.h

t_marked-files: ../marked-files/libmarked-files.a
	@echo building $@
	@$(CXX) $(CXXARGS) -o $@ t_marked-files.o -L. -lmarked-files

test: t_marked-files
	@echo "testing marked-files ... \c";
	@./t_marked-files
	@echo "ok"

clean:
	@rm -f libmarked-files.a marked-files.o t_marked-files
