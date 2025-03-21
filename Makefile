# Variables
CC = gcc
CFLAGS = -Wall -g -pthread
LIBNAME = libclaves.so

# Archivos fuente
CLIENTE = app-cliente
SERVIDOR = servidor-sock

# Objetos
PROXY_OBJ = proxy-sock.o

.PHONY: all clean

all: $(LIBNAME) $(CLIENTE) $(SERVIDOR)

# Biblioteca compartida solo con proxy
$(LIBNAME): $(PROXY_OBJ)
	$(CC) -shared -o $@ $^

# Cliente enlazado con la biblioteca
$(CLIENTE): app-cliente.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ app-cliente.c -L. -lclaves

# Servidor con claves.c real
$(SERVIDOR): servidor-sock.c claves.o
	$(CC) $(CFLAGS) -o $@ servidor-sock.c claves.o

# Compilar objetos
proxy-sock.o: proxy-sock.c claves.h
	$(CC) $(CFLAGS) -fPIC -c proxy-sock.c

claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -fPIC -c claves.c

clean:
	rm -f *.o *.so $(CLIENTE) $(SERVIDOR)
