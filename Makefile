.PHONY: all
all: libsockfilter_accept libsockfilter_connect

libsockfilter_accept:
	$(CC) -Wall -Wextra -pedantic -D_GNU_SOURCE -nostartfiles -shared -fpic -fPIC \
		-fvisibility=hidden \
		-Wconversion -Wshadow \
		-Wpointer-arith -Wcast-qual \
		-Wstrict-prototypes -Wmissing-prototypes \
	 	-o libsockfilter_accept.so libsockfilter_accept.c sockcmp.c -ldl \
		-l:libcdb_pic.a \
	 	-Wl,-z,relro,-z,now -Wl,-z,noexecstack

libsockfilter_connect:
	$(CC) -Wall -Wextra -pedantic -D_GNU_SOURCE -nostartfiles -shared -fpic -fPIC \
		-fvisibility=hidden \
		-Wconversion -Wshadow \
		-Wpointer-arith -Wcast-qual \
		-Wstrict-prototypes -Wmissing-prototypes \
	 	-o libsockfilter_connect.so libsockfilter_connect.c sockcmp.c -ldl \
		-l:libcdb_pic.a \
	 	-Wl,-z,relro,-z,now -Wl,-z,noexecstack

.PHONY: clean
clean:
	-@rm libsockfilter_accept.so libsockfilter_connect.so

.PHONY: test
test:
	@env LD_LIBRARY_PATH=. bats test
