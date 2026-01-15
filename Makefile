ETSHELL_DIR ?= ..
GENERATED = marked-files-out.h marked-files-out.cpp t_marked-files-out.cpp
include with-mdp.mk
include lib.mk

include marked-files-out.d
include t_marked-files-out.d

t_marked-files-out: t_marked-files-out.o $(LIBS)

test: t_marked-files-out
	./t_marked-files-out

$(LIB): $(LIB)(marked-files-out.o)

clean:
	$(RM) libmarked-files-out.a marked-files-out.o t_marked-files-out
	$(RM) t_marked-files-out.o
