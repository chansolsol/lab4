#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    GtkWidget *entry;
    char operator;
    double operand;
} Calculator;

void calculate(Calculator *calc) {
    const char *text = gtk_entry_get_text(GTK_ENTRY(calc->entry));
    double value = atof(text);

    switch (calc->operator) {
        case '+': 
            value = calc->operand + value; 
            break;
        case '-': 
            value = calc->operand - value; 
            break;
        case '*': 
            value = calc->operand * value; 
            break;
        case '/':
            if (value != 0) {
                value = calc->operand / value;
            } else {
                // Handle division by zero error
                GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(calc->entry),
                                                           GTK_DIALOG_MODAL,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Error: Division by zero");
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
                value = calc->operand; // Keep previous value
            }
            break;
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%g", value);
    gtk_entry_set_text(GTK_ENTRY(calc->entry), buffer);
}

void on_button_clicked(GtkWidget *widget, gpointer data) {
    Calculator *calc = (Calculator *)data;
    const char *label = gtk_button_get_label(GTK_BUTTON(widget));
    const char *current_text = gtk_entry_get_text(GTK_ENTRY(calc->entry));

    if (strcmp(label, "=") == 0) {
        calculate(calc);
        calc->operator = '\0';
    } else if (strchr("+-*/", label[0]) != NULL) {
        calc->operator = label[0];
        calc->operand = atof(current_text);
        gtk_entry_set_text(GTK_ENTRY(calc->entry), "");
    } else {
        char *new_text = g_strdup_printf("%s%s", current_text, label);
        gtk_entry_set_text(GTK_ENTRY(calc->entry), new_text);
        g_free(new_text);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *grid, *button;
    Calculator calc = { NULL, '\0', 0 };

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    calc.entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), calc.entry, 0, 0, 4, 1);

    const char *buttons[] = {"7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", "0", "=", ".", "+"};

    for (int i = 0; i < 16; ++i) {
        button = gtk_button_new_with_label(buttons[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), &calc);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, 1 + i / 4, 1, 1);
    }

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
