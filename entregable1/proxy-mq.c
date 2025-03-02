#include "claves.h"
#include <mqueue.h>
#include <unistd.h>
#include <string.h>

int send_recv(struct peticion *p, struct respuesta *r) {
    char qr_name[1024];
    unsigned int prio = 0;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct respuesta);
    attr.mq_curmsgs = 0;

    sprintf(qr_name, "%s%d", "/CLIENTE_", getpid());

    int qs = mq_open(SERVER_QUEUE, O_WRONLY);
    if (qs == -1) {
        perror("Error al abrir la cola del servidor desde el cliente");
        return -2;
    }

    int qr = mq_open(qr_name, O_CREAT | O_RDONLY, 0700, &attr);
    if (qr == -1) {
        perror("Error al crear la cola de respuesta del cliente");
        mq_close(qs);
        return -2;
    }

    strcpy(p->q_name, qr_name);

    if (mq_send(qs, (char *)p, sizeof(struct peticion), 0) == -1) {
        perror("Error al enviar la petición al servidor");
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -2;
    }

    printf("Esperando respuesta del servidor, tamaño esperado: %lu bytes\n", sizeof(struct respuesta));

    if (mq_receive(qr, (char *)r, sizeof(struct respuesta), &prio) == -1) {
        perror("Error al recibir la respuesta del servidor");
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -2;
    }

    printf("Respuesta recibida con status: %d\n", r->status);

    mq_close(qs);
    mq_close(qr);
    mq_unlink(qr_name);

    return r->status;
}


// Implementación de get_value
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    struct peticion p = {0};  // Inicializa toda la estructura a 0
    p.op = 2;
    p.key = key;

    struct respuesta r;

    int status = send_recv(&p, &r);
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
