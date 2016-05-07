#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include <sqlite3.h>
#include <unistd.h>

/*
    Sales.c Base File
    This is the main file in which the basic functions of Sales.c are kept
    Find us on github! https://github.com/pielegacy/salesc
    (You probably have already coz you have the source code lol)
    (Unless you're me, or a tutor)
    (If the latter, hey Steve or Andrew. How have you guys been?)
*/

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
Product *new_product(int id, char name[120], float cost, float discount){
    Product *temp = malloc(sizeof(Product));
    temp->product_id = id;
    strcpy(temp->product_name, name);
    temp->product_cost = cost;
    temp->product_discount = discount;
    return temp;
}
Product *new_product_v2(int id, char *name, float cost, float discount){
    Product *temp = malloc(sizeof(Product));
    temp->product_id = id;
    strcpy(temp->product_name, name);
    temp->product_cost = cost;
    temp->product_discount = discount;
    return temp;
}
void *update_product(Product *product){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    char *testsql = sqlite3_mprintf("UPDATE PRODUCTS SET PRODUCT_NAME='%s', PRODUCT_COST=%0.2f, PRODUCT_DISCOUNT=%0.2f WHERE PRODUCT_ID=%d;", product->product_name, product->product_cost, product->product_discount, product->product_id);
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    sqlite3_close(db);
}
SellFromID *new_sell_from_id(int group, int product, int payment)
{
     SellFromID *temp = malloc(sizeof(SellFromID));
     temp->sale_group = group;
     temp->product_id = product;
     temp->payment_id = payment;
     
}
Payment *new_payment(int id, int paytype, float received){
    Payment *temp = malloc(sizeof(Payment));
    temp->payment_id = id;
    temp->payment_type = paytype;
    temp->payment_amount = received;
    return temp;
}
// Autoinc is often changed in the mainwindow.c file
int db_create(int autoinc){
    if (access("salesc.db", F_OK) == -1){
        sqlite3 *db;
        int rc;
        rc = sqlite3_open("salesc.db", &db);
        char *errMessage = 0;
        if (!rc){
            printf("Created datbase...\n");
        }
        // Using real numbers instead of floating point numbers (REAL = Float in Sqlite)
        if (autoinc == 1){
            char *productsql = "CREATE TABLE PRODUCTS(" \
                "PRODUCT_ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                "PRODUCT_NAME VARCHAR(120) NOT NULL," \
                "PRODUCT_COST REAL NOT NULL," \
                "PRODUCT_DISCOUNT REAL);";
                
            char *salessql = "CREATE TABLE SALES(" \
            "SALE_ID INTEGER PRIMARY KEY AUTOINCREMENT," \
            "SALE_GROUP INTEGER NOT NULL," \
            "SALE_ITEM_ID INTEGER NOT NULL," \
            "SALE_PAYMENT_ID INTEGER NOT NULL);";
            
            char *paymentssql = "CREATE TABLE PAYMENTS(" \
            "PAYMENT_ID INTEGER PRIMARY KEY AUTOINCREMENT," \
            "PAYMENT_TYPE INTEGER NOT NULL," \
            "PAYMENT_AMOUNT REAL NOT NULL);";
            rc = sqlite3_exec(db, productsql, callback, 0, &errMessage);
            rc = sqlite3_exec(db, salessql, callback, 0, &errMessage);
            rc = sqlite3_exec(db, paymentssql, callback, 0, &errMessage); 
        }
        else {
            char *productsql = "CREATE TABLE PRODUCTS(" \
                "PRODUCT_ID INTEGER PRIMARY KEY," \
                "PRODUCT_NAME VARCHAR(120) NOT NULL," \
                "PRODUCT_COST REAL NOT NULL," \
                "PRODUCT_DISCOUNT REAL);";
                
            char *salessql = "CREATE TABLE SALES(" \
            "SALE_ID INTEGER PRIMARY KEY AUTOINCREMENT," \
            "SALE_GROUP INTEGER NOT NULL," \
            "SALE_ITEM_ID INTEGER NOT NULL," \
            "SALE_PAYMENT_ID INTEGER NOT NULL);";
            
            char *paymentssql = "CREATE TABLE PAYMENTS(" \
            "PAYMENT_ID INTEGER PRIMARY KEY AUTOINCREMENT," \
            "PAYMENT_TYPE INTEGER NOT NULL," \
            "PAYMENT_AMOUNT REAL NOT NULL);";
            rc = sqlite3_exec(db, productsql, callback, 0, &errMessage);
            rc = sqlite3_exec(db, salessql, callback, 0, &errMessage);
            rc = sqlite3_exec(db, paymentssql, callback, 0, &errMessage);   
        }   
        return autoinc;
        sqlite3_close(db);
    }
}
void console_readproductnames(){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    sqlite3_stmt *result;
    sqlite3_prepare_v2(db, "SELECT * FROM PRODUCTS", 128, &result, NULL);
    printf("-- BEGIN LIST --\n");   
    while ((rc = sqlite3_step(result)) == SQLITE_ROW){
        printf("%d : %s for $%0.2f\n",sqlite3_column_int(result, 0), sqlite3_column_text(result, 1), sqlite3_column_double(result, 2));
    }
    printf("-- END LIST --");
    sqlite3_finalize(result);
    sqlite3_close(db);
}
// ADDS A TEST VALUE (REMOVE IN FINAL BUILD)
void test_value(){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    char *testsql = "INSERT INTO PRODUCTS (PRODUCT_NAME, PRODUCT_COST) VALUES (\"Alex\", 5.2);\n";
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    sqlite3_close(db);
}
// Adds a product to the database
void add_product(Product *product, int dbtype){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    char *testsql = sqlite3_mprintf("INSERT INTO PRODUCTS (PRODUCT_NAME, PRODUCT_COST) VALUES (\"%s\", %0.2f);", product->product_name, product->product_cost);
    if (dbtype != 1){
        testsql = sqlite3_mprintf("INSERT INTO PRODUCTS (PRODUCT_ID,PRODUCT_NAME, PRODUCT_COST) VALUES (%d,\"%s\", %0.2f);",product->product_id, product->product_name, product->product_cost);
    }
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    sqlite3_close(db);
}
// Adds a payment to the database
void add_payment(Payment *payment){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    char *testsql = sqlite3_mprintf("INSERT INTO PAYMENTS (PAYMENT_TYPE, PAYMENT_AMOUNT) VALUES (%d, %0.2f);", payment->payment_type, payment->payment_amount);
    printf("%s \n",testsql);
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    printf("ERROR : %s\n", err);
    sqlite3_close(db);
}
void add_sell_from_id(SellFromID *sell){
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    char *testsql = sqlite3_mprintf("INSERT INTO SALES (SALE_GROUP, SALE_ITEM_ID, SALE_PAYMENT_ID) VALUES (%d, %d, %d);", sell->sale_group, sell->product_id, sell->payment_id);  
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    sqlite3_close(db);
}

// Returns Product From DB based on ID
Product *search_product(int id){
    Product *temp;
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    sqlite3_stmt *result;
    char *err = 0;
    //printf("SQL : SELECT * FROM PRODUCTS WHERE PRODUCT_ID = %d;\n", id);
    char *search = sqlite3_mprintf("SELECT * FROM PRODUCTS WHERE PRODUCT_ID = %d;", id);
    sqlite3_prepare_v2(db, search, 128, &result, NULL);
    if ((rc = sqlite3_step(result)) == SQLITE_ROW){
        const unsigned char *name = sqlite3_column_text(result, 1);
        char *actualname = strdup(name);
        temp = new_product(sqlite3_column_int(result, 0), actualname, sqlite3_column_double(result, 2), sqlite3_column_double(result, 3));
    }
    else {
        temp = new_product(sqlite3_column_int(result, 0), "DOESNT_EXIST", sqlite3_column_double(result, 2), sqlite3_column_double(result, 3));
    }
    printf("%0.2f\n", temp->product_discount);
    sqlite3_reset(result);
    sqlite3_finalize(result);
    sqlite3_close(db);
    return temp;
}

int recent_payment_id(){
    int temp;
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    sqlite3_stmt *result;
    char *err = 0;
    char *search = sqlite3_mprintf("SELECT PAYMENT_ID FROM PAYMENTS ORDER BY PAYMENT_ID DESC LIMIT 1");
    sqlite3_prepare_v2(db, search, 128, &result, NULL);
    if ((rc = sqlite3_step(result)) == SQLITE_ROW){
        temp = sqlite3_column_int(result, 0);
    }
    printf("THE LAST PAYMENT VALUE IS %d\n", temp);
    sqlite3_reset(result);
    sqlite3_close(db);
    return temp;
}
float payment_amount_retrieve(int id){
    float temp;
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    sqlite3_stmt *result;
    char *err = 0;
    char *search = sqlite3_mprintf("SELECT PAYMENT_AMOUNT FROM PAYMENTS WHERE PAYMENT_ID = %d;", id);
    sqlite3_prepare_v2(db, search, 128, &result, NULL);
    if ((rc = sqlite3_step(result)) == SQLITE_ROW){
        temp = sqlite3_column_int(result, 0);
    }
    sqlite3_reset(result);
    sqlite3_close(db);
    return temp;
}
int payment_type_retrieve(int id){
    int temp;
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    sqlite3_stmt *result;
    char *err = 0;
    char *search = sqlite3_mprintf("SELECT PAYMENT_TYPE FROM PAYMENTS WHERE PAYMENT_ID = %d;", id);
    sqlite3_prepare_v2(db, search, 128, &result, NULL);
    if ((rc = sqlite3_step(result)) == SQLITE_ROW){
        temp = sqlite3_column_int(result, 0);
    }
    sqlite3_reset(result);
    sqlite3_close(db);
    return temp;
}
int new_sale_group(){
    int res = 0;
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    sqlite3_stmt *result;
    char *err = 0;
    char *search = "SELECT * FROM SALES;";
    sqlite3_prepare_v2(db, search, 128, &result, NULL);
    while ((rc = sqlite3_step(result)) == SQLITE_ROW){
        if (sqlite3_column_int(result, 1) > res){
            res = sqlite3_column_int(result, 1);
        }
    }
    res += 1;
    printf("NEWEST SALE GROUP : %d\n", res);
    sqlite3_reset(result);
    sqlite3_close(db);
    return res;
}
char *price_string_concat(float price, char *string){
    char price_string[10];
    sprintf(price_string, "%0.2f", price);
    char *res = malloc(strlen(price_string) + strlen(string) + 1);
    sprintf(res, "$%s : %s", price_string, string);
    return res;
}