PREFIX = /usr
LIBEXECDIR = $(PREFIX)/libexec
DATADIR = $(PREFIX)/share
PROGNAME = qtdialog

.PHONY: all install

all: $(PROGNAME)

install: $(PROGNAME)
	install -Dm755 $(PROGNAME) $(DESTDIR)$(LIBEXECDIR)/$(PROGNAME)
	strip --strip-all $(DESTDIR)$(LIBEXECDIR)/$(PROGNAME)

$(PROGNAME):
	g++ `pkg-config --cflags --libs Qt5Core Qt5Widgets` -fPIC *.cpp -o $(PROGNAME)

clean:
	rm -f $(PROGNAME)

