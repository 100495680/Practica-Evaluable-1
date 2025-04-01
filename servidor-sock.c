#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>

#define MAX_BUFFER 2048

// Funciones tipo de mandado y recepción de mensajes

int sendMessage(int socket, char * buffer, int len)
{
	int r;
	int l = len;
		

	do {	
		r = write(socket, buffer, l);
		l = l -r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fail */
	else
		return(0);	/* full length has been sent */
}
int recvMessage(int socket, char *buffer, int len)
{
	int r;
	int l = len;
		

	do {	
		r = read(socket, buffer, l);
		l = l -r ;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fallo */
	else
		return(0);	/* full length has been receive */
}


// Estas funciones sirven para compatibilidad con equipos diferentes su función es pasar los mensajes
// De una forma que cualquiera con el servidor lo pueda leer dando igual sea un ordenador de 32 o 64 bits

void sacar_peticion_buffer(char *buffer, struct peticion *p) {
    sscanf(buffer, "%d %d %s %d", &p->op, &p->key, p->value1, &p->N_value2); // Sacamos el código del buffer y lo metemos en la estructura directamente
    char *ptr = buffer;
    // Necesitamos un código especial para V_value2 ya que no tiene un tamaño constante
    for (int i = 0; i < p->N_value2; i++) {
        ptr = strchr(ptr, ' ') + 1;
        sscanf(ptr, "%lf", &p->V_value2[i]); // El valor en bytes de un double siempre es constante
    }
    ptr = strchr(ptr, ' ') + 1;
    sscanf(ptr, "%d %d %s", &p->value3.x, &p->value3.y, p->q_name);
}

void meter_respuesta_buffer(char *buffer, struct respuesta *r) {
    sprintf(buffer, "%d %s %d", r->status, r->value1, r->N_value2); // Sacamos el código de la estructura y lo metemos en el buffer como un string
    int offset = strlen(buffer);
    // Necesitamos un código especial para V_value2 ya que no tiene un tamaño constante
    for (int i = 0; i < r->N_value2; i++) {
        offset += sprintf(buffer + offset, " %lf", r->V_value2[i]); // El valor en bytes de un double siempre es constante
    }
    sprintf(buffer + offset, " %d %d", r->value3.x, r->value3.y);
}


// Función ejecutada por cada hilo que atiende a un cliente

void *tratar_cliente(void *arg) {
    int sd = *(int *)arg;
    free(arg);
    char buffer[MAX_BUFFER];

    if (recvMessage(sd, buffer, sizeof(struct peticion)) < 0) {
        close(sd);
        return NULL;
    }

    struct peticion p;
    sacar_peticion_buffer(buffer, &p);
    struct respuesta r;
    // printf("Procesando operación: %d para la clave: %d\n", p.op, p.key);

    switch (p.op) {
        case 0:
            // printf("Ejecutando destroy()\n");
            r.status = destroy();
            break;
        case 1:
            // printf("Ejecutando set_value() para key=%d\n", p.key);
            r.status = set_value(p.key, p.value1, p.N_value2, p.V_value2, p.value3);
            break;
        case 2:
            // printf("Ejecutando get_value() para key=%d\n", p.key);
            r.status = get_value(p.key, r.value1, &r.N_value2, r.V_value2, &r.value3);
            break;
        case 3:
            // printf("Ejecutando modify_value() para key=%d\n", p.key);
            r.status = modify_value(p.key, p.value1, p.N_value2, p.V_value2, p.value3);
            break;
        case 4:
            // printf("Ejecutando delete() para key=%d\n", p.key);
            r.status = delete_key(p.key);
            break;
        case 5:
            // printf("Ejecutando exist() para key=%d\n", p.key);
            r.status = exist(p.key);
            break;
        default:
            // printf("Operación no reconocida: %d\n", p.op);
            r.status = -1;
    }

    meter_respuesta_buffer(buffer, &r);
    sendMessage(sd, buffer, (size_t)sizeof(r));
    close(sd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    int server_sd, *client_sd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd < 0) {
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(puerto);

    if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(server_sd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sd, 10) < 0) {
        perror("Error en listen");
        close(server_sd);
        exit(EXIT_FAILURE);
    }


    while (1) {
        client_sd = malloc(sizeof(int));
        *client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
        if (*client_sd < 0) {
            perror("Error en accept");
            free(client_sd);
            continue;
        }
        pthread_t hilo;
        pthread_create(&hilo, NULL, tratar_cliente, client_sd);
        pthread_detach(hilo);  // Liberar recursos automáticamente
    }

    close(server_sd);
    return 0;
}
