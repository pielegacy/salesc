#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include <sqlite3.h>
#include <unistd.h>

/*
    Sales.c Base File
    This is the main file in which the basic functions of Sales.c are kept
*/

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
Product *new_product(int id, char name[120], float cost){
    Product *temp = malloc(sizeof(Product));
    temp->product_id = id;
    strcpy(temp->product_name, name);
    temp->product_cost = cost;
    temp->product_discount = 0.00;
    return temp;
}
// Sale *new_sale(int id, Product **items, PaymentType paytype, float received){
//     Sale *temp = malloc(sizeof(Sale));
//     temp->sale_id = id;
//     memcpy(temp->sale_items, items, sizeof(items));
//     temp->sale_payment = new_payment(temp->sale_id, paytype, received); // 100 is placeholder code
//     return temp;
// }
Sell *new_sell(int group, Product *product, PaymentType paytype, float received){
    Sell *temp = malloc(sizeof(Sell));
    temp->sale_group = group;
    // memcpy(temp->sale_item, product, sizeof(product));
    temp->sale_item = malloc(sizeof(Product));
    temp->sale_item = product;
    temp->sale_payment = new_payment(1000, paytype, received);
    return temp;    
}
Payment *new_payment(int id, PaymentType paytype, float received){
    Payment *temp = malloc(sizeof(Payment));
    temp->payment_id = id;
    temp->payment_type = paytype;
    temp->payment_amount = received;
    return temp;
}
void db_create(){ //TODO : ADD ABILITY TO INCLUDE TEST VALUES UPON STARTUP
    if (access("salesc.db", F_OK) == -1){
        sqlite3 *db;
        int rc;
        rc = sqlite3_open("salesc.db", &db);
        char *errMessage = 0;
        if (!rc){
            printf("Created datbase...\n");
        }
        // Using real numbers instead of floating point numbers (REAL = Float in Sqlite)
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
        printf("%s for $%0.2f\n", sqlite3_column_text(result, 1), sqlite3_column_double(result, 2));
    }
    printf("-- END LIST --");
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
void add_product(Product *product){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    char *testsql = sqlite3_mprintf("INSERT INTO PRODUCTS (PRODUCT_NAME, PRODUCT_COST) VALUES (\"%s\", %0.2f);", product->product_name, product->product_cost);
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
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    sqlite3_close(db);
}
// Adds a sale (DOESN'T WORK)
// void add_sale(Sale *sale){
//     sqlite3 *db;
//     int rc;
//     int salegroup;
//     rc = sqlite3_open("salesc.db", &db);
//     char *err = 0;
//     sqlite3_stmt *result;
//     sqlite3_prepare_v2(db, "SELECT * FROM SALES ORDER BY SALE_GROUP DESC LIMIT 1", 128, &result, NULL);
//     salegroup = sqlite3_column_int(result, 2) + 1;
//     printf("%d\n", salegroup);
//     int i;
//     for (i = 0; i < 100; i++){
//         if (sale->sale_items[i] != NULL){
//             add_product(sale->sale_items[1]);
//         }
//     }
//     sqlite3_close(db);
// }

// Adds a sell (singular sale)
void add_sell(Sell *sell){
    sqlite3 *db;
    int rc;
    int salegroup, paymentid;
    rc = sqlite3_open("salesc.db", &db);
    char *err = 0;
    add_payment(sell->sale_payment); // Adds the payment
    sqlite3_stmt *result;
    sqlite3_prepare_v2(db, "SELECT * FROM SALES ORDER BY SALE_GROUP DESC LIMIT 1", 128, &result, NULL);
    salegroup = sqlite3_column_int(result, 1) + 1;
    sqlite3_prepare_v2(db, "SELECT * FROM PAYMENTS ORDER BY SALE_GROUP DESC LIMIT 1", 128, &result, NULL);
    paymentid = sqlite3_column_int(result, 0) + 1;
    printf("%d with a payment id of %d\n", salegroup, paymentid);
    char *testsql = sqlite3_mprintf("INSERT INTO SALES (SALE_GROUP, SALE_ITEM_ID, SALE_PAYMENT_ID) VALUES (%d, %d, %d);", sell->sale_group, 420, paymentid);  
    printf("THIS IS A TEST : %s\n", testsql); 
    rc = sqlite3_exec(db, testsql, callback, 0, &err);
    sqlite3_close(db);
}
void last_sale(){
    printf("Last");
}