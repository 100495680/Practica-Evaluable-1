#include "claves.h"
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_MSG_SIZE sizeof(struct peticion)  // Define el tamaño máximo del mensaje
#define MAX_QUEUE_MESSAGES 10  // Número máximo de mensajes en la cola

void tratar_peticion(struct peticion *p) {
    struct respuesta r;
    printf("Procesando operación: %d para la clave: %d\n", p->op, p->key);

    switch (p->op) {
        case 0:
            printf("Ejecutando destroy()\n");
            r.status = destroy();
            break;
        case 1:
            printf("Ejecutando set_value() para key=%d\n", p->key);
            r.status = set_value(p->key, p->value1, p->N_value2, p->V_value2, p->value3);
            break;
        case 2:
            printf("Ejecutando get_value() para key=%d\n", p->key);
            r.status = get_value(p->key, r.value1, &r.N_value2, r.V_value2, &r.value3);
            break;
        default:
            printf("Operación no reconocida: %d\n", p->op);
            r.status = -1;
    }

    // Abrir la cola del cliente para enviar la respuesta
    int qr = mq_open(p->q_name, O_WRONLY);
    if (qr == -1) {
        perror("Error al abrir la cola del cliente para responder");
        return;
    }

    printf("Enviando respuesta al cliente, tamaño: %lu bytes\n", sizeof(struct respuesta));

    if (mq_send(qr, (char *)&r, sizeof(struct respuesta), 0) == -1) {
        perror("Error al enviar la respuesta al cliente");
    }

    mq_close(qr);
}


int main() {
    struct peticion p;
    unsigned int prio;

    // Definir atributos de la cola de mensajes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_QUEUE_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Crear la cola del servidor con el tamaño adecuado
    int qs = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0700, &attr);
    if (qs == -1) {
        perror("Error al abrir la cola del servidor");
        return -1;
    }

    printf("Servidor iniciado y esperando peticiones...\n");

    while (1) {
        if (mq_receive(qs, (char *)&p, MAX_MSG_SIZE, &prio) == -1) {
            perror("Error al recibir petición en el servidor");
            continue;
        }

        printf("Servidor recibió petición: op=%d, key=%d, value1=%s\n",
               p.op, p.key, p.value1);

        tratar_peticion(&p);
    }
}

