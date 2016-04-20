#include<gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include <sqlite3.h>
#include <unistd.h>

static void change_text(GtkWidget *widget, gpointer data){
    const char* output = gtk_entry_get_text(GTK_ENTRY(widget));
    printf("The text is : %s\n", output);
}
static void add_sale_list(GtkWidget *widget, GObject *list){
    GtkWidget *label;
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    sqlite3_stmt *result;
    sqlite3_prepare_v2(db, "SELECT * FROM PRODUCTS", 128, &result, NULL); 
    while ((rc = sqlite3_step(result)) == SQLITE_ROW){
        const char* label_text = sqlite3_column_text(result, 1);
        label = gtk_label_new(label_text);
    }
    sqlite3_close(db);
    gtk_list_box_insert(GTK_LIST_BOX(list), label, 100);  
    gtk_widget_show_all(GTK_WIDGET(list)); 
}
int main(int argc, char *argv[]){
    GtkBuilder *builder;
    gtk_init(&argc, &argv);
    GObject *window;
    GtkEntry *input;
    GObject *sale_list;
    GObject *submit_button;
    // I <3 James Qu      
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "ui/main.ui", NULL);
    window = gtk_builder_get_object(builder, "mainwindow");
    g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    input = GTK_ENTRY(gtk_builder_get_object(builder, "test_entry"));
    submit_button = gtk_builder_get_object(builder, "submit_button");
    //g_signal_connect(input, "changed", G_CALLBACK(change_text), NULL);
    
    
    sale_list = gtk_builder_get_object(builder, "sale_products");
    g_signal_connect(submit_button, "clicked", G_CALLBACK(add_sale_list), sale_list);
    
    // int i = 0;
    // for (i = 0; i < 40; i++){
    //     label = gtk_label_new("Cock");
    //     gtk_list_box_insert(GTK_LIST_BOX(sale_list), label, i);     
    // }
    gtk_widget_show_all(GTK_WIDGET(sale_list));
    // ADD db_create()
    gtk_main();
    
    return 0;
}