//* Info -----------
// Author: Raymond
// Version: -

//This file is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 3, or (at your option) any later version.

//For a full copy of the GNU General Public License
//view <http://www.gnu.org/licenses/>.
//--------------------

//includes
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "helper.h"
#include "timer_interface.h"
#include "counter_interface.h"

/*DEFINES*/
#define CLOSE_ON_FILE_ERROR

/*Function Prototypes*/

void delete_allocation(struct TimerUI *ptr, gpointer data);
void delete_allocation_counter(struct CounterUI *ptr, gpointer data);
void delete_file_path_allocation(char *file_path);

int main(int argc,
         char *argv[])
{
    /*Variables*/
    GtkWidget *window; //GTK window
    GtkBuilder *builder;
    char *file_path = malloc(2 * ONE_KB);
    GError *error = NULL;

    //alocation heap
    struct TimerUI *work_TimerUI = malloc(sizeof *work_TimerUI), *rest_TimerUI = malloc(sizeof *rest_TimerUI);
    struct CounterUI *counterUI_ptr = malloc(sizeof *counterUI_ptr);

    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder, prg_path(file_path, GLADE_FILE_LOC), &error) == 0)
    {
#ifdef DEBUG_PRINT
        g_printerr("Error loading glade file: %s\n", error->message);
#endif
        message_dialog("Initialisation Error!", "Error loading glade file");
        g_clear_error(&error);
        counterUI_ptr->record_write_enable = false;
    }

    if (!init_timer_interface(builder, work_TimerUI, file_path, WORK) || !init_timer_interface(builder, rest_TimerUI, file_path, REST))
    {
#ifdef DEBUG_PRINT
        g_printerr("Error loading init_timer_interface()\n");
#endif
        message_dialog("Initialisation Error!", "Error loading timer interface");
        counterUI_ptr->record_write_enable = false;
    }

    if (!init_tracking_counter(builder, counterUI_ptr, counterUI_ptr->fPointer))
    {
#ifdef DEBUG_PRINT
        perror("Error opening file record.txt: ");
#endif
        message_dialog("Initialisation Error!", "Error reading record file");
        counterUI_ptr->record_write_enable = false;
    }

    /* Connect signal handlers to the constructed widgets. */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    gtk_window_set_title(GTK_WINDOW(window), "Pomodoroooo!"); //Set title of program

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //callback func to destroy window upon exit
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation), work_TimerUI);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation), rest_TimerUI);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(update_record_file), counterUI_ptr);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation_counter), counterUI_ptr);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_file_path_allocation), file_path);
    gtk_builder_connect_signals(builder, NULL);

    if (!counterUI_ptr->record_write_enable)
    {
        gtk_widget_destroy(window);
        return 1;
    }
    else
        gtk_widget_show_all(window);
    gtk_main();              //run program loop
    g_object_unref(builder); //free object in memory

    return 0;
}

/*Free malloc()*/
void delete_allocation(struct TimerUI *ptr, gpointer data)
{
    (void)data; //To get rid of compiler warning
#ifdef DEBUG_PRINT
    g_print("Memory allocation freed %u! \n", ptr->Type);
#endif
    g_free(ptr);
}

void delete_allocation_counter(struct CounterUI *ptr, gpointer data)
{
    (void)data; //To get rid of compiler warning
#ifdef DEBUG_PRINT
    g_print("Memory allocation freed %u! \n", ptr->Type);
#endif
    g_free(ptr);
}

void delete_file_path_allocation(char *file_path)
{
    free(file_path);
#ifdef DEBUG_PRINT
    g_print("Memory allocation for file path pointer freed! \n");
#endif
}
