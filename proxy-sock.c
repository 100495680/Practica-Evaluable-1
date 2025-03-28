#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define MAX_BUFFER 2048

// Lee del socket hasta encontrar el caracter nulo ('\0') y lo guarda en buffer
ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;  /* num of bytes fetched by last read() */
    size_t totRead;	  /* total bytes read so far */
    char *buf;
    char ch;


    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;
    totRead = 0;

    for (;;) {
        numRead = read(fd, &ch, 1);	/* read a byte */

        if (numRead == -1) {
            if (errno == EINTR)	/* interrupted -> restart read() */
                continue;
            else
                return -1;		/* some other error */
        } else if (numRead == 0) {	/* EOF */
            if (totRead == 0)	/* no byres read; return 0 */
                return 0;
            else
                break;
        } else {			/* numRead must be 1 if we get here*/
            if (ch == '\n')
                break;
            if (ch == '\0')
                break;
            if (totRead < n - 1) {		/* discard > (n-1) bytes */
                totRead++;
                *buf++ = ch;
            }
        }
    }

    *buf = '\0';
    return totRead;
}


// Envía por el socket una cadena terminada en nulo ('\0')
int writeLine(int sd, const char *str) {
    size_t len = strlen(str) + 1;
    return write(sd, str, len) == len ? 0 : -1;
}

// Función principal de envío/recepción con el servidor a través del socket TCP
int send_recv_text(const char *mensaje, char *respuesta) {
    char *ip = getenv("IP_TUPLAS");
    char *port_str = getenv("PORT_TUPLAS");
    if (!ip || !port_str) {
        fprintf(stderr, "Variables de entorno IP_TUPLAS o PORT_TUPLAS no definidas\n");
        return -2;
    }

    int puerto = atoi(port_str);
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int sd;

    // Obtener información del host
    hp = gethostbyname(ip);
    if (!hp) return -2;

    // Crear el socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) return -2;

    // Inicializar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);

    // Conectar con el servidor
    if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sd);
        return -2;
    }

    // Enviar petición y recibir respuesta
    if (writeLine(sd, mensaje) < 0 || readLine(sd, respuesta, MAX_BUFFER) < 0) {
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
