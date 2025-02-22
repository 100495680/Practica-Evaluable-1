/* Este archivo es como el servidor gateway que recibe las peticiones del cliente o clientes y crea la cola de mensajes
que manda al servidor para que ejecute los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct Peticion {
    long numero_serie;  // Tipo de mensaje
    char* operation;  // Código de operación POSIX
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};

struct Respuesta {
    long numero_serie;
    int status;  // 0 para éxito, -1 para error lógico, -2 para error de comunicación
};

struct Peticion leerMensajesCliente()  {
    key_t clave = ftok("cola.msg", 22);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    struct Peticion msg;
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);  // Recibir mensaje tipo 1

    printf("Mensaje en Proxy recibido: %s\n", msg.value1);


    return msg;
}

int mandarMensajesServidor(struct Peticion msg)    {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única para la cola de mensajes el número 22 es arbitrario
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Mensaje en Proxy enviado: %s\n", msg.value1);

    return 0;
}

struct Respuesta leerRespuestaServidor()  {
    key_t clave = ftok("cola.msg", 22);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    struct Respuesta msg;
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);  // Recibir mensaje tipo 1

    printf("Respuesta en Proxy recibido\n");

    return msg;
}

int mandarRespuestaCliente(struct Respuesta msg)    {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única para la cola de mensajes el número 22 es arbitrario
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Respuesta en Proxy enviado\n");

    return 0;
}



int main() {
    struct Peticion msg;
    msg = leerMensajesCliente();
    mandarMensajesServidor(msg);

    struct Respuesta rp;
    rp = leerRespuestaServidor();
    mandarRespuestaCliente(rp);
    
}
