/* Este es el código del cliente de este habrá multiples copias y interactuara como si fuese una API 
(conversiones en la lib dinamica) manda los mensajes al proxy y recibe las respuestas*/

#include "claves.h"

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include "proxy-mq.c"

// Como en una red de ordenadores (lo que tratamos de emular) al principio de la ejecución se manda un discover
// De forma que se conozca la ip de los nuevos elementos. Uso el nombre de Proxy por simplicidad aun que no es correcto
// en esta aproximación.




int main() {
    int key = 1;
    char value1[256] = "example_value";
    int N_value2 = 3;
    double V_value2[32] = {1.1, 2.2, 3.3};
    struct Coord value3 = {10, 20};

    int response = set_value(key, value1, N_value2, V_value2, value3);

    if (response == -1) {
        perror("Error en la creacion de la cola\n");
        exit(1);
    } else {if (response == -2) {
        perror("Error en el servidor\n");
        exit(1);
    } else {
        printf("Correcto envio del mensaje\n");
    }
    }
    return 0;
}