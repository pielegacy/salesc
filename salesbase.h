#ifndef SALESBASE_H_
#define SALESBASE_H_

// LOGIC FUNCTIONS //
// A product is the way in which we store items
typedef struct Products{
    int product_id;
    char product_name[120];
    float product_cost;
    float product_discount;
} Product;

// Holds the payment types
typedef enum {CASH, DEBIT, CREDIT, CHEQUE, OTHER} PaymentType;

// A payment is a monetary gain
typedef struct Payments{
    int payment_id;
    int payment_type;
    float payment_amount;
} Payment;

// DEPRECATED AS IT DOESN'T WORK
// A sale holds multiple products
// This needs to be fixed for DB support and sale_payment needs to be a pointer
// I'm sorry the database isn't dynamic :'(
typedef struct Sales{
    int sale_id;
    Product *sale_items[100];
    Payment *sale_payment;
} Sale;
// Temporary sales class whilst I try to get other shit working
typedef struct Sells{
    int sale_group;
    Product *sale_item;
    Payment *sale_payment;
} Sell;
// Most likely the final form of a sale class
typedef struct SellFromIDs{
    int sale_group;
    int product_id;
    int payment_id;
} SellFromID;
// Add a new product
Product *new_product(int id, char name[120], float cost, float discount);
// V2 Of new product
Product *new_product_v2(int id, char *name, float cost, float discount);
// Update product
void *update_product(Product *product);
// Add a new sale [NOT WORKING]
Sale *new_sale(int id, Product **items, PaymentType paytype, float received);
// Adds a sell (Sales prototype)
Sell *new_sell(int group, Product *product, PaymentType paytype, float received);
// Adds a sell using IDs
SellFromID *new_sell_from_id(int group, int product, int payment);
// Add a new Payment
Payment *new_payment(int id, int paytype, float received);
// SQL Callback (Taken from http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm)
// Initialize DB
int db_create(int autoinc);
// Shortcut command for inserting into database
void db_insert();

// Read Products (test function)
void console_readproductnames();
// Add Test Value
void test_value();
// Add Product to Db
void add_product(Product *product, int dbtype);
// Add payment to Db
void add_payment(Payment *payment);
// Add sale to Db
void add_sale(Sale *sale);
// Add sell to Db
void add_sell(Sell *sell);
// Add sell using ID
void add_sell_from_id(SellFromID *sell);
// Returns pointer to Product From DB based on ID
Product *search_product(int id);
// Find new sale_group
int new_sale_group();
// Display price - string values
char *price_string_concat(float price, char *string);
// Returns most recent payment ID
int recent_payment_id();
// Get paymount amount by id
float payment_amount_retrieve(int id);
// Get payment type by id
int payment_type_retrieve(int id);
#endif