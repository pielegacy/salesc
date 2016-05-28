#include<gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include "salesguihelpers.h"
#include <sqlite3.h>
#include <unistd.h>

const int auto_increment = 0; // Does the DB AutoIncrement?
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
static void view_sale_list_window(GtkWidget *widget, gpointer thing);
static void refresh_window(GtkWidget *widget, GObject *sale_list);
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
    //GObject *sale_list;
    GObject *refresh_sales;
    db_create(auto_increment);
    // I <3 James Qu      
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "ui/menu.ui", NULL);
    menu_window = gtk_builder_get_object(builder, "mainwindow");    
    new_sale = gtk_builder_get_object(builder, "new_sale");
    new_product = gtk_builder_get_object(builder, "new_product");
    //sale_list = gtk_builder_get_object(builder, "sales_list");
    refresh_sales = gtk_builder_get_object(builder, "refresh_sales");
    //fill_sales(sale_list);
    g_signal_connect(menu_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(refresh_sales, "clicked", G_CALLBACK(view_sale_list_window), NULL);
    g_signal_connect(new_sale, "clicked", G_CALLBACK(new_sale_window), builder);
    g_signal_connect(new_product, "clicked", G_CALLBACK(new_product_window), builder);
    //gtk_window_fullscreen(GTK_WINDOW(menu_window));
    new_sale_group();
    gtk_main();

    return 0;
}
static void view_sale_list_window(GtkWidget *widget, gpointer thing){
    GtkBuilder *builder;
    GObject *sale_list;
    GObject *menu_window;
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "ui/sale_directory.ui", NULL);
    
    menu_window = gtk_builder_get_object(builder, "mainwindow"); 
    sale_list = gtk_builder_get_object(builder, "sales_list");
    
    fill_sales(sale_list);
}
static void refresh_window(GtkWidget *widget, GObject *sale_list){
    printf("refreshing...\n");
    fill_sales(sale_list);
}
// Fill Sales in a list
// TODO : Fix inconsistencies with count
void fill_sales(GObject *sale_list){
    sqlite3 *db;
    int rc;
    int currentsale = 1;
    int currentsalecount = 0;
    int list_size_current = 0;
    char *currentitemlist = malloc(sizeof("") + 1);
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    sqlite3_stmt *result;
    sqlite3_prepare_v2(db, "SELECT * FROM SALES;", 128, &result, NULL); 
    float payment;
    gtk_list_box_select_all(GTK_LIST_BOX(sale_list));
    GList *list_version = gtk_list_box_get_selected_rows(GTK_LIST_BOX(sale_list));
    guint list_size = g_list_length(list_version);
    printf("The size of the list is %d", list_size);
    while ((rc = sqlite3_step(result)) != SQLITE_DONE){
        if (currentsale != sqlite3_column_int(result, 1)){
            if (list_size_current > list_size){
            GtkWidget *sale_option;
            char *label_final = malloc(strlen(sqlite3_column_text(result,1)) + sizeof(currentsalecount) + sizeof(payment) + (20 * sizeof(char)));
            sprintf(label_final, "%d : %d item/s for $%0.2f",currentsale, currentsalecount, payment);
            //const 
            GtkWidget *product_option = gtk_button_new_with_label(label_final);
            g_signal_connect(product_option, "clicked", G_CALLBACK(view_sale_window), NULL);
            gtk_list_box_insert(GTK_LIST_BOX(sale_list), product_option, 100);
            currentsale = sqlite3_column_int(result, 1);
            currentsalecount = 1;
            }
            list_size_current++;
        }
        else {
            payment = payment_amount_retrieve(sqlite3_column_int(result,3));
            currentsalecount++;
        }
    }
    gtk_widget_show_all(GTK_WIDGET(sale_list));
    sqlite3_finalize(result);
    sqlite3_close(db);
    free(currentitemlist);
}
static void clear_window(GtkWidget *widget, GtkWidget *window){
    gtk_window_close(GTK_WINDOW(window));
}
// View sale window, Sales, Receipt
// Generates a "receipt" from the ID found in the button pressed
// SERIOUS TODO : Fix inconsistencies with sale count and such
static void view_sale_window(GtkWidget *widget, gpointer sale_pointer){
    GtkBuilder *builder;
    GObject *menu_window;
    GObject *sale_product_list;
    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    GtkTextIter iter;
    
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
    float payment_amount;
    // printf("%s\n", output);
    gtk_builder_add_from_file(builder, "ui/sale_details.ui", NULL);
    menu_window = gtk_builder_get_object(builder, "mainwindow");
    sale_product_list = gtk_builder_get_object(builder, "sale_receipt");
    
    // Buffer Code
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sale_product_list));
    gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
    gtk_text_buffer_insert(buffer, &iter, "-- Sales Receipt --\n", -1);
    char group_string[20];
    sprintf(group_string,"Sale #%d\n\n", sale_group);
    gtk_text_buffer_insert(buffer, &iter, group_string, -1);
    float total_cost = 0.0;
    int paytype = 0;
    char *sql_statement = sqlite3_mprintf("SELECT * FROM SALES WHERE SALE_GROUP = %d;", sale_group);
    sqlite3_prepare_v2(db, sql_statement, 128, &result, NULL); 
    while ((rc = sqlite3_step(result)) != SQLITE_DONE){
        Product *temp = malloc(sizeof(Product) + 1);
        temp = search_product(sqlite3_column_int(result, 2));
        payment_amount = payment_amount_retrieve(sqlite3_column_int(result, 3));
        total_cost += sqlite3_column_double(result, 4);
        paytype = payment_type_retrieve(sqlite3_column_int(result, 3));
        char *product_string = malloc(sizeof(sqlite3_column_double(result, 4)) + strlen(temp->product_name) + sizeof(temp->product_id) + 20);
        sprintf(product_string, "$%0.2f - %s (%d)\n", sqlite3_column_double(result, 4), temp->product_name, temp->product_id);
        gtk_text_buffer_insert(buffer, &iter, product_string, -1);
        //free(product_string);
        //free(temp);
    }
    float change = 0.00;
    if (payment_amount > total_cost)
        change = payment_amount - total_cost;
    float gst = total_cost / 10;
    char *ending = malloc(sizeof(total_cost) + sizeof(payment_amount) + sizeof(char[60]) + sizeof(change) + sizeof(gst) + 1); // I know the vague mallocs are bad, soz babes
    sprintf(ending, "\nTotal : $%0.2f\nGST Included : $%0.2f\nPaid : $%0.2f\nChange : $%0.2f\n\n", total_cost, gst, payment_amount, change);
    gtk_text_buffer_insert(buffer, &iter, ending, -1);
    char paytypestring[40] = "Payment Type : Other";
    switch (paytype){
        case 0:
            strcpy(paytypestring, "Payment Type : Cash");
        break;
        case 1:
            strcpy(paytypestring, "Payment Type : Debit");
        break;
        case 2:
            strcpy(paytypestring, "Payment Type : Credit");
        break;
        case 3:
            strcpy(paytypestring, "Payment Type : Cheque");
        break;
        
    }
    gtk_text_buffer_insert(buffer, &iter, paytypestring, -1);
    free(ending);
    sqlite3_finalize(result);
    sqlite3_close(db);
    
}
// The new Sale Window
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
    
    int *val;
    // I <3 James Qu      
    builder = gtk_builder_new();
    db_create(1);
    gtk_builder_add_from_file(builder, "ui/main.ui", NULL);
    sale_window = gtk_builder_get_object(builder, "mainwindow");

    product_search = gtk_builder_get_object(builder, "product_entry");
    //process_button = gtk_builder_get_object(builder, "payment_process");
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
    //memcpy(searchtolist->values, val, sizeof(val) + 1);
    
    product_list = gtk_builder_get_object(builder, "product_list");
    fill_product_list(product_list, searchtolist);
    g_signal_connect(product_search, "activate", G_CALLBACK(add_sale_list), searchtolist);
    //g_signal_connect(process_button, "clicked", G_CALLBACK(clear_window), sale_window);
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
// Fills the list of available products to sell
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
        g_signal_connect(product_option, "clicked", G_CALLBACK(add_from_list), fields);
        gtk_list_box_insert(GTK_LIST_BOX(list), product_option, 100);
    }
    sqlite3_finalize(result);
    gtk_widget_show_all(GTK_WIDGET(list));
    sqlite3_close(db);
}

// Adds a product to the sale list
static void add_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    GtkWidget *label;
    sqlite3 *db;
    int rc;
    SearchSubmitPair *temp = malloc(sizeof(SearchSubmitPair) + 1);
    memcpy(temp, pair, sizeof(pair) + 1);
    Product *searchproduct = search_product(atoi(gtk_entry_get_text(GTK_ENTRY(pair->input))));
    new_sale_group();
    if (strcmp(searchproduct->product_name, "DOESNT_EXIST") != 0){
        char *res = malloc(strlen(searchproduct->product_name)*2 + 1); // I know this is dodgy, will fix late
        sprintf(res, "$%0.2f : %s", (searchproduct->product_cost - (searchproduct->product_cost * searchproduct->product_discount)), searchproduct->product_name);
        const char* label_text = res;
        label = gtk_label_new(label_text);
        printf("%s\n", label_text);
        // Dyanmic Memory Allocation
        int *temp_list = malloc(sizeof(int) * 1 + 1);
        //pair->values[pair->count] = searchproduct->product_id;
        pair->count++;
        temp_list = (int *)realloc(pair->values, sizeof(int) * pair->count);
        if (temp_list){
            pair->values = temp_list;
            pair->values[pair->count - 1] = searchproduct->product_id;
        }
        else {
            printf("Dun didn't work");
        }
        //printf("%s added for $%0.2f, size of sale increased to : %d\n", searchproduct->product_name, searchproduct->product_cost, pair->count);
        gtk_list_box_insert(GTK_LIST_BOX(pair->output), label, 100);
        //g_signal_connect(GTK_WIDGET(label), "clicked", G_CALLBACK(gtk_main_quit), NULL);  
        gtk_widget_show_all(GTK_WIDGET(pair->output));
    }
    //free(temp_list);
    //free(searchproduct);
    gtk_entry_set_text(GTK_ENTRY(pair->input), "");
}

// Completes the sale 
static void total_sale_list(GtkWidget *widget, SearchSubmitPair *pair){
    int i;
    float cost = 0.00;
    Product *temp;
    char* placeholder = malloc(strlen("0000000000.00") + 1);
    for (i = 0; i < pair->count; i++){
        if (pair->values[i] != 0){
            temp = malloc(sizeof(search_product(pair->values[i])) + 1);
            temp = search_product(pair->values[i]);
            //printf("Value %s is %0.2f\n", temp->product_name, temp->product_cost);
            cost += temp->product_cost - (temp->product_cost * temp->product_discount);
        }
        else {
            break;
        }
    }
    float paid = atof(gtk_entry_get_text(GTK_ENTRY(widget)));
    if (paid >= cost && cost != 0 && atof(gtk_entry_get_text(GTK_ENTRY(widget))) != 0){
        sale_success(widget, pair);
        if (paid > cost){
            GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
            GtkWidget *dialog;
            dialog = gtk_message_dialog_new (GTK_WINDOW(pair->window),
                                        flags,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_CLOSE,
                                        "$%0.2f change",
                                        paid - cost);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_window_close(GTK_WINDOW(pair->window));
        }
        else {
            gtk_window_close(GTK_WINDOW(pair->window));
        }
        //free(temp);
        //free(pair);
    }
    else {
        sprintf(placeholder, "%0.2f", cost);
        gtk_entry_set_text(GTK_ENTRY(widget), placeholder);
    }
    //free(temp);
}

static void show_price(GtkWidget *widget, SearchSubmitPair *pair){
    int i = 0;
    printf("The count is %d\n", pair->count);
}
// Simple method to clear text of a payment widget
static void clear_payment_field(GtkWidget *widget, SearchSubmitPair *pair){
    gtk_entry_set_text(GTK_ENTRY(widget), "");
}
// Called when a sale is successful
static void sale_success(GtkWidget *paywidget, SearchSubmitPair *pair){
    const gchar *widgettype = gtk_widget_get_name(paywidget);
    printf("TYPE IS %s\n", widgettype);
    printf("Processing payment...\n");
    Payment *temp = malloc(sizeof(Payment) + 1);
    int pay_type;
    float paid = atof(gtk_entry_get_text(GTK_ENTRY(paywidget)));
    // Possibly could be done better with an enumeration, not worth the bug fixing tho
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
    temp = new_payment(0,pay_type,paid);
    add_payment(temp);
    int pay_id = recent_payment_id();
    int sale_group = new_sale_group();
    int i;
    for (i = 0; i < pair->count; i++){
        if (pair->values[i] != 0){
            Product *check = malloc(sizeof(Product) + 1);
            check = search_product(pair->values[i]);
            float actual_cost = check->product_cost - (check->product_cost * check->product_discount);
            printf("Actual cost : $%0.2f", actual_cost);
            add_sell_from_id(new_sell_from_id(sale_group, pair->values[i], pay_id, actual_cost));
        }
        else {
            break;
        }
    }
    //free(temp);	
}

static void update_products(GtkWidget *widget, ProductFieldSet *fields);
static void pull_product(GtkWidget *widget, ProductFieldSet *fields);
static void updatefields(GtkWidget *widget, ProductFieldSet *fields);
// Product Window, New Product //
// Displays the window used to add and update products, possibly remove oldbuilder for future releases
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
}
static void update_products(GtkWidget *widget, ProductFieldSet *fields){
    Product *passthrough = malloc(sizeof(Product) + 1);
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(fields->product_id)));
    char *name_final = malloc(sizeof(fields->product_name) + 1);
    strcpy(name_final, gtk_entry_get_text(GTK_ENTRY(fields->product_name)));
    float cost = atof(gtk_entry_get_text(GTK_ENTRY(fields->product_cost)));
    float discount = atof(gtk_entry_get_text(GTK_ENTRY(fields->product_discount)));
    passthrough = new_product_v2(id, name_final, cost, discount);
    passthrough->product_discount = discount;
    int success = process_product_fields(passthrough, auto_increment);
    if (success == 1){
        empty_fields(fields);
    }
}
// Used as a middle ground to fill the products in the list
static void pull_product(GtkWidget *widget, ProductFieldSet *fields){
    fill_product_fields(fields);
}