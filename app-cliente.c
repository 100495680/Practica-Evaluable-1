/* Este es el código del cliente de este habrá multiples copias y interactuara como si fuese una API 
(conversiones en la lib dinamica) manda los mensajes al proxy y recibe las respuestas*/

#include "claves.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// Como en una red de ordenadores (lo que tratamos de emular) al principio de la ejecución se manda un discover
// De forma que se conozca la ip de los nuevos elementos. Uso el nombre de Proxy por simplicidad aun que no es correcto
// en esta aproximación.
int mandarDiscover()    {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única para la cola de mensajes el número 22 es arbitrario
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola
    long msg = getpid();
    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Proxy discover enviado: ");

    return 0;
}

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

struct Respuesta leerRespuestaServidor(){
    key_t clave = ftok("cola.msg", 22);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    struct Respuesta msg;
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);  // Recibir mensaje tipo 1

    printf("Respuesta en Cliente recibido\n");

    return msg;
}

int mandarMensajeServidor( struct Peticion msg) {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola

    msg.numero_serie = 1;  // Tipo de mensaje

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Mensaje enviado: %s\n", msg.value1);

    return 0;
}

struct Peticion encapsulador ( struct tupla tpl, char* operation) {
    struct Peticion msg;
    msg.numero_serie = 0;
    strcpy(msg.operation, operation);
    msg.key = tpl.key;
    strcpy(msg.value1, tpl.value1);
    msg.N_value2 = tpl.N_value2;
    memcpy(msg.V_value2, tpl.V_value2, sizeof(tpl.V_value2)); 
    msg.value3 = tpl.value3;
    return msg;
}



int main() {
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

    mandarMensajeServidor( encapsulador (tpl, operacion));
}