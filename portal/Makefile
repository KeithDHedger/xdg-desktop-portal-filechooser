VALAC = valac
PREFIX = /usr
LIBEXECDIR = $(PREFIX)/libexec
DATADIR = $(PREFIX)/share
IFLAGS = --pkg gio-2.0 --pkg gtk+-3.0 
PASSCCFLAGS = -Wno-incompatible-pointer-types
PROGNAME = filechooser

DBUS := org.freedesktop.impl.portal.desktop.filechooser

.PHONY: all install

all: filechooser $(DBUS).service

install: filechooser $(DBUS).service filechooser.portal
	install -Dm755 filechooser $(DESTDIR)$(LIBEXECDIR)/$(PROGNAME)
	strip --strip-all $(DESTDIR)$(LIBEXECDIR)/$(PROGNAME)
	install -Dm644 $(DBUS).service $(DESTDIR)$(DATADIR)/dbus-1/services/$(DBUS).service
	install -Dm644 filechooser.portal $(DESTDIR)$(DATADIR)/xdg-desktop-portal/portals/filechooser.portal
	install -Dm777 xdgportalscript $(DESTDIR)$(DATADIR)/filechooserportal/xdgportalscript

$(DBUS).service:
	printf '%s\n' '[DBUS Service]' 'Name=$(DBUS)' 'Exec=$(LIBEXECDIR)/filechooser' > $(DBUS).service

filechooser: filechooser.vala
	$(VALAC) $(IFLAGS) --Xcc=$(PASSCCFLAGS) -o $@  $<

clean:
	rm -f $(DBUS).service $(PROGNAME)