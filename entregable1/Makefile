CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lrt

all: servidor cliente

servidor: servidor-mq.c claves.c
	$(CC) $(CFLAGS) -o servidor servidor-mq.c claves.c $(LDFLAGS)

cliente: proxy-mq.c app-cliente.c
	$(CC) $(CFLAGS) -shared -o libclaves.so proxy-mq.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o cliente app-cliente.c -L. -lclaves $(LDFLAGS)

clean:
	rm -f servidor cliente libclaves.so
