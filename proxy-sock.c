#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER 2048

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



// Función principal de envío/recepción con el servidor a través del socket TCP
int send_recv_text(const char *mensaje, char *respuesta) {
    
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

    // Enviar petición y recibir respuesta
    if (sendMessage(sd, buffer, sizeof(buffer)+1)) {
        close(sd);
        return -2;
    }

    if (recvMessage(sd, buffer, sizeof(struct respuesta))) { // La structura respuesta ocupa 528 bytes 
        close(sd);
        return -2;
    }

    close(sd);  // Cerrar socket
    return 0;
}

// Implementación de set_value usando protocolo textual
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > MAX_VECTOR || strlen(value1) >= MAX_STRING) return -1;

    char mensaje[MAX_BUFFER] = {0};
    char respuesta[MAX_BUFFER];

    // Construir la cadena de petición: "1 key value1 N v2[0] v2[1] ... v3.x v3.y"
    snprintf(mensaje, MAX_BUFFER, "1 %d %s %d", key, value1, N_value2);
    for (int i = 0; i < N_value2; ++i)
        snprintf(mensaje + strlen(mensaje), MAX_BUFFER - strlen(mensaje), " %lf", V_value2[i]);
    snprintf(mensaje + strlen(mensaje), MAX_BUFFER - strlen(mensaje), " %d %d", value3.x, value3.y);

    int err = send_recv_text(mensaje, respuesta);
    return err < 0 ? err : atoi(respuesta);
}

// Implementación de get_value con lectura de campos individuales
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    char mensaje[MAX_BUFFER];
    char respuesta[MAX_BUFFER];

    snprintf(mensaje, MAX_BUFFER, "2 %d", key);  // operación 2: get
    int err = send_recv_text(mensaje, respuesta);
    if (err < 0) return err;

    int status, n;
    sscanf(respuesta, "%d %s %d", &status, value1, &n);  // leer status, value1 y N_value2
    *N_value2 = n;

    // Avanzar punteros para leer el vector de doubles y value3
    char *ptr = strchr(respuesta, ' ') + 1; // saltar status
    ptr = strchr(ptr, ' ') + 1;             // saltar value1
    ptr = strchr(ptr, ' ') + 1;             // saltar N

    for (int i = 0; i < n; ++i) {
        V_value2[i] = atof(ptr);
        ptr = strchr(ptr, ' ') + 1;
    }
    value3->x = atoi(ptr);
    ptr = strchr(ptr, ' ') + 1;
    value3->y = atoi(ptr);

    return status;
}

// Implementación de modify_value similar a set_value
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > MAX_VECTOR || strlen(value1) >= MAX_STRING) return -1;

    char mensaje[MAX_BUFFER] = {0};
    char respuesta[MAX_BUFFER];

    snprintf(mensaje, MAX_BUFFER, "3 %d %s %d", key, value1, N_value2); // operación 3: modify
    for (int i = 0; i < N_value2; ++i)
        snprintf(mensaje + strlen(mensaje), MAX_BUFFER - strlen(mensaje), " %lf", V_value2[i]);
    snprintf(mensaje + strlen(mensaje), MAX_BUFFER - strlen(mensaje), " %d %d", value3.x, value3.y);

    int err = send_recv_text(mensaje, respuesta);
    return err < 0 ? err : atoi(respuesta);
}

// Enviar petición para borrar una clave
int delete_key(int key) {
    char mensaje[MAX_BUFFER];
    char respuesta[MAX_BUFFER];
    snprintf(mensaje, MAX_BUFFER, "4 %d", key);  // operación 4: delete
    int err = send_recv_text(mensaje, respuesta);
    return err < 0 ? err : atoi(respuesta);
}

// Consultar existencia de clave
int exist(int key) {
    char mensaje[MAX_BUFFER];
    char respuesta[MAX_BUFFER];
    snprintf(mensaje, MAX_BUFFER, "5 %d", key);  // operación 5: exist
    int err = send_recv_text(mensaje, respuesta);
    return err < 0 ? err : atoi(respuesta);
}

// Petición para destruir todas las tuplas
int destroy() {
    char respuesta[MAX_BUFFER];
    int err = send_recv_text("0", respuesta);  // operación 0: destroy
    return err < 0 ? err : atoi(respuesta);
}
