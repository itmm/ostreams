include ../Makefile.base
include Makefile.lib

t_marked-files-out.o: ../marked-files-out/marked-files-out.h

t_marked-files-out: t_marked-files-out.o ../marked-files-out/libmarked-files-out.a
	@echo building $@
	@$(CXX) $(CXXARGS) -o $@ t_marked-files-out.o -L. -lmarked-files-out

test: t_marked-files-out
	@echo "testing marked-files-out ... \c";
	@./t_marked-files-out
	@echo "ok"

clean:
	@rm -f libmarked-files-out.a marked-files-out.o t_marked-files-out t_marked-files-out.o
