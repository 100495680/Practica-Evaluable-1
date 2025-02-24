/* Este archivo es como el servidor gateway que recibe las peticiones del cliente o clientes y crea la cola de mensajes
que manda al servidor para que ejecute los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// Como en una red de ordenadores (lo que tratamos de emular) al principio de la ejecución se manda un discover
// De forma que se conozca la ip de los nuevos elementos. Uso el nombre de Proxy por simplicidad aun que no es correcto
// en esta aproximación.
long mandarDiscover()    {
    key_t clave = ftok("cola.msg", 22);  // Generar clave única para la cola de mensajes el número 22 es arbitrario
    int msgid = msgget(clave, 0666 | IPC_CREAT);  // Crear o conectar cola
    long msg = getpid();
    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar mensaje
    printf("Proxy discover enviado: ");

    return msg;
}

long recibirDiscover() {
    key_t clave = ftok("cola.msg", 22);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    long proxy_add;
    msgrcv(msgid, &proxy_add, sizeof(proxy_add) - sizeof(long), 1, 0);  // Recibir mensaje tipo 1

    printf("proxy Add del Cliente Recibida: %d\n", proxy_add);
    return proxy_add;
}

struct Peticion {
    long ip_add; // Vamos a imitar la comunicacion de un sevidor real
    long proxy_add;
    char* operation;  // Código de operación POSIX
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

struct Peticion leerMensajesCliente()  {
    key_t clave = ftok("cola.msg", 22);
    int msgid = msgget(clave, 0666);  // Obtener ID de la cola

    struct Peticion msg;
    msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), , 0);  // Recibir mensaje tipo 1

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

    // Identificación al servidor y guardar mi ip
    long mi_ip = mandarDiscover();

    // Crear el directorio de ips conectadas
    int *lista;
    int size = 1;

    lista = (int*)malloc(size * sizeof(int));

    // La lógica del servidor estará en un loop infinito
    //while (true)
    //{
        // Actualizacion del directorio de ips lista dinamica
        long client_ip = recibirDiscover();
        //if (condicion activada cuando un nuevo proceso se conecte)
        /*
        lista[0] = client_ip;
        int size ++;

        lista = (int*)realloc(lista, size * sizeof(int));
        */


        struct Peticion msg;
        msg = leerMensajesCliente();
        mandarMensajesServidor(msg);
        
        struct Respuesta rp;
        rp = leerRespuestaServidor();
        mandarRespuestaCliente(rp);
        
    //}
}
