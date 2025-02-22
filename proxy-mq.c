/* Este archivo es como el servidor gateway que recibe las peticiones del cliente o clientes y crea la cola de mensajes
que manda al servidor para que ejecute los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct Peticion {
    long numero_serie;  // Tipo de mensaje
    int operation;  // Código de operación POSIX
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};

struct Respuesta {
    long numero_serie;
    int status;  // 0 para éxito, -1 para error lógico, -2 para error de comunicación
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};


int main() {
    key_t clave = ftok("cola.msg", 65);  // Generar clave única
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola

    struct Peticion msg;
    msg.numero_serie = 1;  // Tipo de mensaje
    strcpy(msg.value1, "Hola, este es un mensaje!");

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Mensaje enviado: %s\n", msg.value1);

    return 0;
}
