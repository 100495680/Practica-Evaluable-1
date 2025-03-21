#include <stdio.h>
#include "claves.h"

int main() {
    int key = 100;
    char *v1 = "test_value1";
    double v2[] = {1.1, 2.2, 3.3};
    struct Coord v3 = {10, 20};

    printf("Probando set_value...\n");
    if (set_value(key, v1, 3, v2, v3) == 0)
        printf("Tupla insertada correctamente.\n");
    else
        printf("Error al insertar tupla.\n");

    char buffer[256];
    double v2_out[32];
    int N;
    struct Coord v3_out;

    printf("Probando get_value...\n");
    if (get_value(key, buffer, &N, v2_out, &v3_out) == 0) {
        printf("Tupla recuperada: %s | N=%d | Coord=(%d,%d)\n", buffer, N, v3_out.x, v3_out.y);
        for (int i = 0; i < N; i++)
            printf("  v2[%d] = %lf\n", i, v2_out[i]);
    } else {
        printf("Error al recuperar tupla.\n");
    }

    printf("Probando destroy...\n");
    if (destroy() == 0)
        printf("Tuplas eliminadas.\n");
    else
        printf("Error al destruir.\n");

    return 0;
}