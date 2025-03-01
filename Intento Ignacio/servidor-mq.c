/* Este archivo es como el servidor que recibe las peticiones y le pide a claves.c que ejecute los cambios
los comandos POSIX (set_value, get_value, modify_value, delete_key, exist, destroy) */

#include "claves.h"
#include "claves.c"

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>

// Como en una red de ordenadores (lo que tratamos de emular) al principio de la ejecución se manda un discover
// De forma que se conozca la ip de los nuevos elementos. Uso el nombre de Proxy por simplicidad aun que no es correcto
// en esta aproximación.




struct paquete
{
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
};

struct paquete sacarPaquete(struct Peticion msg) {
    struct paquete pkt;
    strcpy(pkt.value1, msg.value1);
    pkt.N_value2 = msg.N_value2;
    memcpy(pkt.V_value2, msg.V_value2, sizeof(msg.V_value2)); 
    pkt.value3 = msg.value3;
    return pkt;
};

struct Peticion leerMensajesCliente(mqd_t mq) {
    
    struct Peticion msg;
    mq_receive(mq, (char *)&msg, sizeof(msg), 0);
    
    printf("Mensaje recibido: %s\n", msg.value1);

    return msg;
}

int mandarRespuestaCliente(int status, long ip_add, long proxy_add, mqd_t mq) {

    struct Respuesta msg;
    msg.ip_add = ip_add;
    msg.proxy_add = proxy_add;
    msg.status = status;

    if (mq_send(mq, (char *)&msg, sizeof(msg),0) == -1)
    { perror("Client MQ send error"); exit(1);}

    printf("Respuesta de Servidor enviado\n");

    return 0;
}



int main() {

    mqd_t mq = mq_open("/colaEnvios",  O_RDWR, 0666, NULL);

    struct Peticion msg;
    struct paquete elemento;

    msg = leerMensajesCliente(mq);
    elemento = sacarPaquete(msg);
    
    if (1 == 2) {
        mandarRespuestaCliente(-2, msg.ip_add, msg.proxy_add, mq);
    }

    if (strcmp(msg.operation, "set_value") == 0)
    { 
        mandarRespuestaCliente( 0, msg.ip_add, msg.proxy_add, mq );}
    if (strcmp(msg.operation, "get_value") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.ip_add, msg.proxy_add, mq );}
    if (strcmp(msg.operation, "modify_value") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.ip_add, msg.proxy_add, mq );}
    if (strcmp(msg.operation, "delete_key") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.ip_add, msg.proxy_add, mq );}
    if (strcmp(msg.operation, "exist") == 0)
    { 
        
        mandarRespuestaCliente( 0, msg.ip_add, msg.proxy_add, mq );}
    if (strcmp(msg.operation, "destroy") == 0)
    { 
        printf("Value1 %s", msg.value1);
        mandarRespuestaCliente( 0, msg.ip_add, msg.proxy_add, mq);}

    mq_unlink("/colaEnvios");
}
