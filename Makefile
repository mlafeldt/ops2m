CC = gcc
CFLAGS = -Wall -Werror -O2 -s
LIBS =
prefix = $(HOME)

PROG = ops2m
OBJS += ops2m.o

all: $(PROG)

install: $(PROG)
	install $(PROG) $(prefix)/bin

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

clean:
	$(RM) $(PROG) $(OBJS)
