all : longexp

CFLAGS:=-O2 -g
LDFLAGS:=-g
LDLIBS:=-lavcodec -lavformat -lswscale -lm  -lpthread -lavutil

longexp : longexp.o ffmdecode.o
	gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean :
	rm -rf *.o *~ longexp

