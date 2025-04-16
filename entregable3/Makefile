# Variables
CC = gcc
CFLAGS = -Wall -g -pthread -fPIC -I/usr/include/tirpc
LDLIBS = -ltirpc
LIBNAME = libclaves.so

CLIENTES = app-cliente app-cliente1 app-cliente2 app-cliente3 app-cliente4 app-cliente5
SERVIDOR = servidor

PROXY_OBJ = proxy-rpc.o claves_rpc_clnt.o claves_rpc_xdr.o
SRV_OBJ = claves.o claves_rpc_server.o claves_rpc_svc.o claves_rpc_xdr.o

.PHONY: all clean

all: $(LIBNAME) $(CLIENTES) $(SERVIDOR)

# Biblioteca compartida (para clientes)
$(LIBNAME): $(PROXY_OBJ)
	$(CC) -shared -o $@ $^ $(LDLIBS)

# Compilar todos los clientes
%: %.c $(LIBNAME)
	$(CC) $(CFLAGS) -o $@ $< -L. -lclaves $(LDLIBS)

# Compilar el servidor (usando claves_rpc_svc.c)
$(SERVIDOR): $(SRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Objetos
claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -c $<

proxy-rpc.o: proxy-rpc.c claves_rpc.h claves.h
	$(CC) $(CFLAGS) -c $<

claves_rpc_clnt.o: claves_rpc_clnt.c claves_rpc.h
	$(CC) $(CFLAGS) -c $<

claves_rpc_svc.o: claves_rpc_svc.c claves_rpc.h
	$(CC) $(CFLAGS) -c $<

claves_rpc_server.o: claves_rpc_server.c claves_rpc.h claves.h
	$(CC) $(CFLAGS) -c $<

claves_rpc_xdr.o: claves_rpc_xdr.c claves_rpc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.so $(CLIENTES) $(SERVIDOR)
