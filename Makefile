PKG = --pkg libgsf-1
LIB = `pkg-config --libs libgsf-1`

all:
	valac -c --Xcc=-fPIC *.vala $(PKG)
	gcc --shared -fPIC -o libghwp.so *.o $(LIB)

clean:
	rm -f *.h *.o *.so
