#include <stdio.h>
#include <string.h>
#include "claves.h"

int main() {


    int key = 42;
    char v1[MAX_STRING] = "valor original";
    double v2[3] = {1.1, 2.2, 3.3};
    struct Coord v3 = {7, 8};
    int result;

    printf("🧹 Destruyendo lista previa...\n");
    if (destroy() == 0)
        printf("✅ Lista destruida\n");
    else
        printf("❌ Error al destruir lista\n");

    printf("\n📥 Insertando tupla...\n");
    result = set_value(key, v1, 3, v2, v3);
    if (result == 0)
        printf("✅ set_value OK\n");
    else
        printf("❌ set_value ERROR (%d)\n", result);

    printf("\n🔍 Recuperando tupla...\n");
    char buffer[MAX_STRING];
    int N;
    double v2_out[MAX_VECTOR];
    struct Coord v3_out;

    result = get_value(key, buffer, &N, v2_out, &v3_out);
    if (result == 0) {
        printf("✅ get_value OK: %s | N=%d | Coord=(%d,%d)\n", buffer, N, v3_out.x, v3_out.y);
        for (int i = 0; i < N; i++)
            printf("  v2[%d] = %.2f\n", i, v2_out[i]);
    } else {
        printf("❌ get_value ERROR (%d)\n", result);
    }

    printf("\n✏️ Modificando tupla...\n");
    strcpy(v1, "valor modificado");
    result = modify_value(key, v1, 3, v2, v3);
    if (result == 0)
        printf("✅ modify_value OK\n");
    else
        printf("❌ modify_value ERROR (%d)\n", result);

    printf("\n❓ Verificando existencia...\n");
    result = exist(key);
    if (result == 1)
        printf("✅ La clave existe\n");
    else if (result == 0)
        printf("❌ La clave NO existe (error)\n");
    else
        printf("❌ exist ERROR (%d)\n", result);

    printf("\n🗑️ Borrando clave...\n");
    result = delete_key(key);
    if (result == 0)
        printf("✅ delete_key OK\n");
    else
        printf("❌ delete_key ERROR (%d)\n", result);

    printf("\n🔁 Verificando que ya no exista...\n");
    result = exist(key);
    if (result == 0)
        printf("✅ Clave ya no existe (correcto)\n");
    else if (result == 1)
        printf("❌ Clave todavía existe (error)\n");
    else
        printf("❌ exist ERROR (%d)\n", result);

    printf("\n🏁 Test finalizado.\n");
    return 0;
}
