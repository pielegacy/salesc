#ifndef SALESGUI_H_
#define SALESGUI_H_

#include<gtk/gtk.h>

typedef struct searchsubmit{
    GObject *input;
    GObject *output;
    int count;
    int values[100];
} SearchSubmitPair;

// Used for passing multiple fields to a callback function in GTK
SearchSubmitPair *sale_io_pair(GObject *i, GObject *o);

#endif