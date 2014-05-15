MSGFMT = msgfmt

MSGMERGE = msgmerge

SRCFILES = $(wildcard ../src/*.c)

POFILES = $(wildcard *.po)

MOFILES = $(POFILES:.po=.mo)

all: $(MOFILES)

%.mo: %.po
	$(MSGFMT) -o $@ $<

EXTRA_DIST = $(POFILES)

CLEANFILES = $(MOFILES)
