#include <stdio.h>
#include "claves.h"

int main() {
    int key1 = 1, key2 = 2;
    char value1_1[] = "Hola";
    char value1_2[] = "Mundo";

    double value2_1[] = {3.14, 2.71};
    double value2_2[] = {1.41, 1.73};

    struct Coord value3_1 = {7, 8};
    struct Coord value3_2 = {10, 20};

    printf("🔹 Insertando tupla 1...\n");
    int result1 = set_value(key1, value1_1, 2, value2_1, value3_1);
    if (result1 == 0)
        printf("✅ Tupla 1 insertada correctamente.\n");
    else
        printf("❌ Error al insertar Tupla 1. Código de error: %d\n", result1);

    printf("🔹 Insertando tupla 2...\n");
    int result2 = set_value(key2, value1_2, 2, value2_2, value3_2);
    if (result2 == 0)
        printf("✅ Tupla 2 insertada correctamente.\n");
    else
        printf("❌ Error al insertar Tupla 2. Código de error: %d\n", result2);

    return 0;
}
