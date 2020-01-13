#
# sys-resource-win Makefile
# Copied from mman-win32
#
include config.mak

CFLAGS=-Wall -O3 -fomit-frame-pointer

ifeq ($(BUILD_STATIC),yes)
	TARGETS+=libresource.a
	INSTALL+=static-install
endif

ifeq ($(BUILD_SHARED),yes)
	TARGETS+=libresource.dll
	INSTALL+=shared-install
	CFLAGS+=-DRESOURCE_LIBRARY_DLL -DRESOURCE_LIBRARY
endif

ifeq ($(BUILD_MSVC),yes)
	SHFLAGS+=-Wl,--output-def,libresource.def
	INSTALL+=lib-install
endif

all: $(TARGETS)

resource.o: resource.c resource.h
	$(CC) -o resource.o -c resource.c $(CFLAGS)

libresource.a: resource.o
	$(AR) cru libresource.a resource.o
	$(RANLIB) libresource.a

libresource.dll: resource.o
	$(CC) -shared -o libresource.dll resource.o -Wl,--out-implib,libresource.dll.a

header-install:
	mkdir -p $(DESTDIR)$(incdir)
	cp resource.h $(DESTDIR)$(incdir)

static-install: header-install
	mkdir -p $(DESTDIR)$(libdir)
	cp libresource.a $(DESTDIR)$(libdir)

shared-install: header-install
	mkdir -p $(DESTDIR)$(libdir)
	cp libresource.dll.a $(DESTDIR)$(libdir)
	mkdir -p $(DESTDIR)$(bindir)
	cp libresource.dll $(DESTDIR)$(bindir)

lib-install:
	mkdir -p $(DESTDIR)$(libdir)
	cp libresource.lib $(DESTDIR)$(libdir)

install: $(INSTALL)

test.exe: test.c resource.c resource.h
	$(CC) -o test.exe test.c -L. -lresource

test: $(TARGETS) test.exe
	test.exe

clean::
	rm -f resource.o libresource.a libresource.dll.a libresource.dll libresource.def libresource.lib test.exe *.dat

distclean: clean
	rm -f config.mak

.PHONY: clean distclean install test
