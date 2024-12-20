#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 255

typedef struct
{
    char title[MAX_LINE_LENGTH];
    char author[MAX_LINE_LENGTH];
} Book;

typedef struct
{
    GtkBuilder *builder;
    GtkWidget *search_entry;
    GtkWidget *book_list;
    GtkWidget *add_book_button;
    GtkWidget *add_book_dialog;
    GtkWidget *title_entry;
    GtkWidget *author_entry;
} SearchData;

static void display_books(GtkBuilder *builder, GtkWidget *book_list, const char *query);

static Book *selected_book = NULL;

static void on_search_clicked(GtkWidget *button, gpointer user_data)
{
    SearchData *data = (SearchData *)user_data;

    GtkBuilder *builder = data->builder;
    GtkWidget *search_entry = data->search_entry;
    GtkWidget *book_list = data->book_list;

    const char *query = gtk_editable_get_text(GTK_EDITABLE(search_entry));
    g_print("Search query: %s\n", query);

    display_books(builder, book_list, query);
}

static void display_books(GtkBuilder *builder, GtkWidget *book_list, const char *query)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char title[MAX_LINE_LENGTH], author[MAX_LINE_LENGTH];

    gtk_list_box_remove_all(GTK_LIST_BOX(book_list));

    file = fopen("books.txt", "r");
    if (file == NULL)
    {
        g_print("Error: Could not open books.txt\n");
        g_object_unref(builder);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%255[^-] - %255[^\n]", title, author);
        line[strcspn(line, "\n")] = '\0';

        if (query == NULL || strstr(line, query) != NULL)
        {
            Book *book = g_new(Book, 1);
            strncpy(book->title, title, sizeof(book->title));
            strncpy(book->author, author, sizeof(book->author));
            GtkWidget *row = gtk_list_box_row_new();

            g_object_set_data(G_OBJECT(row), "book", book);

            GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            GtkWidget *label = gtk_label_new(line);

            gtk_box_append(GTK_BOX(box), label);

            gtk_list_box_insert(GTK_LIST_BOX(book_list), row, -1);
            gtk_widget_set_parent(box, row);
        }
    }

    fclose(file);
}

static void on_add_book_clicked(GtkWidget *button, gpointer user_data)
{
    SearchData *data = (SearchData *)user_data;
    gtk_window_present(GTK_WINDOW(data->add_book_dialog));
}

static void on_add_book_confirmed(GtkWidget *button, gpointer user_data)
{
    SearchData *data = (SearchData *)user_data;

    GtkBuilder *builder = data->builder;
    GtkWidget *book_list = data->book_list;

    const char *title = gtk_editable_get_text(GTK_EDITABLE(data->title_entry));
    const char *author = gtk_editable_get_text(GTK_EDITABLE(data->author_entry));

    g_print("Title: %s\n", title);
    g_print("Author: %s\n", author);

    FILE *file = fopen("books.txt", "a");
    if (file == NULL)
    {
        g_print("Error: Could not open books.txt for writing.\n");
        return;
    }

    fprintf(file, "%s - %s\n", title, author);
    fclose(file);

    g_print("New book added to books.txt: %s - %s\n", title, author);

    gtk_editable_set_text(GTK_EDITABLE(data->title_entry), "");
    gtk_editable_set_text(GTK_EDITABLE(data->author_entry), "");

    display_books(builder, book_list, NULL);
}

static void on_delete_clicked(GtkWidget *button, gpointer user_data)
{
    FILE *file, *temp_file;
    char line[MAX_LINE_LENGTH];
    char title[MAX_LINE_LENGTH], author[MAX_LINE_LENGTH];

    file = fopen("books.txt", "r");
    if (file == NULL)
    {
        g_print("Error: Could not open books.txt for reading.\n");
        return;
    }

    temp_file = fopen("temp_books.txt", "w");
    if (temp_file == NULL)
    {
        g_print("Error: Could not open temp_books.txt for writing.\n");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%255[^-] - %255[^\n]", title, author);

        if (strcmp(title, selected_book->title) != 0 || strcmp(author, selected_book->author) != 0)
        {
            fputs(line, temp_file);
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("books.txt");
    rename("temp_books.txt", "books.txt");

    g_print("Deleted book: %s by %s\n", selected_book->title, selected_book->author);

    display_books(user_data, GTK_WIDGET(user_data), NULL);
}

static void on_cancel_clicked(GtkWidget *button, gpointer user_data)
{
    SearchData *data = (SearchData *)user_data;

    gtk_window_close(GTK_WINDOW(data->add_book_dialog));
}

static void on_exit_clicked(GtkWidget *button, gpointer user_data)
{
    GtkApplication *app = GTK_APPLICATION(user_data);

    g_application_quit(G_APPLICATION(app));
}

static void on_row_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data)
{
    if (row != NULL)
    {
        selected_book = (Book *)g_object_get_data(G_OBJECT(row), "book");

        if (selected_book != NULL)
        {
            g_print("Book selected: %s by %s\n", selected_book->title, selected_book->author);
        }

        GtkWidget *delete_button = GTK_WIDGET(user_data);
        gtk_widget_set_sensitive(delete_button, TRUE);
    }
}

static void on_activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window, *book_list, *scrolled_window, *search_entry, *search_button;
    SearchData *data = g_new(SearchData, 1);

    builder = gtk_builder_new_from_file("ui.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    book_list = GTK_WIDGET(gtk_builder_get_object(builder, "book_list"));
    scrolled_window = GTK_WIDGET(gtk_builder_get_object(builder, "scrolled_window"));
    search_entry = GTK_WIDGET(gtk_builder_get_object(builder, "search_entry"));

    search_button = GTK_WIDGET(gtk_builder_get_object(builder, "search_button"));

    data->builder = builder;
    data->search_entry = search_entry;
    data->book_list = book_list;
    data->add_book_button = GTK_WIDGET(gtk_builder_get_object(builder, "add_book_button"));
    data->add_book_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "add_book_dialog"));
    data->title_entry = GTK_WIDGET(gtk_builder_get_object(builder, "title_entry"));
    data->author_entry = GTK_WIDGET(gtk_builder_get_object(builder, "author_entry"));

    GtkWidget *add_button = GTK_WIDGET(gtk_builder_get_object(builder, "add_button"));
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_book_confirmed), data);

    GtkWidget *delete_button = GTK_WIDGET(gtk_builder_get_object(builder, "remove_book_button"));

    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_button"));
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_clicked), data);

    GtkWidget *exit_button = GTK_WIDGET(gtk_builder_get_object(builder, "exit_button"));
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_exit_clicked), app);

    gtk_widget_set_size_request(scrolled_window, 500, 400);

    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_clicked), data);
    g_signal_connect(data->add_book_button, "clicked", G_CALLBACK(on_add_book_clicked), data);

    display_books(builder, book_list, NULL);

    gtk_widget_set_sensitive(delete_button, FALSE);

    g_signal_connect(book_list, "row-selected", G_CALLBACK(on_row_selected), delete_button);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_clicked), book_list);

    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_present(GTK_WINDOW(window));

    g_object_unref(builder);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.library", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
