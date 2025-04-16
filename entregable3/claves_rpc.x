struct Coord {
    int x;
    int y;
};

struct ret_get_value {
    int status;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord value3;
};

struct arg_set_value {
    int key;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord value3;
};

program CLAVES_PROG {
    version CLAVES_VERS {
        int remote_destroy(void) = 1;
        int remote_set_value(arg_set_value) = 2;
        ret_get_value remote_get_value(int) = 3;
        int remote_modify_value(arg_set_value) = 4;
        int remote_delete_key(int) = 5;
        int remote_exist(int) = 6;
    } = 1;
} = 0x31234567;
