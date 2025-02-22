/* Este es el código del cliente de este habrá multiples copias y interactuara como si fuese una API 
(conversiones en la lib dinamica) manda los mensajes al proxy y recibe las respuestas*/

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

int main() {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola

    struct Peticion msg;
    msg.numero_serie = 1;  // Tipo de mensaje
    strcpy(msg.value1, "Mensaje del cliente");

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Mensaje enviado: %s\n", msg.value1);

    return 0;
}
