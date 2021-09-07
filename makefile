CC ?= gcc
CFLAGS += -ansi -Wall -Wextra -pedantic -Werror -O2
LDFLAGS += -lm

SRCS := $(shell find src/ -name '*.c')
OBJS := $(SRCS:.c=.o)

PREFIX ?= /usr
MAKE_SYMLINK = 1

slim: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: slim slim.1
	cp slim.1 $(PREFIX)/share/man/man1/
	cp $< $(PREFIX)/bin/
	if [ $(MAKE_SYMLINK) -eq 1 ]; then \
		ln -s $(PREFIX)/bin/$< /usr/bin/sleep; \
	fi

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f slim
