SHELL=/bin/bash

all: bashp

bashp: bash/execute_cmd.c.patched bash/Makefile
	cp bash/execute_cmd.c bash/execute_cmd.c.orig \
	    && cp bash/execute_cmd.c.patched bash/execute_cmd.c \
	    && make -C bash \
	    && cp bash/execute_cmd.c.orig bash/execute_cmd.c \
	    && cp bash/bash bashp

bash/Makefile: bash/configure
	pushd bash && ./configure && popd

bash/execute_cmd.c.patched: bashp.patch bashp.c
	pushd bash \
	    && git checkout execute_cmd.c \
	    && git apply ../bashp.patch \
	    && cp execute_cmd.c execute_cmd.c.patched \
	    && popd

clean:
	rm -f bashp
	pushd bash \
	    && git reset --hard HEAD \
	    && git clean -fdx \
	    && popd

.PHONY: all clean
