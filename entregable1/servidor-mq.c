#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include "claves.h"

#define QUEUE_NAME "/cola_servidor"
#define RESPONSE_QUEUE_NAME "/cola_respuesta"
#define MAX_MSG_SIZE 1024

void procesar_mensaje(char *mensaje, char *respuesta) {
    char comando[20];
    int key, N_value2;
    char value1[MAX_STRING];
    double V_value2[MAX_VECTOR];
    struct Coord value3;
    int resultado;

    sscanf(mensaje, "%s", comando);

    if (strcmp(comando, "SET") == 0) {
        sscanf(mensaje, "SET %d %s %d", &key, value1, &N_value2);
        char *ptr = strchr(mensaje, ' ') + 1; // Saltar el primer espacio
        ptr = strchr(ptr, ' ') + 1; // Saltar el segundo espacio
        ptr = strchr(ptr, ' ') + 1; // Saltar el tercer espacio

        for (int i = 0; i < N_value2; i++) {
            sscanf(ptr, "%lf", &V_value2[i]);
            ptr = strchr(ptr, ' ') + 1;
        }
        sscanf(ptr, "%d %d", &value3.x, &value3.y);

        resultado = set_value(key, value1, N_value2, V_value2, value3);
    } else if (strcmp(comando, "GET") == 0) {
        sscanf(mensaje, "GET %d", &key);
        resultado = get_value(key, value1, &N_value2, V_value2, &value3);
        if (resultado == 0) {
            snprintf(respuesta, MAX_MSG_SIZE, "%s %d", value1, N_value2);
            for (int i = 0; i < N_value2; i++) {
                snprintf(respuesta + strlen(respuesta), MAX_MSG_SIZE - strlen(respuesta), " %lf", V_value2[i]);
            }
            snprintf(respuesta + strlen(respuesta), MAX_MSG_SIZE - strlen(respuesta), " %d %d", value3.x, value3.y);
        } else {
            strcpy(respuesta, "ERROR");
        }
    } else if (strcmp(comando, "MODIFY") == 0) {
        sscanf(mensaje, "MODIFY %d %s %d", &key, value1, &N_value2);
        char *ptr = strchr(mensaje, ' ') + 1;
        ptr = strchr(ptr, ' ') + 1;
        ptr = strchr(ptr, ' ') + 1;

        for (int i = 0; i < N_value2; i++) {
            sscanf(ptr, "%lf", &V_value2[i]);
            ptr = strchr(ptr, ' ') + 1;
        }
        sscanf(ptr, "%d %d", &value3.x, &value3.y);

        resultado = modify_value(key, value1, N_value2, V_value2, value3);
    } else if (strcmp(comando, "DELETE") == 0) {
        sscanf(mensaje, "DELETE %d", &key);
        resultado = delete_key(key);
    } else if (strcmp(comando, "EXIST") == 0) {
        sscanf(mensaje, "EXIST %d", &key);
        resultado = exist(key);
    } else if (strcmp(comando, "DESTROY") == 0) {
        resultado = destroy();
    } else {
        resultado = -1;
    }

    snprintf(respuesta, MAX_MSG_SIZE, "%d", resultado);
}

int main() {
    mqd_t mq, mq_resp;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    char respuesta[MAX_MSG_SIZE];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == -1) {
        perror("Error al abrir la cola de mensajes del servidor");
        return 1;
    }

    mq_resp = mq_open(RESPONSE_QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq_resp == -1) {
        perror("Error al abrir la cola de respuesta");
        mq_close(mq);
        return 1;
    }

    printf("Servidor esperando peticiones...\n");

    while (1) {
        ssize_t bytes_leidos = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_leidos >= 0) {
            buffer[bytes_leidos] = '\0';
            printf("Mensaje recibido: %s\n", buffer);

            procesar_mensaje(buffer, respuesta);

            if (mq_send(mq_resp, respuesta, strlen(respuesta) + 1, 0) == -1) {
                perror("Error al enviar respuesta al cliente");
            }
        } else {
            perror("Error al recibir mensaje");
        }
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    mq_close(mq_resp);
    mq_unlink(RESPONSE_QUEUE_NAME);

    return 0;
}
