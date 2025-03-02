CC = gcc
CFLAGS = -Wall -Wextra -pthread -lrt -fPIC
LDFLAGS = -lrt -pthread

# Archivos fuente
SRV_SRC = servidor-mq.c claves.c
CLI_SRC = app-cliente.c
PROXY_SRC = proxy-mq.c

# Ejecutables y biblioteca compartida
SERVER = servidor
CLIENT = cliente
LIBRARY = libclaves.so

all: $(SERVER) $(LIBRARY) $(CLIENT)

# Compilar el servidor
$(SERVER): $(SRV_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SRV_SRC) $(LDFLAGS)

# Compilar el proxy y generar la librería compartida
$(LIBRARY): $(PROXY_SRC)
	$(CC) $(CFLAGS) -shared -o $(LIBRARY) $(PROXY_SRC) $(LDFLAGS)

# Compilar el cliente con la librería compartida
$(CLIENT): $(CLI_SRC) $(LIBRARY)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLI_SRC) -L. -lclaves $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT) $(LIBRARY) *.o

rebuild: clean all
