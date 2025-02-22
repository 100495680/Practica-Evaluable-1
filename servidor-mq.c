/* Este archivo es como el servidor que recibe las peticiones y le pide a claves.c que ejecute los cambios
los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
    key_t clave = ftok("cola.msg", 65);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    struct Peticion msg;
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);  // Recibir mensaje tipo 1

    printf("Mensaje recibido: %s\n", msg.value1);

    return 0;
}
