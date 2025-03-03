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
	g++ `pkg-config --cflags --libs Qt5Gui Qt5Widgets Qt5Concurrent` -fPIC *.cpp -o $(PROGNAME)

clean:
	rm -f $(PROGNAME)