#include "claves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_BUFFER 2048

// Lee del socket una cadena terminada en nulo ('\0')
int readLine(int sd, char *buffer, size_t max) {
    size_t pos = 0;
    char c;
    while (pos < max - 1) {
        if (read(sd, &c, 1) <= 0) return -1;
        if (c == '\0') break;
        buffer[pos++] = c;
    }
    buffer[pos] = '\0';
    return 0;
}

// Envía por el socket una cadena terminada en nulo ('\0')
int writeLine(int sd, const char *str) {
    size_t len = strlen(str) + 1;
    return write(sd, str, len) == len ? 0 : -1;
}

// Función ejecutada por cada hilo que atiende a un cliente
void *tratar_cliente(void *arg) {
    int sd = *(int *)arg;
    free(arg);
    char buffer[MAX_BUFFER];

    if (readLine(sd, buffer, MAX_BUFFER) < 0) {
        close(sd);
        return NULL;
    }

    int op, key, N;
    char value1[MAX_STRING];
    double V_value2[MAX_VECTOR];
    struct Coord value3;
    char respuesta[MAX_BUFFER];

    // Interpretar la operación
    sscanf(buffer, "%d", &op);
    char *ptr = strchr(buffer, ' ') + 1;

    int status = -1;

    switch (op) {
        case 0:
            status = destroy();
            break;
        case 1:
        case 3:
            sscanf(ptr, "%d %s %d", &key, value1, &N);
            ptr = strchr(ptr, ' ') + 1; // key
            ptr = strchr(ptr, ' ') + 1; // value1
            ptr = strchr(ptr, ' ') + 1; // N
            for (int i = 0; i < N; ++i) {
                V_value2[i] = atof(ptr);
                ptr = strchr(ptr, ' ') + 1;
            }
            value3.x = atoi(ptr);
            ptr = strchr(ptr, ' ') + 1;
            value3.y = atoi(ptr);
            if (op == 1)
                status = set_value(key, value1, N, V_value2, value3);
            else
                status = modify_value(key, value1, N, V_value2, value3);
            break;
        case 2:
            sscanf(ptr, "%d", &key);
            status = get_value(key, value1, &N, V_value2, &value3);
            if (status == 0) {
                snprintf(respuesta, MAX_BUFFER, "%d %s %d", status, value1, N);
                for (int i = 0; i < N; ++i)
                    snprintf(respuesta + strlen(respuesta), MAX_BUFFER - strlen(respuesta), " %lf", V_value2[i]);
                snprintf(respuesta + strlen(respuesta), MAX_BUFFER - strlen(respuesta), " %d %d", value3.x, value3.y);
                writeLine(sd, respuesta);
                close(sd);
                return NULL;
            }
            break;
        case 4:
            sscanf(ptr, "%d", &key);
            status = delete_key(key);
            break;
        case 5:
            sscanf(ptr, "%d", &key);
            status = exist(key);
            break;
        default:
            status = -1;
    }

    snprintf(respuesta, MAX_BUFFER, "%d", status);
    writeLine(sd, respuesta);
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
