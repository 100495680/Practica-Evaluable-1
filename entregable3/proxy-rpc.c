#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves_rpc.h"
#include "claves.h"


CLIENT *clnt = NULL;

// Inicializa el cliente RPC usando IP_TUPLAS
int init_proxy() {
    char *ip = getenv("IP_TUPLAS");
    if (!ip) {
        fprintf(stderr, "Error: variable de entorno IP_TUPLAS no definida.\n");
        return -1;
    }

    // Crear cliente RPC con protocolo UDP
    clnt = clnt_create(ip, CLAVES_PROG, CLAVES_VERS, "udp");
    if (!clnt) {
        clnt_pcreateerror(ip);
        return -1;
    }

    return 0;
}

//Llama remotamente a destroy() para eliminar todas las tuplas en el servidor.
int destroy() {
    if (init_proxy() != 0) {
        printf("Error al inicializar el proxy RPC.\n");
        return 1;
    }
    int result;
    enum clnt_stat stat = remote_destroy_1(NULL, &result, clnt);
    return (stat == RPC_SUCCESS) ? result : -1;
}

//Inserta una nueva tupla con clave y valores en el servidor.
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (init_proxy() != 0) {
        printf("Error al inicializar el proxy RPC.\n");
        return 1;
    }
    arg_set_value args;
    args.key = key;
    args.value1 = value1;
    args.N_value2 = N_value2;
    args.V_value2.V_value2_len = N_value2;
    args.V_value2.V_value2_val = V_value2;
    args.value3 = value3;

    int result;
    enum clnt_stat stat = remote_set_value_1(&args, &result, clnt);
    return (stat == RPC_SUCCESS) ? result : -1;
}

//Recupera una tupla del servidor dado su key. Llena los buffers value1, N_value2, V_value2 y value3 con los datos
// recuperados.
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    if (init_proxy() != 0) {
        printf("Error al inicializar el proxy RPC.\n");
        return 1;
    }
    ret_get_value result;
    result.value1 = (char *)malloc(MAX_STRING);
    if (!result.value1) return -1;

    double buffer[MAX_VECTOR];
    result.V_value2.V_value2_val = buffer;

    // Llamada remota para obtener la tupla
    enum clnt_stat stat = remote_get_value_1(&key, &result, clnt);
    if (stat != RPC_SUCCESS || result.status != 0) {
        free(result.value1);
        return -1;
    }

    // Copiar valores devueltos del servidor al cliente
    strcpy(value1, result.value1);
    *N_value2 = result.N_value2;
    memcpy(V_value2, result.V_value2.V_value2_val, (*N_value2) * sizeof(double));
    *value3 = result.value3;

    free(result.value1);
    return 0;
}

// Modifica una tupla existente en el servidor
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (init_proxy() != 0) {
        printf("Error al inicializar el proxy RPC.\n");
        return 1;
    }
    arg_set_value args;
    args.key = key;
    args.value1 = value1;
    args.N_value2 = N_value2;
    args.V_value2.V_value2_len = N_value2;
    args.V_value2.V_value2_val = V_value2;
    args.value3 = value3;

    int result;
    enum clnt_stat stat = remote_modify_value_1(&args, &result, clnt);
    return (stat == RPC_SUCCESS) ? result : -1;
}

//Elimina una tupla del servidor dado su key.
int delete_key(int key) {
    if (init_proxy() != 0) {
        printf("Error al inicializar el proxy RPC.\n");
        return 1;
    }
    int result;
    enum clnt_stat stat = remote_delete_key_1(&key, &result, clnt);
    return (stat == RPC_SUCCESS) ? result : -1;
}

//Comprueba si existe una tupla con la clave dada.
int exist(int key) {
    if (init_proxy() != 0) {
        printf("Error al inicializar el proxy RPC.\n");
        return 1;
    }
    int result;
    enum clnt_stat stat = remote_exist_1(&key, &result, clnt);
    return (stat == RPC_SUCCESS) ? result : -1;
}
