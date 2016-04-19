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

int main(int argc, char *argv[]){
    GtkBuilder *builder;
    gtk_init(&argc, &argv);
    GObject *window;
    GtkEntry *input;
        
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "ui/main.ui", NULL);
    
    window = gtk_builder_get_object(builder, "mainwindow");
    g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    input = GTK_ENTRY(gtk_builder_get_object(builder, "test_entry"));
    g_signal_connect(input, "changed", G_CALLBACK(change_text), NULL);
    
    
    
    gtk_main();
    
    return 0;
}