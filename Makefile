CC = gcc
CFLAGS = -Wall -Wextra -pthread -lrt -fPIC
LDFLAGS = -lrt -pthread

# Archivos fuente
SRV_SRC = servidor-mq.c claves.c
CLI_SRC_1 = app-cliente1.c
CLI_SRC_2 = app-cliente2.c
CLI_SRC_3 = app-cliente3.c
PROXY_SRC = proxy-mq.c

# Ejecutables y biblioteca compartida
SERVER = servidor
CLIENT_1 = cliente1
CLIENT_2 = cliente2
CLIENT_3 = cliente3

LIBRARY = libclaves.so

all: $(SERVER) $(LIBRARY) $(CLIENT_1) $(CLIENT_2) $(CLIENT_3)

# Compilar el servidor
$(SERVER): $(SRV_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SRV_SRC) $(LDFLAGS)

# Compilar el proxy y generar la librería compartida
$(LIBRARY): $(PROXY_SRC)
	$(CC) $(CFLAGS) -shared -o $(LIBRARY) $(PROXY_SRC) $(LDFLAGS)

# Compilar el cliente 1 con la librería compartida
$(CLIENT_1): $(CLI_SRC_1) $(LIBRARY)
	$(CC) $(CFLAGS) -o $(CLIENT_1) $(CLI_SRC_1) -L. -lclaves $(LDFLAGS)


# Compilar el cliente 2 con la librería compartida
$(CLIENT_2): $(CLI_SRC_2) $(LIBRARY)
	$(CC) $(CFLAGS) -o $(CLIENT_2) $(CLI_SRC_2) -L. -lclaves $(LDFLAGS)


# Compilar el cliente 3 con la librería compartida
$(CLIENT_3): $(CLI_SRC_3) $(LIBRARY)
	$(CC) $(CFLAGS) -o $(CLIENT_3) $(CLI_SRC_3) -L. -lclaves $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT) $(LIBRARY) *.o

rebuild: clean all
