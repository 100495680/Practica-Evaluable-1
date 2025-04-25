#include <stdio.h>
#include "claves.h"

int main() {
    int key = 10;
    char v1[MAX_STRING] = "Ejemplo";
    double v2[MAX_VECTOR] = {1.1, 2.2, 3.3};
    struct Coord v3 = {5, 6};
    int response;


    for (int i=0; i < 10000; i++) {
    // Prueba de set value con 30 mil claves, se puede probar la concurrencia corriendo otros procesos mientras este está en ejecución.
    printf("Insertando clave...\n");
    response = set_value(key, v1, 3, v2, v3);
    if (response == 0) {
        printf("Clave insertada correctamente\n");
    } else {
        if (response == -1) {
            printf("Error al insertar la clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }
    key++;
    }

    return 0;
}
