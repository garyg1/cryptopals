CC=clang
CFLAGS= -Wall -O2 -L/opt/homebrew/opt/openssl@3/lib -I/opt/homebrew/opt/openssl@3/include -lssl -lcrypto

all: 1a 1b 1c 1d 1e 1f 1g 1h

1a:
	$(CC) $(CFLAGS) 1a.c -o 1a.exe
1b:
	$(CC) $(CFLAGS) 1b.c -o 1b.exe
1c:
	$(CC) $(CFLAGS) 1c.c -o 1c.exe
1d:
	$(CC) $(CFLAGS) 1d.c -o 1d.exe
1e:
	$(CC) $(CFLAGS) 1e.c -o 1e.exe
1f:
	$(CC) $(CFLAGS) 1f.c -o 1f.exe
1g:
	$(CC) $(CFLAGS) 1g.c -o 1g.exe
1h:
	$(CC) $(CFLAGS) 1h.c -o 1h.exe

clean:
	rm -f *.exe
	rm -rf *.exe.dSYM