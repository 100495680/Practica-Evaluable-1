/* Este es el código del cliente de este habrá multiples copias y interactuara como si fuese una API 
(conversiones en la lib dinamica) manda los mensajes al proxy y recibe las respuestas*/

#include "claves.h"

// Ld_library path
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>

// Como en una red de ordenadores (lo que tratamos de emular) al principio de la ejecución se manda un discover
// De forma que se conozca la ip de los nuevos elementos. Uso el nombre de Proxy por simplicidad aun que no es correcto
// en esta aproximación.

struct Peticion {
    long ip_add; // Vamos a imitar la comunicacion de un sevidor real
    long proxy_add;
    char * operation;  // Código de operación POSIX
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};

struct Respuesta {
    long ip_add; // Vamos a imitar la comunicacion de un sevidor real
    long proxy_add;
    int status;  // 0 para éxito, -1 para error lógico, -2 para error de comunicación
};

struct tupla
{
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};

struct Respuesta leerRespuestaServidor(mqd_t mq){
    
    struct Respuesta msg;
    mq_receive(mq, (char *)&msg, sizeof(msg), 0);
    

    printf("Respuesta en Cliente recibido\n");

    return msg;
}

int mandarMensajeServidor( struct Peticion msg, mqd_t mq) {
    if (mq_send(mq, (char *)&msg, sizeof(msg),0) == -1)
    { perror("Client MQ send error"); exit(1);}

    printf("Mensaje enviado: %s\n", msg.value1);
    return 0;
}

struct Peticion encapsulador ( struct tupla tpl, char* operation) {
    struct Peticion msg;
    strcpy(msg.operation, operation);
    msg.key = tpl.key;
    strcpy(msg.value1, tpl.value1);
    msg.N_value2 = tpl.N_value2;
    memcpy(msg.V_value2, tpl.V_value2, sizeof(tpl.V_value2)); 
    msg.value3 = tpl.value3;
    return msg;
}



int main() {
    
    mqd_t mq = mq_open("/colaClientes", O_CREAT | O_RDWR, 0666, NULL);
    if (mq == -1){
        perror("Error en la creacion de la cola\n");
        exit(1);
    }
    struct tupla tpl;
    char* operacion;

    strcpy(operacion, "destroy");


    tpl.key = 0;
    strcpy(tpl.value1, "Tupla");
    tpl.N_value2 = 2;
    tpl.V_value2[0] = 2.0;
    tpl.V_value2[1] = 3.0;
    tpl.value3.x = 2;
    tpl.value3.y = 4;

    mandarMensajeServidor( encapsulador (tpl, operacion), mq);
}