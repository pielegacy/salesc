#include<gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include "salesguihelpers.h"
#include <sqlite3.h>
#include <unistd.h>

// static void change_text(GtkWidget *widget, gpointer data){
//     const char* output = gtk_entry_get_text(GTK_ENTRY(widget));
//     printf("The text is : %s\n", output);
// }
static void add_sale_list(GtkWidget *widget, SearchSubmitPair *pair);
static void total_sale_list(GtkWidget *widget, SearchSubmitPair *pair);

int main(int argc, char *argv[]){
    GtkBuilder *builder;
    gtk_init(&argc, &argv);
    GObject *window;
    GObject *product_search;
    GObject *sale_list;
    GObject *submit_button;
    GObject *process_button;
    int val[100];
    // I <3 James Qu      
    builder = gtk_builder_new();
    db_create();
    gtk_builder_add_from_file(builder, "ui/main.ui", NULL);
    window = gtk_builder_get_object(builder, "mainwindow");
    g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    product_search = gtk_builder_get_object(builder, "product_entry");
    submit_button = gtk_builder_get_object(builder, "submit_button");
    process_button = gtk_builder_get_object(builder, "payment_process");
    
    sale_list = gtk_builder_get_object(builder, "sale_products");
    
    SearchSubmitPair *searchtolist = malloc(sizeof(SearchSubmitPair));
    searchtolist->input = product_search;
    searchtolist->output = sale_list;
    searchtolist->count = 0;
    memcpy(searchtolist->values, val, sizeof(val) + 1);
    
    g_signal_connect(submit_button, "clicked", G_CALLBACK(add_sale_list), searchtolist);
    g_signal_connect(product_search, "activate", G_CALLBACK(add_sale_list), searchtolist);
    g_signal_connect(process_button, "clicked", G_CALLBACK(total_sale_list), searchtolist);
    
    gtk_widget_show_all(GTK_WIDGET(sale_list));
    new_sale_group();
    gtk_main();

    return 0;
}

// Adds a product to the sale list
static void add_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    GtkWidget *label;
    sqlite3 *db;
    int rc;
    Product *searchproduct = search_product(atoi(gtk_entry_get_text(GTK_ENTRY(pair->input))));
    new_sale_group();
    
    if (strcmp(searchproduct->product_name, "DOESNT_EXIST") != 0){
        char *res = malloc(strlen(searchproduct->product_name)*2 + 1);
        sprintf(res, "$%0.2f : %s", searchproduct->product_cost, searchproduct->product_name);
        
        const char* label_text = res;
        label = gtk_label_new(label_text);
        printf("%s\n", label_text);
        pair->values[pair->count] = searchproduct->product_id;
        pair->count += 1;
        printf("%s added for $%0.2f, size of sale increased to : %d\n", searchproduct->product_name, searchproduct->product_cost, pair->count);
        gtk_list_box_insert(GTK_LIST_BOX(pair->output), label, 100);  
        gtk_widget_show_all(GTK_WIDGET(pair->output)); 
    }
    gtk_entry_set_text(GTK_ENTRY(pair->input), "");
}

static void total_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    int i;
    for (i = 0; i < pair->count; i++){
        printf("Value #%d is %d\n", i, pair->values[i]);
    }
}