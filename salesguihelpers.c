#include<gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include "salesguihelpers.h"
#include <sqlite3.h>
#include <unistd.h>
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
int process_product_fields(ProductFieldSet *fields){
    Product *temp = malloc(sizeof(Product) + 1);
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(fields->product_id)));
    const char *name_temp = gtk_entry_get_text(GTK_ENTRY(fields->product_name));
    char *name_final;
    strcpy(name_final, name_temp);
    float cost = atof(gtk_entry_get_text(GTK_ENTRY(fields->product_cost)));
    float discount = atof(gtk_entry_get_text(GTK_ENTRY(fields->product_discount)));
    temp = search_product(id);
    if (strcmp(temp->product_name, "DOESNT_EXIST") != 0){ // Exists
        temp = new_product_v2(id, name_final, cost);
        temp->product_discount = discount;
        update_product(temp);
    }
    else {
        temp = new_product_v2(id, name_final, cost);
        add_product(temp);
    }
    return 0;
}
void fill_product_fields(ProductFieldSet *fields){
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(fields->product_id)));
    Product *temp = malloc(sizeof(Product) + 1);
    temp = search_product(id);
    if (strcmp(temp->product_name, "DOESNT_EXIST") != 0){ // Exists
        char *placeholder = malloc(sizeof("0000000000.00") + 1);
        sprintf(placeholder, "%0.2f", temp->product_cost);
        gtk_entry_set_text(GTK_ENTRY(fields->product_cost), placeholder);
    }
}