PREFIX = /usr
LIBEXECDIR = $(PREFIX)/libexec
DATADIR = $(PREFIX)/share
PROGNAME = customdialog

.PHONY: all install

all: $(PROGNAME)

install: $(PROGNAME)
	install -Dm755 $(PROGNAME) $(DESTDIR)$(LIBEXECDIR)/$(PROGNAME)
	strip --strip-all $(DESTDIR)$(LIBEXECDIR)/$(PROGNAME)

$(PROGNAME):
#	g++ -D$(DEF) `pkg-config --cflags --libs $(WHAT)` -fPIC *.cpp -o $(PROGNAME)
	g++ -g -Wall `pkg-config --cflags --libs Qt6Core Qt6Widgets` -fPIC *.cpp -fPIC -o $(PROGNAME)

clean:
	rm -f $(PROGNAME)