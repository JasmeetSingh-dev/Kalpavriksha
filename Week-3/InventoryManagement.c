#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int productId;
    char productName[100];
    float productPrice;
    int productQuantity;
} Product;

void addNewProduct(Product **products, int *count) 
{
    *products = (Product *)realloc(*products, (*count + 1) * sizeof(Product));
    if (*products == NULL) 
    {
        printf("Memory reallocation failed!\n");
        return;
    }

    Product *newProduct = &((*products)[*count]);
    printf("\nEnter Product ID: ");
    scanf("%d", &newProduct->productId);
    printf("Enter Product Name: ");
    scanf("%s", newProduct->productName);
    printf("Enter Price: ");
    scanf("%f", &newProduct->productPrice);
    printf("Enter Quantity: ");
    scanf("%d", &newProduct->productQuantity);

    (*count)++;
    printf("Product added successfully!\n");
}

void viewAllProducts(const Product *products, const int count) 
{
    if (count == 0) 
    {
        printf("No products available.\n");
        return;
    }

    printf("\n====== PRODUCT LIST ======\n");
    for (int index = 0; index < count; index++) 
    {
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               products[index].productId,
               products[index].productName,
               products[index].productPrice,
               products[index].productQuantity);
    }
}

void updateQuantity(Product *products, const int count) 
{
    int id, newQuantity;
    printf("Enter Product ID to update quantity: ");
    scanf("%d", &id);

    for (int index = 0; index < count; index++) 
    {
        if (products[index].productId == id) 
        {
            printf("Enter new quantity: ");
            scanf("%d", &newQuantity);
            products[index].productQuantity = newQuantity;
            printf("Quantity updated successfully.\n");
            return;
        }
    }
    printf("Product not found.\n");
}

void searchById(const Product *products, const int count) 
{
    int id;
    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    for (int index = 0; index < count; index++) 
    {
        if (products[index].productId == id) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[index].productId,
                   products[index].productName,
                   products[index].productPrice,
                   products[index].productQuantity);
            return;
        }
    }
    printf("Product not found.\n");
}

void searchByName(const Product *products, const int count) 
{
    char nameSearch[100];
    int found = 0;

    printf("Enter name or substring to search: ");
    scanf("%s", nameSearch);

    for (int index = 0; index < count; index++) 
    {
        if (strstr(products[index].productName, nameSearch)) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[index].productId,
                   products[index].productName,
                   products[index].productPrice,
                   products[index].productQuantity);
            found = 1;
        }
    }

    if (!found)
        printf("No matching products found.\n");
}

void searchByPriceRange(const Product *products, const int count) 
{
    float minPrice, maxPrice;
    int found = 0;

    printf("Enter minimum price: ");
    scanf("%f", &minPrice);
    printf("Enter maximum price: ");
    scanf("%f", &maxPrice);

    for (int index = 0; index < count; index++) 
    {
        if (products[index].productPrice >= minPrice && products[index].productPrice <= maxPrice) 
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[index].productId,
                   products[index].productName,
                   products[index].productPrice,
                   products[index].productQuantity);
            found = 1;
        }
    }

    if (!found)
        printf("No products found in this price range.\n");
}

void deleteProduct(Product **products, int *count) 
{
    int id, foundIndex = -1;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    for (int index = 0; index < *count; index++) 
    {
        if ((*products)[index].productId == id) 
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
        scanf("%d", &inventory[index].productId);
        printf("Product Name: ");
        scanf("%s", inventory[index].productName);
        printf("Price: ");
        scanf("%f", &inventory[index].productPrice);
        printf("Quantity: ");
        scanf("%d", &inventory[index].productQuantity);
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

        switch (choice) 
        {
            case 1:
                addNewProduct(&inventory, &totalProducts);
                break;
            case 2:
                viewAllProducts(inventory, totalProducts);
                break;
            case 3:
                updateQuantity(inventory, totalProducts);
                break;
            case 4:
                searchById(inventory, totalProducts);
                break;
            case 5:
                searchByName(inventory, totalProducts);
                break;
            case 6:
                searchByPriceRange(inventory, totalProducts);
                break;
            case 7:
                deleteProduct(&inventory, &totalProducts);
                break;
            case 8:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 8);

    free(inventory);
    return 0;
}
