#include <stdio.h>
#include <string.h>
#include "salesbase.h"
// #include <sqlite3.h>

// Main loop
int main(int argc, char *argv[]){
    Product sale_items[2];
    sale_items[0] = new_product(0, "yes", 2.0);
    printf("%f\n", sale_items[0].product_cost);
    // printf("The version of Sqlite3 that is being ran is version %s\n", sqlite3_libversion());
    getchar();
    return 0;
}