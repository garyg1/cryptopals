CC=clang
CFLAGS= -Wall -L/opt/homebrew/opt/openssl@3/lib -I/opt/homebrew/opt/openssl@3/include -lssl -lcrypto

all: challenges

# recompile every time due to test binary changes to ./lib
challenges:
	$(CC) $(CFLAGS) 1-1.c -o 1-1.exe
	$(CC) $(CFLAGS) 1-2.c -o 1-2.exe
	$(CC) $(CFLAGS) 1-3.c -o 1-3.exe
	$(CC) $(CFLAGS) 1-4.c -o 1-4.exe
	$(CC) $(CFLAGS) 1-5.c -o 1-5.exe
	$(CC) $(CFLAGS) 1-6.c -o 1-6.exe
	$(CC) $(CFLAGS) 1-7.c -o 1-7.exe
	$(CC) $(CFLAGS) 1-8.c -o 1-8.exe
	$(CC) $(CFLAGS) 2-1.c -o 2-1.exe
	$(CC) $(CFLAGS) 2-2.c -o 2-2.exe
	$(CC) $(CFLAGS) 2-3.c -o 2-3.exe
	$(CC) $(CFLAGS) 2-4.c -o 2-4.exe
	$(CC) $(CFLAGS) 2-5.c -o 2-5.exe
	$(CC) $(CFLAGS) 2-6.c -o 2-6.exe
	$(CC) $(CFLAGS) 2-7.c -o 2-7.exe
	$(CC) $(CFLAGS) 2-8.c -o 2-8.exe
	$(CC) $(CFLAGS) 3-1.c -o 3-1.exe
	$(CC) $(CFLAGS) 3-2.c -o 3-2.exe
	$(CC) $(CFLAGS) 3-4.c -o 3-4.exe
	$(CC) $(CFLAGS) 3-5.c -o 3-5.exe
	$(CC) $(CFLAGS) 3-6.c -o 3-6.exe
	$(CC) $(CFLAGS) 3-7.c -o 3-7.exe
	$(CC) $(CFLAGS) 3-8.c -o 3-8.exe
	$(CC) $(CFLAGS) 4-1.c -o 4-1.exe
	$(CC) $(CFLAGS) 4-2.c -o 4-2.exe
	$(CC) $(CFLAGS) 4-3.c -o 4-3.exe
	$(CC) $(CFLAGS) 4-4.c -o 4-4.exe
	$(CC) $(CFLAGS) 4-5.c -o 4-5.exe
	$(CC) $(CFLAGS) 5-1.c -o 5-1.exe

clean:
	rm -f *.exe
	rm -rf *.exe.dSYM