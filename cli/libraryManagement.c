#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    char *title;
    char *author;
    int stock;
} Book;

typedef struct
{
    Book *data;
    int size;
    int capacity;
} Library;

Library load_file(int);
void free_library(Library *);
void add_book(Library *);
void delete_book(Library *);
void pop(Library *);
bool display(Library *);
void view_books();
char *search(Library *, char *);
void save_file(Library *);
void save_book(Library *, Book *);
void parse_line(const char *line, char *title, char *author, int *stock);

int main()
{
    Library library = load_file(1);
    bool show_display = true;

    while (show_display)
    {
        show_display = display(&library);
    }

    free_library(&library);
    return 0;
}

bool display(Library *library)
{
    int choice;

    puts("\n\n************************************************");
    puts("*                                              *");
    puts("*      Welcome to Your Library System         *");
    puts("*                                              *");
    puts("************************************************\n");
    printf("%-20s\n", "Options:");
    puts("[1] View all books\n");
    puts("[2] Add a book\n");
    puts("[3] Delete a book\n");
    puts("[4] Search for a book\n");
    puts("[5] Exit\n");
    puts("-----------------------------------------------\n");

    printf("Enter your choice (1-5): ");
    scanf("%d", &choice);
    getchar();
    puts("\n-----------------------------------------------");

    if (choice == 1)
    {
        view_books();
    }
    else if (choice == 2)
    {
        add_book(library);
    }
    else if (choice == 3)
    {
        if (library->size > 0)
        {
            pop(library);
            printf("Last book removed from the library.\n");
        }
        else
        {
            printf("Library is empty, nothing to delete.\n");
        }
    }
    else if (choice == 4)
    {
        char search_term[100];
        printf("Enter the title of the book to search: ");
        fgets(search_term, 100, stdin);
        search_term[strcspn(search_term, "\n")] = '\0';

        char *result = search(library, search_term);
        printf("%s\n", result);
    }
    else if (choice == 5)
    {
        return false;
    }
    else
    {
        printf("Invalid choice. Please select a number between 1 and 5.\n");
    }

    return true;
}

Library load_file(int initial_size)
{
    Library library;
    library.data = malloc(initial_size * sizeof(Book));
    library.size = 0;
    library.capacity = initial_size;

    FILE *fptr = fopen("./library.txt", "r");
    if (!fptr)
    {
        perror("Error opening file");
        return library;
    }

    char data[100];
    while (fgets(data, sizeof(data), fptr) != NULL)
    {
        char title[50], author[50];
        int stock;
        parse_line(data, title, author, &stock);

        Book tmp;
        tmp.title = strdup(title);
        tmp.author = strdup(author);
        tmp.stock = stock;

        save_book(&library, &tmp);
    }

    fclose(fptr);
    return library;
}

void add_book(Library *library)
{
    Book user_book;
    user_book.title = malloc(100 * sizeof(char));
    user_book.author = malloc(100 * sizeof(char));

    printf("Enter Title: ");
    fgets(user_book.title, 100, stdin);
    user_book.title[strcspn(user_book.title, "\n")] = '\0';

    printf("Enter Author: ");
    fgets(user_book.author, 100, stdin);
    user_book.author[strcspn(user_book.author, "\n")] = '\0';

    printf("Enter Stock: ");
    scanf("%d", &user_book.stock);
    getchar();

    save_book(library, &user_book);

    puts("Book Added Successfully.");

    free(user_book.title);
    free(user_book.author);
}

void delete_book(Library *library)
{
}

void pop(Library *library)
{
    if (library->size > 0)
    {
        library->size--;
        free(library->data[library->size].title);
        free(library->data[library->size].author);

        if (library->size < library->capacity / 4)
        {
            library->capacity /= 2;
            library->data = realloc(library->data, library->capacity * sizeof(Book));
        }
    }

    save_file(library);
}

char *search(Library *library, char *search_term)
{
    for (int i = 0; i < library->size; i++)
    {
        if (strcmp(library->data[i].title, search_term) == 0)
        {
            static char result[200];
            snprintf(result, sizeof(result), "Found: Title: %s, Author: %s, Stock: %d",
                     library->data[i].title, library->data[i].author, library->data[i].stock);
            return result;
        }
    }
    return "No results match criteria.";
}

void free_library(Library *library)
{
    for (int i = 0; i < library->size; i++)
    {
        free(library->data[i].title);
        free(library->data[i].author);
    }
    free(library->data);
    library->data = NULL;
    library->size = 0;
    library->capacity = 0;
}

void view_books()
{
    FILE *fptr = fopen("./library.txt", "r");
    if (!fptr)
    {
        perror("Error opening file");
        return;
    }

    char buff[100];
    int i = 1;

    puts("\n\n************************************************");
    puts("*                                              *");
    puts("*      List of Books in the Library          *");
    puts("*                                              *");
    puts("************************************************");
    puts("*                                              *");

    while (fgets(buff, sizeof(buff), fptr) != NULL)
    {
        size_t len = strlen(buff);
        buff[len - 1] = '\0';
        printf("* %2d. %-40s *\n", i, buff);
        i++;
    }
    puts("*                                              *");
    puts("************************************************\n\n");

    fclose(fptr);
}

void save_file(Library *library)
{
    FILE *ptr = fopen("./library.txt", "w");
    if (!ptr)
    {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < library->size; i++)
    {
        fprintf(ptr, "Title: %s, Author: %s, Stock: %d\n",
                library->data[i].title, library->data[i].author, library->data[i].stock);
    }

    fclose(ptr);
}

void parse_line(const char *line, char *title, char *author, int *stock)
{
    sscanf(line, "Title: %[^,], Author: %[^,], Stock: %d", title, author, stock);
}

void save_book(Library *library, Book *book)
{
    if (library->size >= library->capacity)
    {
        library->capacity *= 2;
        library->data = realloc(library->data, library->capacity * sizeof(Book));
    }

    library->data[library->size].title = strdup(book->title);
    library->data[library->size].author = strdup(book->author);
    library->data[library->size].stock = book->stock;
    library->size++;

    save_file(library);
}
