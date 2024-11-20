
PREFIX = /usr
LIBEXECDIR = $(PREFIX)/libexec
DATADIR = $(PREFIX)/share
SUBS = customdialogsrc portal qtdialog

all:
	for dir in $(SUBS); do \
	pushd $$dir; \
	make $@; \
	popd ; \
	done

install:
	for dir in $(SUBS); do \
	pushd $$dir; \
	make $@; \
	popd ; \
	done

clean:
	for dir in $(SUBS); do \
	pushd $$dir; \
	make $@; \
	popd ; \
	done