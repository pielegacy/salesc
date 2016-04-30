#ifndef SALESGUI_H_
#define SALESGUI_H_

#include<gtk/gtk.h>
#include "salesbase.h"
typedef struct searchsubmit{
    GObject *input;
    GObject *output;
    GObject *window;
    GtkBuilder *builder;
    int count;
    int values[100];
} SearchSubmitPair;

typedef struct productSubmit{
    GObject *window;
    GtkBuilder *builder;
    GObject *product_id;
    GObject *product_name;
    GObject *product_cost;
    GObject *product_discount;
} ProductFieldSet;

// Used for passing multiple fields to a callback function in GTK
SearchSubmitPair *sale_io_pair(GObject *i, GObject *o);
// Used to return float of total values
float sales_total(SearchSubmitPair *pair);
// Used to convert a ProductFieldSet into a product in the DB
int process_product_fields(Product *product, int autoinc);
// Fill fields with existing product
void fill_product_fields(ProductFieldSet *fields);
// Empties fields after submit
void empty_fields(ProductFieldSet *fields);
#endif