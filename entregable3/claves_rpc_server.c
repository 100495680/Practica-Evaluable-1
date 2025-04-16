#include "claves.h"
#include "claves_rpc.h"

bool_t remote_destroy_1_svc(void *arg, int *result, struct svc_req *rqstp) {
    *result = destroy();
    return TRUE;
}

bool_t remote_set_value_1_svc(arg_set_value *arg, int *result, struct svc_req *rqstp) {
    *result = set_value(arg->key, arg->value1, arg->N_value2, arg->V_value2.V_value2_val, arg->value3);
    return TRUE;
}

bool_t remote_get_value_1_svc(int *arg, ret_get_value *result, struct svc_req *rqstp) {
    // Usamos buffers estáticos en lugar de malloc para evitar conflictos de liberación automática
    static char buffer1[MAX_STRING];
    static double buffer2[MAX_VECTOR];

    result->value1 = buffer1;
    result->V_value2.V_value2_val = buffer2;

    result->status = get_value(*arg, result->value1, &result->N_value2, buffer2, &result->value3);
    result->V_value2.V_value2_len = result->N_value2;

    return TRUE;
}

bool_t remote_modify_value_1_svc(arg_set_value *arg, int *result, struct svc_req *rqstp) {
    *result = modify_value(arg->key, arg->value1, arg->N_value2, arg->V_value2.V_value2_val, arg->value3);
    return TRUE;
}

bool_t remote_delete_key_1_svc(int *arg, int *result, struct svc_req *rqstp) {
    *result = delete_key(*arg);
    return TRUE;
}

bool_t remote_exist_1_svc(int *arg, int *result, struct svc_req *rqstp) {
    *result = exist(*arg);
    return TRUE;
}

int claves_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result) {
    // No es necesario liberar nada manualmente, ya que usamos buffers estáticos
    return 1;
}
