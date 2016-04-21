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

static void add_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    GtkWidget *label;
    sqlite3 *db;
    int rc;
    Product *searchproduct = search_product(atoi(gtk_entry_get_text(GTK_ENTRY(pair->input))));
    // TODO: ADD PRICES
    const char* label_text = strdup(searchproduct->product_name);
    label = gtk_label_new(label_text);
    gtk_list_box_insert(GTK_LIST_BOX(pair->output), label, 100);  
    gtk_widget_show_all(GTK_WIDGET(pair->output)); 
    gtk_entry_set_text(GTK_ENTRY(pair->input), "");
}

int main(int argc, char *argv[]){
    GtkBuilder *builder;
    gtk_init(&argc, &argv);
    GObject *window;
    GObject *product_search;
    GObject *sale_list;
    GObject *submit_button;
    // I <3 James Qu      
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "ui/main.ui", NULL);
    window = gtk_builder_get_object(builder, "mainwindow");
    g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    product_search = gtk_builder_get_object(builder, "product_entry");
    submit_button = gtk_builder_get_object(builder, "submit_button");
    
    sale_list = gtk_builder_get_object(builder, "sale_products");
    
    SearchSubmitPair *searchtolist = malloc(sizeof(SearchSubmitPair));
    searchtolist->input = product_search;
    searchtolist->output = sale_list;
    g_signal_connect(submit_button, "clicked", G_CALLBACK(add_sale_list), searchtolist);
    g_signal_connect(product_search, "activate", G_CALLBACK(add_sale_list), searchtolist);
    gtk_widget_show_all(GTK_WIDGET(sale_list));
    gtk_main();
    
    return 0;
}