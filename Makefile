PKG = --pkg libgsf-1 --pkg cairo
LIB = `pkg-config --libs libgsf-1`

all:
	valac -H ghwp.h --library ghwp -c --Xcc=-fPIC *.vala $(PKG)
	gcc --shared -fPIC -o libghwp.so *.o $(LIB)
	(cd test; make)

clean:
	rm -f *.h *.vapi *.o *.so
	(cd test; make clean)
