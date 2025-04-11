#include "claves.h"
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>


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

void meter_peticion_buffer(char *buffer, struct peticion *p) {
    sprintf(buffer, "%d %d %s %d", p->op, p->key, p->value1, p->N_value2);   // Sacamos el código del buffer y lo metemos en la estructura directamente
    int offset = strlen(buffer);
    // Necesitamos un código especial para V_value2 ya que no tiene un tamaño constante
    for (int i = 0; i < p->N_value2; i++) {
        offset += sprintf(buffer + offset, " %lf", p->V_value2[i]); // El valor en bytes de un double siempre es constante
    }
    sprintf(buffer + offset, " %d %d %s", p->value3.x, p->value3.y, p->q_name);
}

void sacar_respuesta_buffer(char *buffer, struct respuesta *r) {
    sscanf(buffer, "%d %s %d", &r->status, r->value1, &r->N_value2);
    char *ptr = buffer;
    // Necesitamos un código especial para V_value2 ya que no tiene un tamaño constante
    for (int i = 0; i < r->N_value2; i++) {
        ptr = strchr(ptr, ' ') + 1;
        sscanf(ptr, "%lf", &r->V_value2[i]); // El valor en bytes de un double siempre es constante
    }
    ptr = strchr(ptr, ' ') + 1;
    sscanf(ptr, "%d %d", &r->value3.x, &r->value3.y);
}

// Función comun al todas las operaciones que contiene la lógica del mandado y recepción de mensajes

int send_recv(struct peticion *p, struct respuesta *r) {
    char *maquina; short puerto;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int sd, ret;

    maquina = getenv("IP_TUPLAS");
    puerto = (short)atoi(getenv("PORT_TUPLAS"));

    hp = gethostbyname(maquina);
    if (NULL == hp) {
        printf("ERROR en gethostbyname con '%s'\n", maquina) ;
        return -1 ;
    }

    // (1) creación del socket (NO tiene dirección asignada aquí)
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("ERROR en socket: ") ;
        return -1 ;
    }

    // (2) obtener la dirección
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);

    // (3) Solicitud de conexión (con socket remoto)
    // * si el socket local no tiene dirección asignada
    //   entonces se le asigna una automáticamente con puerto temporal
    ret = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) ;
    if (ret < 0) {
        perror("ERROR en connect: ");
        return -1;
    }

    char buffer[MAX_BUFFER];
    meter_peticion_buffer(buffer, p);

    // Enviar petición y recibir respuesta
    if (sendMessage(sd, buffer, sizeof(struct peticion))) {
        close(sd);
        return -2;
    }
        // Aquí creo que tiene que ir al buffer
    if (recvMessage(sd, buffer, (size_t)sizeof(struct respuesta)) < 0) { // La structura respuesta ocupa 528 bytes
        close(sd);
        return -2;
    }
    sacar_respuesta_buffer(buffer, r);

    close(sd);  // Cerrar socket
    return r->status;
}


// Implementación de get_value
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 2;
    p.key = key;

    struct respuesta r;

    int status = send_recv(&p, &r); //llama a la función anterior que gestiona el envío del mensaje y la answer
    if (status == 0) {
        strncpy(value1, r.value1, MAX_STRING);
        *N_value2 = r.N_value2;
        memcpy(V_value2, r.V_value2, (*N_value2) * sizeof(double));
        *value3 = r.value3;
    }

    return status;
}

// Implementación de set_value
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > MAX_VECTOR) {
        return -1;  // Error si el vector está fuera de rango
    }

    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 1;  // Código de operación para set_value
    p.key = key;
    strncpy(p.value1, value1, MAX_STRING - 1);
    p.value1[MAX_STRING - 1] = '\0';  // Asegurar terminación nula
    p.N_value2 = N_value2;
    memcpy(p.V_value2, V_value2, N_value2 * sizeof(double));
    p.value3 = value3;

    struct respuesta r;
    return send_recv(&p, &r);
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > MAX_VECTOR) {
        return -1;  // Error si el vector está fuera de rango
    }

    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 3;  // Código de operación para modify_value
    p.key = key;
    strncpy(p.value1, value1, MAX_STRING - 1);
    p.value1[MAX_STRING - 1] = '\0';  // Asegurar terminación nula
    p.N_value2 = N_value2;
    memcpy(p.V_value2, V_value2, N_value2 * sizeof(double));
    p.value3 = value3;

    struct respuesta r;

    return send_recv(&p, &r);
}

int destroy() {
    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 0;  // Código de operación para destroy
    
    struct respuesta r;

    return send_recv(&p, &r);
}

int exist(int key) {
    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 5;  // Código de operación para destroy
    p.key = key;

    struct respuesta r;

    return send_recv(&p, &r);
}

int delete_key(int key) {
    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 4;  // Código de operación para destroy
    p.key = key;

    struct respuesta r;

    return send_recv(&p, &r);
}