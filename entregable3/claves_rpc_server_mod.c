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
    char *value1 = malloc(MAX_STRING * sizeof(char));
    double *value2 = malloc(MAX_VECTOR * sizeof(double));

    if (!value1 || !value2) {
        if (value1) free(value1);
        if (value2) free(value2);
        result->status = -1;  // Código de error por falta de memoria
        return TRUE;
    }

    result->value1 = value1;
    result->V_value2.V_value2_val = value2;

    result->status = get_value(*arg, value1, &result->N_value2, value2, &result->value3);
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

/*int claves_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result) {
    // No es necesario liberar nada manualmente, ya que usamos buffers estáticos
    return 1;
}*/
int claves_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result) {
    // Liberamos memoria si el resultado es del tipo get_value
    if (xdr_result == (xdrproc_t)xdr_ret_get_value) {
        ret_get_value *res = (ret_get_value *) result;
        if (res->value1) free(res->value1);
        if (res->V_value2.V_value2_val) free(res->V_value2.V_value2_val);
    }

    return 1;
}

