include ../Makefile.base
include Makefile.lib

t_marked-files-out.o: $(LIBS_H)

t_marked-files-out: t_marked-files-out.o $(LIBS_O)

test: t_marked-files-out
	./t_marked-files-out

clean:
	$(RM) libmarked-files-out.a marked-files-out.o t_marked-files-out t_marked-files-out.o
