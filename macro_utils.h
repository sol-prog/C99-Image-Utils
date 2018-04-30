#pragma once

#include <stdio.h>
#include <stdlib.h>

# if !defined ON_ERROR_EXIT
    // Error utility macro
    #define ON_ERROR_EXIT(p, message) \
    do { \
        if((p)) { \
            printf("Error in function: \"%s\" at line: %d\n", __func__, __LINE__); \
            perror((message)); \
            exit(1); \
        } \
    } while(0)
#endif
