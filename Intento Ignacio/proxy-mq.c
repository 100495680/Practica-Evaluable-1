/* Este archivo es como el servidor gateway que recibe las peticiones del cliente o clientes y crea la cola de mensajes
que manda al servidor para que ejecute los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>

// Como en una red de ordenadores (lo que tratamos de emular) al principio de la ejecución se manda un discover
// De forma que se conozca la ip de los nuevos elementos. Uso el nombre de Proxy por simplicidad aun que no es correcto
// en esta aproximación.

struct tupla
{
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};

struct Peticion encapsulador ( struct tupla tpl, char* operation) {
    struct Peticion msg;
    strcpy(msg.operation, operation);
    msg.key = tpl.key;
    strcpy(msg.value1, tpl.value1);
    msg.N_value2 = tpl.N_value2;
    memcpy(msg.V_value2, tpl.V_value2, sizeof(double) * tpl.N_value2); // Correct size for memcpy 
    msg.value3 = tpl.value3;
    return msg;
}

int mandarMensaje(struct Peticion msg, mqd_t mq)    {
    
    if (mq_send(mq, (char *)&msg, sizeof(msg),0) == -1)
    { perror("Client MQ send error"); exit(1);}
    printf("Mensaje en Proxy enviado: %s\n", msg.value1);

    return 0;
}

struct Peticion leerMensajesCliente(mqd_t mq, struct Peticion msg) {
    
    
    mq_receive(mq, (char *)&msg, sizeof(msg), 0);
    
    printf("Mensaje en Proxy recibido: %s\n", msg.value1);


    return msg;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3)
{
    mqd_t mq = mq_open("/colaEnvios", O_CREAT | O_RDWR, 0666, NULL);
    if (mq == -1)
    {
        perror("Error en la creacion de la cola\n");
        exit(1);
    }
    struct tupla tpl;
    char operacion[10]; // Allocate enough space for the operation string

    strcpy(operacion, "set_value");

    tpl.key = key;
    strcpy(tpl.value1, value1);
    tpl.N_value2 = N_value2;
    memcpy(tpl.V_value2, V_value2, sizeof(double) * N_value2); // Correct size for memcpy
    tpl.value3 = value3;

    mandarMensaje(encapsulador(tpl, operacion), mq);

    struct Peticion msg;
    msg = leerMensajesCliente(mq, msg);

    mq_close(mq);

    return 0; // Ensure the function returns an int
}



