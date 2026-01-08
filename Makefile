include Makefile.base
include Makefile.lib

include t_marked-files-out.d

t_marked-files-out: t_marked-files-out.o $(LIBS)

test: t_marked-files-out
	./t_marked-files-out

clean:
	$(RM) libmarked-files-out.a marked-files-out.o t_marked-files-out t_marked-files-out.o
