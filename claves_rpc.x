struct Coord {
            int x;
            int y;
        };

struct ret_get_value{
    int status;
    string value1<256>;
    int N_value2; 
    double V_value2<32>; 
    struct Coord value3;
       } ;


struct arg_set_value{
    int key;
    string value1<256>; 
    int N_value2; 
    double V_value2<32>; 
    struct Coord value3;
       } ;

program CLAVES {
    version CLAVES_VER {
        int destroy(void) = 1;
        int set_value(struct arg_set_value a1) = 2;
        struct ret_get_value get_value(int key) = 3;
        int modify_value(struct arg_set_value a2) = 4;
        int delete_key(int key) = 5;
        int exist(int key) = 6;
    } = 1;
} = 100495680;