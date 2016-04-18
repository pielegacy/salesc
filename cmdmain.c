#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salesbase.h"
#include <sqlite3.h>
#include <unistd.h>


// Main loop
int main(int argc, char *argv[]){
    Product *test = new_product(1000, "Not A Computer Science Student", 420.0);
    PaymentType testpaytype = DEBIT;
    Product *testproduct = new_product(1030, "Ytes2", 32.0);
    Payment *testpay = new_payment(0, testpaytype, 2.45);
    Sell *testsale = new_sell(30, testproduct, testpaytype, 100.0);
    printf("Sales.c Command Line Client\n");
    printf("Development Build 0.2\n");
    db_create();
    // test_value();
    add_product(test);
    add_payment(testpay);
    add_sell(testsale);
    // last_sale();
    console_readproductnames();
    // printf("The version of Sqlite3 that is being ran is version %s\n", sqlite3_libversion());
    getchar();
    return 0;
}