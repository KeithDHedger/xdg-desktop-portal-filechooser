
PREFIX = /usr
LIBEXECDIR = $(PREFIX)/libexec
DATADIR = $(PREFIX)/share
USEQT6 = 1

ifeq ($(USEQT6),1)
	SUBS = customdialogsrcqt6 portal qtdialog
else
	SUBS = customdialogsrcqt5 portal qtdialog
endif

all:
	echo $(SUBS)
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