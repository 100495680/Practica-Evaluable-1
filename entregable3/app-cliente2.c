#include <stdio.h>
#include "claves.h"

int main() {


    int key = 10;
    char v1[MAX_STRING] = "Ejemplo";
    double v2[MAX_VECTOR] = {1.1, 2.2, 3.3};
    struct Coord v3 = {5, 6};
    int response;


    // Prueba de set value
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

    char retrieved_v1[MAX_STRING];
    int N_value2;
    double retrieved_v2[MAX_VECTOR];
    struct Coord retrieved_v3;


    // Prueba de get value
    printf("Obteniendo clave...\n");
    response = get_value(key, retrieved_v1, &N_value2, retrieved_v2, &retrieved_v3);
    if (response == 0) {
        printf("Clave obtenida correctamente: %s\n", retrieved_v1);
    } else {
        if (response == -1) {
            printf("Error al obtener clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }


    // Prueba de modify value
    printf("Modificando clave...\n");
    strncpy(v1, "Ejemplo modificado", MAX_STRING - 1);
    response = modify_value(key, v1, 3, v2, v3);
    if (response == 0) {
        printf("Clave modificada correctamente: %s\n", v1);
    } else {
        if (response == -1) {
            printf("Error al obtener clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }

    // Prueba de delete value
    printf("Borrando clave...\n");
    response = delete_key(key);
    if (response == 0) {
        printf("Clave borrada correctamente\n");
    } else {
        if (response == -1) {
            printf("Error al obtener clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }
    
    // Prueba de exist sin existir
    printf("¿Existe clave?...\n");
    response = exist(key);
    if (response == 0) {
        printf("No existe la clave\n");
    } else {
        if (response == 1) {
            printf("Existe la clave\n");
        }
        if (response == -1) {
            printf("Error al insertar la clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }

    // Prueba de exist existiendo
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
    printf("¿Existe clave?...\n");
    response = exist(key);
    if (response == 0) {
        printf("No existe la clave\n");
    } else {
        if (response == 1) {
            printf("Existe la clave\n");
        }
        if (response == -1) {
            printf("Error al insertar la clave\n");
        }
        if (response == -2) {
            printf("Error en el servidor\n");
        }
    }

    return 0;
}
