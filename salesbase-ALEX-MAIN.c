#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include <sqlite3.h>

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
Product new_product(int id, char name[120], float cost){
    Product temp;
    temp.product_id = id;
    strcpy(temp.product_name, name);
    temp.product_cost = cost;
    temp.product_discount = 0.00;
    return temp;
}
Sale new_sale(int id, Product *items, PaymentType paytype, float received){
    Sale temp;
    temp.sale_id = id;
    memcpy(temp.sale_items, items, sizeof(items));
    temp.sale_payment = new_payment(temp.sale_id, paytype, received); // 100 is placeholder code
    return temp;
}
Payment new_payment(int id, PaymentType paytype, float received){
    Payment temp;
    temp.payment_id = id;
    temp.payment_type = paytype;
    temp.payment_type = received;
    return temp;
}
void create_db(){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *errMessage = 0;
    if (!rc){
        printf("Created datbase...\n");
    }
    
    char *productsql = "CREATE TABLE PRODUCTS(" \
        "INT PRODUCT_ID PRIMARY KEY NOT NULL," \
        "PRODUCT_NAME VARCHAR(120) NOT NULL," \
        "PRODUCT_COST FLOAT NOT NULL," \
        "PRODUCT_DISCOUNT FLOAT);";
        
    char *salessql = "CREATE TABLE SALES(" \
    "INT SALE_ID PRIMARY KEY NOT NULL," \
    "SALE_GROUP INT NOT NULL," \
    "SALE_ITEM_ID INT NOT NULL," \
    "SALE_PAYMENT_ID INT NOT NULL);";
    
    char *paymentssql = "CREATE TABLE PAYMENTS(" \
    "INT PAYMENT_ID PRIMARY KEY NOT NULL," \
    "PAYMENT_TYPE INT NOT NULL," \
    "PAYMENT_AMOUNT FLOAT NOT NULL);";
    
    rc = sqlite3_exec(db, productsql, callback, 0, &errMessage);
    rc = sqlite3_exec(db, salessql, callback, 0, &errMessage);
    rc = sqlite3_exec(db, paymentssql, callback, 0, &errMessage);
    printf("Database created!\n\n");        
    sqlite3_close(db);
}

void console_readproductnames(){
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("salesc.db", &db);
    char *errmessage = 0;
    const char *tail;
    sqlite3_stmt *result;
    printf("--BEGIN LIST--\n");
    sqlite3_prepare(db, "SELECT * FROM PRODUCTS", 128, &result, &tail);
    while (sqlite3_step(result) == SQLITE_ROW){
        printf("%s for $%d\n", sqlite3_column_text(result, 1), sqlite3_column_double(result, 2));
    }
    printf("--END LIST--\n");
}