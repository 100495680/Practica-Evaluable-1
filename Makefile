# Variables
CC = gcc
CFLAGS = -Wall -g -pthread
LIBNAME = libclaves.so

# Archivos fuente
CLIENTE = app-cliente
CLIENTE1 = app-cliente1
CLIENTE2 = app-cliente2
CLIENTE3 = app-cliente3
CLIENTE4 = app-cliente4
CLIENTE5 = app-cliente5

SERVIDOR = servidor-sock
PROXY = proxy-sock

# Objetos
PROXY_OBJ = proxy-sock.o

.PHONY: all clean

all: $(LIBNAME) $(CLIENTE) $(CLIENTE1) $(CLIENTE2) $(CLIENTE3) $(CLIENTE4) $(CLIENTE5) $(SERVIDOR)

# Biblioteca compartida solo con proxy
$(LIBNAME): $(PROXY_OBJ)
	$(CC) -shared -o $@ $^

# Cliente enlazado con la biblioteca
$(CLIENTE): app-cliente.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente.c -L. -lclaves

$(CLIENTE1): app-cliente1.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente1.c -L. -lclaves

$(CLIENTE2): app-cliente2.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente2.c -L. -lclaves

$(CLIENTE3): app-cliente3.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente3.c -L. -lclaves

$(CLIENTE4): app-cliente4.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente4.c -L. -lclaves

$(CLIENTE5): app-cliente5.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente5.c -L. -lclaves


# Servidor con claves.c real
$(SERVIDOR): servidor-sock.c claves.o
	$(CC) $(CFLAGS) -o $@ servidor-sock.c claves.o


# Servidor con claves.c real
$(PROXY): proxy-sock.c claves.h
	$(CC) $(CFLAGS) -fPIC -c proxy-sock.c

# Compilar objetos
proxy-sock.o: proxy-sock.c claves.h
	$(CC) $(CFLAGS) -fPIC -c proxy-sock.c

claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -fPIC -c claves.c

clean:
	rm -f *.o *.so $(CLIENTE) $(CLIENTE1) $(CLIENTE2) $(CLIENTE3) $(CLIENTE4) $(CLIENTE5) $(SERVIDOR)
