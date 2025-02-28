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


struct Peticion leerMensajesCliente(mqd_t mq)  {
    
    struct Peticion msg;
    mq_receive(mq, (char *)&msg, sizeof(msg), 0);
    
    printf("Mensaje en Proxy recibido: %s\n", msg.value1);


    return msg;
}

int mandarMensajesServidor(struct Peticion msg, mqd_t mq)    {
    
    if (mq_send(mq, (char *)&msg, sizeof(msg),0) == -1)
    { perror("Client MQ send error"); exit(1);}
    printf("Mensaje en Proxy enviado: %s\n", msg.value1);

    return 0;
}

struct Respuesta leerRespuestaServidor(mqd_t mq)  {
    
    struct Respuesta msg;
    mq_receive(mq, (char *)&msg, sizeof(msg), 0);
    
    printf("Respuesta en Proxy recibido\n");

    return msg;
}

int mandarRespuestaCliente(struct Respuesta msg, mqd_t mq)    {

    if (mq_send(mq, (char *)&msg, sizeof(msg),0) == -1)
    { perror("Client MQ send error"); exit(1);}

    printf("Respuesta en Proxy enviado \n");

    return 0;
}



int main() {

    mqd_t mq_cliente = mq_open("/colaCliente", O_CREAT | O_RDWR, 0666, NULL);
    mqd_t mq_servidor = mq_open("/colaServidor", O_CREAT | O_RDWR, 0666, NULL);

    // La lógica del servidor estará en un loop infinito
    //while (true)
    //{
        //if (condicion activada cuando un nuevo proceso se conecte)
        /*
        lista[0] = client_ip;
        int size ++;

        lista = (int*)realloc(lista, size * sizeof(int));
        */


        struct Peticion msg;
        msg = leerMensajesCliente(mq_cliente);
        mandarMensajesServidor(msg, mq_servidor);
        
        struct Respuesta rp;
        rp = leerRespuestaServidor(mq_servidor);
        mandarRespuestaCliente(rp, mq_cliente);
        
    //}
}
