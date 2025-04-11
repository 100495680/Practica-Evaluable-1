struct Coord {
            int x;
            int y;
        };

program CLAVES {
    version CLAVES_VER {

        

        int destroy(void) = 1;
        int set_value(int key, string value1<256>, int N_value2, double *V_value2, struct Coord value3) = 2;
        int get_value(int key, string value1<256>, int *N_value2, double *V_value2, struct Coord *value3) = 3;
        int modify_value(int key, string value1<256>, int N_value2, double *V_value2, struct Coord value3) = 4;
        int delete_key(int key) = 5;
        int exist(int key) = 6;

    } = 1
} = 100495680