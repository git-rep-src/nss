CC        = g++
CFLAGS    = -std=c++11 -Wall
LIBS      = -lform -lncurses
SOURCEDIR = src
BUILDDIR  = .build
SOURCES   = $(wildcard src/*.cc)
OBJECTS   = $(patsubst $(SOURCEDIR)/%.cc,$(BUILDDIR)/%.o,$(SOURCES))
DEPS     := $(OBJECTS:.o=.d)
PREFIX    = /usr/local
TARGET    = nss

.SILENT:

all: build $(BUILDDIR)/$(TARGET)

build:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

-include $(DEPS)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cc
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(BUILDDIR)/*

install:
	install -D $(BUILDDIR)/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
