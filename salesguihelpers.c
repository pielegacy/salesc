#include<gtk/gtk.h>
#include "salesguihelpers.h"
#include "salesbase.h"
#include <stdlib.h>
// SearchSubmitPair *sale_io_pair(GObject *i, GObject *o){
//     SearchSubmitPair *temp = malloc(sizeof(SearchSubmitPair));
//     memcpy(temp->input, i, sizeof(i));
//     memcpy(temp->ouput, o, sizeof(o));
//     return temp;
// }
// FIX DIS
float sales_total(SearchSubmitPair *pair){
    int i;
    float total = 0;
    Product *temp = malloc(sizeof(Product) + 1);
    printf("The count is %d\n", pair->count);
    for (i = 0; i < pair->count-1; i++){
        if (pair->values[i] != 0){
            temp = search_product(pair->values[i]);
            printf("ID = %d, %0.2f\n",pair->values[i], temp->product_cost);
            total += temp->product_cost;
        }
    }
    return total;
}	