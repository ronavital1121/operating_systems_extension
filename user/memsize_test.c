#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int initial_size = memsize();
    printf("Initial memory size: %d bytes\n", initial_size);

    void *ptr = malloc(20 * 1024);  
    if (!ptr) {  
        printf("ERROR: malloc failed!\n");
        exit(1, "");  
    }
    int after_alloc_size = memsize();
    printf("Memory size after allocation: %d bytes\n", after_alloc_size);

    free(ptr);
    int after_free_size = memsize();
    printf("Memory size after freeing: %d bytes\n", after_free_size);

    exit(0, "");
}
