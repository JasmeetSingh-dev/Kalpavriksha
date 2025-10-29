#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct 
{
    int id;
    char name[100];
    float price;
    int quantity;
} Product;

typedef enum 
{
    ADD_PRODUCT = 1,
    VIEW_PRODUCTS,
    UPDATE_QUANTITY,
    SEARCH_BY_ID,
    SEARCH_BY_NAME,
    SEARCH_BY_PRICE_RANGE,
    DELETE_PRODUCT,
    EXIT_PROGRAM
} MenuOption;

void addNewProduct(Product **products, int *count) 
{
    *products = (Product *)realloc(*products, (*count + 1) * sizeof(Product));
    if (*products != NULL) 
    {
    Product *newProduct = &((*products)[*count]);
    printf("\nEnter Product ID: ");
    scanf("%d", &newProduct->id);
    printf("Enter Product Name: ");
    scanf("%s", newProduct->name);
    printf("Enter Price: ");
    scanf("%f", &newProduct->price);
    printf("Enter Quantity: ");
    scanf("%d", &newProduct->quantity);
    } 
    else 
    {
        printf("Memory allocation failed!\n");
        return;
    }

    (*count)++;
    printf("Product added successfully!\n");
}

void viewAllProducts(const Product *products, const int count) 
{
    if (count > 0) 
    {
        printf("\n====== PRODUCT LIST ======\n");
        for (int index = 0; index < count; index++) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", products[index].id, products[index].name, products[index].price, products[index].quantity);
        }
    }
    else
    {
        printf("No products available.\n");
    }
}

void updateQuantity(Product *products, const int count) 
{
    int id, newQuantity;
    bool found = false;
    printf("Enter Product ID to update quantity: ");
    scanf("%d", &id);

    for (int index = 0; index < count; index++) 
    {
        if (products[index].id == id) 
        {
            printf("Enter new quantity: ");
            scanf("%d", &newQuantity);
            products[index].quantity = newQuantity;
            printf("Quantity updated successfully.\n");
            found = true;
            break;
        }
    }
    if (!found) 
    {
        printf("Product not found.\n");
    }
}

void searchById(const Product *products, const int count) 
{
    int id;
    bool found = false;
    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    for (int index = 0; index < count; index++) 
    {
        if (products[index].id == id) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", products[index].id, products[index].name, products[index].price, products[index].quantity);
            found = true;
            break;
        }
    }
    if (!found) 
    {
        printf("Product not found.\n");
    }
}

void searchByName(const Product *products, const int count) 
{
    char nameSearch[100];
    bool found = false;

    printf("Enter name or substring to search: ");
    scanf("%s", nameSearch);

    for (int index = 0; index < count; index++) 
    {
        if (strstr(products[index].name, nameSearch)) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", products[index].id, products[index].name, products[index].price, products[index].quantity);
            found = true;
        }
    }

    if (!found)
    {
        printf("No matching products found.\n");
    }
}

void searchByPriceRange(const Product *products, const int count) 
{
    float minPrice, maxPrice;
    bool found = false;

    printf("Enter minimum price: ");
    scanf("%f", &minPrice);
    printf("Enter maximum price: ");
    scanf("%f", &maxPrice);

    for (int index = 0; index < count; index++) 
    {
        if (products[index].price >= minPrice && products[index].price <= maxPrice) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", products[index].id, products[index].name, products[index].price, products[index].quantity);
            found = true;
        }
    }

    if (!found)
    {
        printf("No products found in this price range.\n");
    }
}

void deleteProduct(Product **products, int *count) 
{
    int id, foundIndex = -1;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    for (int index = 0; index < *count; index++) 
    {
        if ((*products)[index].id == id) 
        {
            foundIndex = index;
            break;
        }
    }

    if (foundIndex == -1) 
    {
        printf("Product not found.\n");
        return;
    }

    for (int index = foundIndex; index < *count - 1; index++) 
    {
        (*products)[index] = (*products)[index + 1];
    }

    *products = (Product *)realloc(*products, (*count - 1) * sizeof(Product));
    (*count)--;

    printf("Product deleted successfully.\n");
}

int main() 
{
    int totalProducts;
    printf("Enter initial number of products: ");
    scanf("%d", &totalProducts);

    Product *inventory = (Product *)calloc(totalProducts, sizeof(Product));
    if (inventory == NULL) 
    {
        printf("Memory allocation failed!\n");
        return -1;
    }

    for (int index = 0; index < totalProducts; index++) 
    {
        printf("\nEnter details for product %d:\n", index + 1);
        printf("Product ID: ");
        scanf("%d", &inventory[index].id);
        printf("Product Name: ");
        scanf("%s", inventory[index].name);
        printf("Price: ");
        scanf("%f", &inventory[index].price);
        printf("Quantity: ");
        scanf("%d", &inventory[index].quantity);
    }

    int choice;
    do 
    {
        printf("\n\t\t INVENTORY MENU \n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch ((MenuOption)choice) 
        {
            case ADD_PRODUCT:
                addNewProduct(&inventory, &totalProducts);
                break;
            case VIEW_PRODUCTS:
                viewAllProducts(inventory, totalProducts);
                break;
            case UPDATE_QUANTITY:
                updateQuantity(inventory, totalProducts);
                break;
            case SEARCH_BY_ID:
                searchById(inventory, totalProducts);
                break;
            case SEARCH_BY_NAME:
                searchByName(inventory, totalProducts);
                break;
            case SEARCH_BY_PRICE_RANGE:
                searchByPriceRange(inventory, totalProducts);
                break;
            case DELETE_PRODUCT:
                deleteProduct(&inventory, &totalProducts);
                break;
            case EXIT_PROGRAM:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != EXIT_PROGRAM);

    free(inventory);
    return 0;
}
