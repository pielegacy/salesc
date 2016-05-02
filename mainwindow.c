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
const int auto_increment = 0;
static void add_sale_list(GtkWidget *widget, SearchSubmitPair *pair);
static void total_sale_list(GtkWidget *widget, SearchSubmitPair *pair);
static void show_price(GtkWidget *widget, SearchSubmitPair *pair);
static void clear_payment_field(GtkWidget *widget, SearchSubmitPair *pair);
static void clear_window(GtkWidget *widget, GtkWidget *window);
static void sale_success(GtkWidget *paywidget, SearchSubmitPair *pair);
static void add_from_list(GtkWidget *widget, SearchSubmitPair *fields);
// Window methods
static void new_sale_window(GtkWidget *widget, GtkBuilder *builder);
static void new_product_window(GtkWidget *widget, GtkBuilder *builder);
static void view_sale_window(GtkWidget *widget, gpointer sale_pointer);
// Fill list of products
void fill_product_list(GObject *list, SearchSubmitPair *fields);
void fill_sales(GObject *sale_list);

int salecount = 0;
int main(int argc, char *argv[]){
    GtkBuilder *builder;
    gtk_init(&argc, &argv);
    GObject *menu_window;
    GObject *new_sale;
    GObject *new_product;
    GObject *sale_list;
    db_create(auto_increment);
    // I <3 James Qu      
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "ui/menu.ui", NULL);
    menu_window = gtk_builder_get_object(builder, "mainwindow");    
    new_sale = gtk_builder_get_object(builder, "new_sale");
    new_product = gtk_builder_get_object(builder, "new_product");
    sale_list = gtk_builder_get_object(builder, "sales_list");
    fill_sales(sale_list);
    g_signal_connect(new_sale, "clicked", G_CALLBACK(new_sale_window), builder);
    g_signal_connect(new_product, "clicked", G_CALLBACK(new_product_window), builder);
    //gtk_window_fullscreen(GTK_WINDOW(menu_window));
    new_sale_group();
    gtk_main();

    return 0;
}
void fill_sales(GObject *sale_list){
    sqlite3 *db;
    int rc;
    int currentsale = 1;
    int currentsalecount = 0;
    char *currentitemlist = malloc(sizeof("") + 1);
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    sqlite3_stmt *result;
    sqlite3_prepare_v2(db, "SELECT * FROM SALES;", 128, &result, NULL); 
    while ((rc = sqlite3_step(result)) == SQLITE_ROW){
        if (currentsale != sqlite3_column_int(result, 1)){
            GtkWidget *sale_option;
            float payment = payment_amount_retrieve(sqlite3_column_int(result,3));
            char *label_final = malloc(strlen(sqlite3_column_text(result,1)) + sizeof(currentsalecount) + sizeof(payment) + (20 * sizeof(char)));
            sprintf(label_final, "%d : %d item/s for $%0.2f",currentsale, currentsalecount, payment);
            //const 
            GtkWidget *product_option = gtk_button_new_with_label(label_final);
            g_signal_connect(product_option, "clicked", G_CALLBACK(view_sale_window), NULL);
            gtk_list_box_insert(GTK_LIST_BOX(sale_list), product_option, 100);
            currentsale = sqlite3_column_int(result, 1);
            currentsalecount = 1;
        }
        else {
            currentsalecount++;
        }
        
    }
    gtk_widget_show_all(GTK_WIDGET(sale_list));
    sqlite3_close(db);
}
static void clear_window(GtkWidget *widget, GtkWidget *window){
    gtk_window_close(GTK_WINDOW(window));
}
static void view_sale_window(GtkWidget *widget, gpointer sale_pointer){
    GtkBuilder *builder;
    GObject *menu_window;
    GObject *sale_product_list;
    
    builder = gtk_builder_new();
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    sqlite3_stmt *result;
    const char *text = gtk_button_get_label(GTK_BUTTON(widget));
    char *text_normal = malloc(strlen(text) + 1);
    strcpy(text_normal, text);
    int sale_group = atoi(strtok(text_normal, ":"));
    // printf("%s\n", output);
    gtk_builder_add_from_file(builder, "ui/sale_details.ui", NULL);
    menu_window = gtk_builder_get_object(builder, "mainwindow");
    sale_product_list = gtk_builder_get_object(builder, "sale_receipt");
    char *sql_statement = sqlite3_mprintf("SELECT * FROM SALES WHERE SALE_GROUP = %d;", sale_group);
    sqlite3_prepare_v2(db, sql_statement, 128, &result, NULL); 
    while ((rc = sqlite3_step(result)) == SQLITE_ROW){
        printf("%d\n", sqlite3_column_int(result, 2));
        
        gtk_label_set_text(GTK_LABEL(sale_product_list),);
        //g_signal_connect(product_option, "clicked", G_CALLBACK(add_from_list), fields);
    }
    
    sqlite3_close(db);
    
}
static void new_sale_window(GtkWidget *widget, GtkBuilder *oldbuilder){
    GtkBuilder *builder;
    GObject *sale_window;
    GObject *product_search;
    GObject *sale_list;
    GObject *product_list;
    //GObject *submit_button;
    GObject *process_button;
    GObject *new_sale;
    // Price inputs
    GObject *payment_cash;
    GObject *payment_debit;
    GObject *payment_credit;
    GObject *payment_cheque;
    
    //ListSubmitSet *listsubmit = malloc(sizeof(ListSubmitSet) + 1);
    
    int val[100];
    // I <3 James Qu      
    builder = gtk_builder_new();
    db_create(1);
    gtk_builder_add_from_file(builder, "ui/main.ui", NULL);
    sale_window = gtk_builder_get_object(builder, "mainwindow");
    //g_signal_connect (sale_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    product_search = gtk_builder_get_object(builder, "product_entry");
    //submit_button = gtk_builder_get_object(builder, "submit_button");
    process_button = gtk_builder_get_object(builder, "payment_process");
    sale_list = gtk_builder_get_object(builder, "sale_products");
    payment_cash = gtk_builder_get_object(builder, "payment_cash");
    payment_debit = gtk_builder_get_object(builder, "payment_debit");
    payment_credit = gtk_builder_get_object(builder, "payment_credit");
    payment_cheque = gtk_builder_get_object(builder, "payment_cheque");
    
    new_sale = gtk_builder_get_object(builder, "new_sale");
    
    SearchSubmitPair *searchtolist = malloc(sizeof(SearchSubmitPair) + 1);
    searchtolist->input = product_search;
    searchtolist->output = sale_list;
    searchtolist->builder = builder;
    searchtolist->count = 0;
    searchtolist->window = sale_window;
    memcpy(searchtolist->values, val, sizeof(val) + 1);
    
    product_list = gtk_builder_get_object(builder, "product_list");
    fill_product_list(product_list, searchtolist);
    //_signal_connect(submit_button, "clicked", G_CALLBACK(add_sale_list), searchtolist);
    g_signal_connect(product_search, "activate", G_CALLBACK(add_sale_list), searchtolist);
    g_signal_connect(process_button, "clicked", G_CALLBACK(clear_window), sale_window);
    g_signal_connect(payment_cash, "activate", G_CALLBACK(total_sale_list), searchtolist);
    g_signal_connect(payment_cash, "focus-out-event", G_CALLBACK(clear_payment_field), searchtolist);
    g_signal_connect(payment_debit, "activate", G_CALLBACK(total_sale_list), searchtolist);
    g_signal_connect(payment_debit, "focus-out-event", G_CALLBACK(clear_payment_field), searchtolist);
    g_signal_connect(payment_credit, "activate", G_CALLBACK(total_sale_list), searchtolist);
    g_signal_connect(payment_credit, "focus-out-event", G_CALLBACK(clear_payment_field), searchtolist);
    g_signal_connect(payment_cheque, "activate", G_CALLBACK(total_sale_list), searchtolist);
    g_signal_connect(payment_cheque, "focus-out-event", G_CALLBACK(clear_payment_field), searchtolist);
    
    //gtk_window_fullscreen(GTK_WINDOW(sale_window));
    gtk_widget_show_all(GTK_WIDGET(sale_list));
    gtk_widget_grab_focus(GTK_WIDGET(product_search));
}
// Adds a product from the product list in the bottom
static void add_from_list(GtkWidget *widget, SearchSubmitPair *fields){
    gtk_widget_grab_focus(GTK_WIDGET(fields->input));
    const char *text = gtk_button_get_label(GTK_BUTTON(widget));
    char *text_normal = malloc(strlen(text) + 1);
    strcpy(text_normal, text);
    char *output = strtok(text_normal, ":");
    gtk_entry_set_text(GTK_ENTRY(fields->input), output);
}
void fill_product_list(GObject *list, SearchSubmitPair *fields){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    sqlite3_stmt *result;
    sqlite3_prepare_v2(db, "SELECT * FROM PRODUCTS", 128, &result, NULL); 
    while ((rc = sqlite3_step(result)) == SQLITE_ROW){
        GtkWidget *product_option;
        char *label = malloc(strlen(sqlite3_column_text(result, 0)) + strlen(sqlite3_column_text(result, 1)) + strlen(sqlite3_column_text(result, 2)) + 6);
        int labelid = sqlite3_column_int(result, 0);
        fields->id = labelid;
        sprintf(label, "%s: %s $%0.2f", sqlite3_column_text(result, 0), sqlite3_column_text(result, 1), sqlite3_column_double(result, 2));
        const char *label_final = label;
        product_option = gtk_button_new_with_label(label_final);
        //printf("%d : %s for $%0.2f\n",sqlite3_column_int(result, 0), sqlite3_column_text(result, 1), sqlite3_column_double(result, 2));
        g_signal_connect(product_option, "clicked", G_CALLBACK(add_from_list), fields);
        gtk_list_box_insert(GTK_LIST_BOX(list), product_option, 100);
    }
    gtk_widget_show_all(GTK_WIDGET(list));
    sqlite3_close(db);
}
// Adds a product to the sale list
static void add_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    GtkWidget *label;
    sqlite3 *db;
    int rc;
    SearchSubmitPair *temp = malloc(sizeof(pair + 1));
    memcpy(temp, pair, sizeof(pair + 1));
    Product *searchproduct = search_product(atoi(gtk_entry_get_text(GTK_ENTRY(pair->input))));
    new_sale_group();
    
    if (strcmp(searchproduct->product_name, "DOESNT_EXIST") != 0){
        char *res = malloc(strlen(searchproduct->product_name)*2 + 1); // I know this is dodgy, will fix late
        sprintf(res, "$%0.2f : %s", searchproduct->product_cost, searchproduct->product_name);
        const char* label_text = res;
        label = gtk_label_new(label_text);
        printf("%s\n", label_text);
        pair->values[pair->count] = searchproduct->product_id;
        pair->count += 1;
        printf("%s added for $%0.2f, size of sale increased to : %d\n", searchproduct->product_name, searchproduct->product_cost, pair->count);
        gtk_list_box_insert(GTK_LIST_BOX(pair->output), label, 100);
        //g_signal_connect(GTK_WIDGET(label), "clicked", G_CALLBACK(gtk_main_quit), NULL);  
        gtk_widget_show_all(GTK_WIDGET(pair->output));
    }
    gtk_entry_set_text(GTK_ENTRY(pair->input), "");
}


static void total_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    int i;
    float cost = 0.00;
    Product *temp = malloc(sizeof(temp) + 1);
    char* placeholder = malloc(sizeof("0000000000.00") + 1);
    for (i = 0; i < pair->count; i++){
        if (pair->values[i] != 0){
            temp = search_product(pair->values[i]);
            printf("Value %s is %0.2f\n", temp->product_name, temp->product_cost);
            cost += temp->product_cost;
        }
        else {
            break;
        }
    }
    
    if (atof(gtk_entry_get_text(GTK_ENTRY(widget))) >= cost && cost != 0 && atof(gtk_entry_get_text(GTK_ENTRY(widget))) != 0){
        sale_success(widget, pair);
    }
    else {
        sprintf(placeholder, "%0.2f", cost);
        gtk_entry_set_text(GTK_ENTRY(widget), placeholder);
    }
    printf("THE TOTAL COST IS $%0.2f\n", cost);
}

static void show_price(GtkWidget *widget, SearchSubmitPair *pair){
    int i = 0;
    printf("The count is %d\n", pair->count);
}

static void clear_payment_field(GtkWidget *widget, SearchSubmitPair *pair){
    gtk_entry_set_text(GTK_ENTRY(widget), "");
}

static void sale_success(GtkWidget *paywidget, SearchSubmitPair *pair){
    const gchar *widgettype = gtk_widget_get_name(paywidget);
    //printf("TYPE IS %s", widgettype);
    printf("Processing payment...\n");
    Payment *temp = malloc(sizeof(Payment) + 1);
    int pay_type;
    float paid = atof(gtk_entry_get_text(GTK_ENTRY(paywidget)));
    if (strcmp("payment_cash", widgettype) == 0){
        pay_type = 0;
    }
    else if (strcmp("payment_debit", widgettype) == 0)
    {
        pay_type = 1;
    }
    else if (strcmp("payment_credit", widgettype) == 0)
    {
        pay_type = CREDIT;
    }
    temp = new_payment(0,CASH,paid);
    add_payment(temp);
    int pay_id = recent_payment_id();
    int sale_group = new_sale_group();
    int i;
    for (i = 0; i < pair->count; i++){
        if (pair->values[i] != 0){
            add_sell_from_id(new_sell_from_id(sale_group, pair->values[i], pay_id));
        }
        else {
            break;
        }
    }
    gtk_window_close(GTK_WINDOW(pair->window));
}

static void update_products(GtkWidget *widget, ProductFieldSet *fields);
static void pull_product(GtkWidget *widget, ProductFieldSet *fields);
static void updatefields(GtkWidget *widget, ProductFieldSet *fields);

static void new_product_window(GtkWidget *widget, GtkBuilder *oldbuilder){
    GtkBuilder *builder;
    GObject *product_window;
    GObject *product_process;
    GObject *product_search;
    
    ProductFieldSet *fields = malloc(sizeof(ProductFieldSet) + 1);
    GObject *product_id;
    GObject *product_name;
    GObject *product_cost;
    GObject *product_discount;
    
    builder = gtk_builder_new();
    db_create(0);
    gtk_builder_add_from_file(builder, "ui/products.ui", NULL);
    product_window = gtk_builder_get_object(builder, "mainwindow");
    product_process = gtk_builder_get_object(builder, "product_process");
    product_search = gtk_builder_get_object(builder, "product_search");
    
    product_id = gtk_builder_get_object(builder, "product_id");
    product_name = gtk_builder_get_object(builder, "product_name");
    product_cost = gtk_builder_get_object(builder, "product_cost");
    product_discount = gtk_builder_get_object(builder, "product_discount");
    fields->builder = builder;
    fields->product_id = product_id;
    fields->product_name = product_name;
    fields->product_cost = product_cost;
    fields->product_discount = product_discount;
    
    g_signal_connect(product_process, "clicked", G_CALLBACK(update_products), fields);    
    g_signal_connect(product_search, "clicked", G_CALLBACK(pull_product), fields);
    g_signal_connect(product_id, "activate", G_CALLBACK(pull_product), fields);
    // g_signal_connect(product_id, "changed", G_CALLBACK(updatefields), fields);
}
static void update_products(GtkWidget *widget, ProductFieldSet *fields){
    Product *passthrough = malloc(sizeof(Product) + 1);
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(fields->product_id)));
    char *name_final = malloc(sizeof(fields->product_name));
    strcpy(name_final, gtk_entry_get_text(GTK_ENTRY(fields->product_name)));
    float cost = atof(gtk_entry_get_text(GTK_ENTRY(fields->product_cost)));
    float discount = atof(gtk_entry_get_text(GTK_ENTRY(fields->product_discount)));
    passthrough = new_product_v2(id, name_final, cost);
    passthrough->product_discount = discount;
    int success = process_product_fields(passthrough, auto_increment);
    if (success == 1){
        empty_fields(fields);
    }
}
// static void updatefields(GtkWidget *widget, ProductFieldSet *fields){
//     fields->product_id = gtk_builder_get_object(fields->builder, "product_id");
//     fields->product_name = gtk_builder_get_object(fields->builder, "product_name");
//     fields->product_cost = gtk_builder_get_object(fields->builder, "product_cost");
//     fields->product_discount = gtk_builder_get_object(fields->builder, "product_discount");
// }
static void pull_product(GtkWidget *widget, ProductFieldSet *fields){
    fill_product_fields(fields);
}