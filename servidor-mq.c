/* Este archivo es como el servidor que recibe las peticiones y le pide a claves.c que ejecute los cambios
los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"
#include "claves.c"

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

struct paquete
{
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

struct paquete sacarPaquete(struct Peticion msg) {
    struct paquete pkt;
    strcpy(pkt.value1, msg.value1);
    pkt.N_value2 = msg.N_value2;
    memcpy(pkt.V_value2, msg.V_value2, sizeof(msg.V_value2)); 
    pkt.value3 = msg.value3;
    return pkt;
};

struct Peticion leerMensajesCliente() {
    key_t clave = ftok("cola.msg", 22);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    struct Peticion msg;
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);  // Recibir mensaje tipo 1

    printf("Mensaje recibido: %s\n", msg.value1);

    return msg;
}

int mandarRespuestaCliente(int status, long numero_serie) {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única para la cola de mensajes el número 22 es arbitrario
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola

    struct Respuesta msg;
    msg.numero_serie = numero_serie;
    msg.status = status;

    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Respuesta de Servidor enviado\n");

    return 0;
}



int main() {
    struct Peticion msg;
    struct paquete elemento;

    msg = leerMensajesCliente();
    elemento = sacarPaquete(msg);
    
    if (1 == 2) {
        mandarRespuestaCliente(-2,msg.numero_serie);
    }

    if (strcmp(msg.operation, "set_value") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.numero_serie );}
    if (strcmp(msg.operation, "get_value") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.numero_serie );}
    if (strcmp(msg.operation, "modify_value") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.numero_serie );}
    if (strcmp(msg.operation, "delete_key") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.numero_serie );}
    if (strcmp(msg.operation, "exist") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.numero_serie );}
    if (strcmp(msg.operation, "destroy") == 0)
    { 
        destroy();
        printf("Value1 %s", msg.value1);
        mandarRespuestaCliente( 0, msg.numero_serie );}
}
