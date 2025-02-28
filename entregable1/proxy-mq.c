#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include "claves.h"

#define QUEUE_NAME "/cola_servidor"
#define RESPONSE_QUEUE_NAME "/cola_respuesta"
#define MAX_MSG_SIZE 1024

int send_message(const char *msg, char *response) {
    mqd_t mq, mq_resp;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];

    // Abrir la cola de mensajes para enviar peticiones
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == -1) {
        perror("Error al abrir la cola de mensajes del servidor");
        return -2;
    }

    // Enviar la solicitud
    if (mq_send(mq, msg, strlen(msg) + 1, 0) == -1) {
        perror("Error al enviar mensaje al servidor");
        mq_close(mq);
        return -2;
    }
    mq_close(mq);

    // Abrir la cola de mensajes para recibir respuesta
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq_resp = mq_open(RESPONSE_QUEUE_NAME, O_RDONLY | O_CREAT, 0644, &attr);
    if (mq_resp == -1) {
        perror("Error al abrir la cola de respuesta");
        return -2;
    }

    // Esperar respuesta del servidor
    ssize_t bytes_read = mq_receive(mq_resp, buffer, MAX_MSG_SIZE, NULL);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0';
        strcpy(response, buffer);
        printf("📩 Respuesta recibida: %s\n", response);  // 🔍 Agregar log
    } else {
        perror("Error al recibir respuesta del servidor");
        mq_close(mq_resp);
        mq_unlink(RESPONSE_QUEUE_NAME);
        return -2;
    }

    mq_close(mq_resp);
    mq_unlink(RESPONSE_QUEUE_NAME);
    return 0;
}



int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    char msg[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    snprintf(msg, sizeof(msg), "SET %d %s %d", key, value1, N_value2);
    for (int i = 0; i < N_value2; i++) {
        snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), " %lf", V_value2[i]);
    }
    snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), " %d %d", value3.x, value3.y);

    if (send_message(msg, response) == 0) {
        return atoi(response);
    }
    return -2;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    char msg[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    snprintf(msg, sizeof(msg), "GET %d", key);

    if (send_message(msg, response) == 0) {
        if (strcmp(response, "ERROR") == 0) return -1;
        sscanf(response, "%s %d", value1, N_value2);
        char *ptr = strchr(response, ' ') + 1; // Saltar el primer espacio
        ptr = strchr(ptr, ' ') + 1; // Saltar el segundo espacio

        for (int i = 0; i < *N_value2; i++) {
            sscanf(ptr, "%lf", &V_value2[i]);
            ptr = strchr(ptr, ' ') + 1;
        }
        sscanf(ptr, "%d %d", &value3->x, &value3->y);
        return 0;
    }
    return -2;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    char msg[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    snprintf(msg, sizeof(msg), "MODIFY %d %s %d", key, value1, N_value2);
    for (int i = 0; i < N_value2; i++) {
        snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), " %lf", V_value2[i]);
    }
    snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), " %d %d", value3.x, value3.y);

    if (send_message(msg, response) == 0) {
        return atoi(response);
    }
    return -2;
}

int delete_key(int key) {
    char msg[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    snprintf(msg, sizeof(msg), "DELETE %d", key);

    if (send_message(msg, response) == 0) {
        return atoi(response);
    }
    return -2;
}

int exist(int key) {
    char msg[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    snprintf(msg, sizeof(msg), "EXIST %d", key);

    if (send_message(msg, response) == 0) {
        return atoi(response);
    }
    return -2;
}

int destroy() {
    char msg[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    snprintf(msg, sizeof(msg), "DESTROY");

    if (send_message(msg, response) == 0) {
        return atoi(response);
    }
    return -2;
}
