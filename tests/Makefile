PKG = --pkg libgsf-1 --pkg cairo
LIB = -X -lghwp
INC = -X -I..

all:
	valac -o test-ghwp test-ghwp.vala ../ghwp.vapi $(INC) $(PKG) $(LIB)

clean:
	rm -f test-ghwp
