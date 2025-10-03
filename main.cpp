#include <stdio.h>
#include "skip.h"
#include <iostream>

int main() {
    void* test = skip_create_base_config();
    

    skip_push_type_to_config(test , skip_char , 12);

    char* buffer = new char[skip_get_cfg_size(test)];

    skip_write_index_to_buffer(test , (void*)buffer , (void*)"Hello World\0" , 0);

    std::cout << skip_get_cfg_size(test) << "\n";

    char* value = new char[12];

    skip_read_index_from_buffer(test , (void*)buffer , (void*)value , 0);

    std::cout << value;

    skip_free_cfg(test);
    delete[] buffer;
    return 0;
}
