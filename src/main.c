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
#include "timer_interface.h"
#include "helper.h"

/*DEFINES*/
#define CLOSE_ON_FILE_ERROR

#define COUNTER 2

#define GLADE_FILE_LOC "/GTK-Pomodoro/glade/mainUI.glade"
#define RECORD_FILE_LOC "GTK-Pomodoro/res/record.txt"
#define NEW_RECORD_FILE_LOC "GTK-Pomodoro/res/new_record.txt"

/*Defines for button ID*/
struct CounterUI
{
    GtkLabel *counter_label;
    GtkButton *counter_up_btn;
    GtkButton *counter_down_btn;

    uint8_t Type;
    gint day_today, month_today, year_today;
    gint day_Of_Week;
    int curr_counter;
    bool increment_counter;
    FILE *fPointer;
    int final_line_count;
    bool record_write_enable;
};

/*Function Prototypes*/

static void counter_up_btn_clicked(GtkWidget *widget, gpointer data);
static void counter_down_btn_clicked(GtkWidget *widget, gpointer data);
static void message_dialog(char *primary_msg, char *secondary_msg);

bool init_tracking_counter(GtkBuilder *builder, struct CounterUI *counterUI, FILE *fPointer_ptr);
bool equal_today_date(struct CounterUI *counterUI, FILE *fPointer_ptr);
void init_curr_date_counter(struct CounterUI *counterUI);
bool file_append_new_date_entry(struct CounterUI *counterUI, FILE *fPointer_ptr);

void delete_allocation(struct TimerUI *ptr, gpointer data);
void update_record_file(struct CounterUI *ptr, gpointer data);
void delete_allocation_counter(struct CounterUI *ptr, gpointer data);
void delete_file_path_allocation(char *file_path);

void update_daily_counter(struct CounterUI *counterUI);

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

bool init_tracking_counter(GtkBuilder *builder, struct CounterUI *counterUI, FILE *fPointer_ptr)
{
    bool status_flag = true;
    counterUI->Type = COUNTER;
    counterUI->counter_label = GTK_LABEL(gtk_builder_get_object(builder, "dailyCounter"));
    counterUI->counter_up_btn = GTK_BUTTON(gtk_builder_get_object(builder, "counter_up_btn"));
    counterUI->counter_down_btn = GTK_BUTTON(gtk_builder_get_object(builder, "counter_down_btn"));
    g_signal_connect(counterUI->counter_up_btn, "clicked", G_CALLBACK(counter_up_btn_clicked), counterUI);
    g_signal_connect(counterUI->counter_down_btn, "clicked", G_CALLBACK(counter_down_btn_clicked), counterUI);

    /*check if file accessible*/
    fPointer_ptr = fopen(RECORD_FILE_LOC, "r");

    if (fPointer_ptr)
    {
        fclose(fPointer_ptr);
        /*Enable creating new record file if all safety checks cleared*/
        counterUI->record_write_enable = true;
        if (!equal_today_date(counterUI, fPointer_ptr))
        {
            status_flag = file_append_new_date_entry(counterUI, fPointer_ptr);
        }
    }
    else if (fPointer_ptr == NULL)
    {
        status_flag = false;
    }

    return status_flag;
}

/*Open file in read mode and update counterUI struct with today date*/
bool equal_today_date(struct CounterUI *counterUI, FILE *fPointer_ptr)
{
    bool equal_date_flag = false;
    char line[ONE_KB] = {0};
    char str[TWEPOWEIGHT];

    gint day, month, year;
    gint day_of_week_temp;
    int counter_temp;

    init_curr_date_counter(counterUI);
    counterUI->final_line_count = 0;

    //OPEN FFILE AS READ AND FORMAT GRAB
    fPointer_ptr = fopen(RECORD_FILE_LOC, "r");

    while (fgets(line, ONE_KB, fPointer_ptr) != NULL)
    {
        // Just search for the latest line, do nothing in the loop
        counterUI->final_line_count++;
    }
#ifdef DEBUG_PRINT
    printf("Last line in record.txt: %s and Line count: %d\n", line, counterUI->final_line_count);
#endif
    /*Check if file is not empty*/
    if (strlen(line) > 0)
    {
        sscanf(line, "%d-%d-%d,%d,%d", &day, &month, &year, &day_of_week_temp, &counter_temp);
        /*Compare record with today date*/
        if ((day == counterUI->day_today) && (month == counterUI->month_today) && (year == counterUI->year_today))
        {
            equal_date_flag = true;
            /*update and assign read values to struct*/
            counterUI->day_Of_Week = day_of_week_temp;
            counterUI->curr_counter = counter_temp;
        }
    }
    /*update counter label*/
    sprintf(str, "%d", counterUI->curr_counter);
    gtk_label_set_text(counterUI->counter_label, str);

    fclose(fPointer_ptr);

    return equal_date_flag;
}

void init_curr_date_counter(struct CounterUI *counterUI)
{
    /*GLIB - GDateTime*/
    GDateTime *date_time;
    date_time = g_date_time_new_now_local(); // get local time
    counterUI->day_today = g_date_time_get_day_of_month(date_time);
    counterUI->month_today = g_date_time_get_month(date_time);
    counterUI->year_today = g_date_time_get_year(date_time);
    counterUI->day_Of_Week = g_date_time_get_day_of_week(date_time);
    counterUI->curr_counter = COUNTER_INIT;
}

bool file_append_new_date_entry(struct CounterUI *counterUI, FILE *fPointer_ptr)
{
    bool status_flag = true;
    char buffer[TWEPOWEIGHT];

    /*Append new date entry and init counter to 0*/
    fPointer_ptr = fopen(RECORD_FILE_LOC, "a");
    if (fPointer_ptr != NULL)
    {
        snprintf(buffer, TWEPOWEIGHT, "%d-%d-%d,%d,%d\n", counterUI->day_today, counterUI->month_today, counterUI->year_today, counterUI->day_Of_Week, COUNTER_INIT);
        fprintf(fPointer_ptr, "%s", buffer);
        fflush(fPointer_ptr);
#ifdef DEBUG_PRINT
        printf("Successfully added new Date format entry: %d-%d-%d\n", counterUI->day_today, counterUI->month_today, counterUI->year_today);
#endif
        fclose(fPointer_ptr);
    }
    else
    {
        status_flag = false;
    }

    return status_flag;
}

static void
counter_up_btn_clicked(GtkWidget *widget,
                       gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct CounterUI *counterUI_ptr = data;
    counterUI_ptr->increment_counter = true;
    update_daily_counter(counterUI_ptr);
}

static void
counter_down_btn_clicked(GtkWidget *widget,
                         gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct CounterUI *counterUI_ptr = data;
    counterUI_ptr->increment_counter = false;
    update_daily_counter(counterUI_ptr);
}

void update_daily_counter(struct CounterUI *counterUI)
{
    char str[TWEPOWEIGHT];
    counterUI->curr_counter = (counterUI->increment_counter == true) ? (counterUI->curr_counter + 1) : (counterUI->curr_counter - 1);
    if (counterUI->curr_counter < 0)
        counterUI->curr_counter = 0;
    sprintf(str, "%d", counterUI->curr_counter);
    gtk_label_set_text(counterUI->counter_label, str);
}

static void message_dialog(char *primary_msg, char *secondary_msg)
{
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, primary_msg);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(message), secondary_msg, 99);
    gtk_dialog_run(GTK_DIALOG(message));
    gtk_widget_destroy(message);
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

void update_record_file(struct CounterUI *counterUI_ptr, gpointer data)
{
    (void)data; //To get rid of compiler warning
    FILE *new_record;
    char str[TWEPOWEIGHT];
    int line_counter = 0;

    if (counterUI_ptr->record_write_enable == true)
    {
        new_record = fopen(NEW_RECORD_FILE_LOC, "w");
        counterUI_ptr->fPointer = fopen(RECORD_FILE_LOC, "r");
        if (new_record && counterUI_ptr->fPointer)
        {
            while (!feof(counterUI_ptr->fPointer))
            {
                strcpy(str, "\0");
                fgets(str, TWEPOWEIGHT, counterUI_ptr->fPointer);
                if (!feof(counterUI_ptr->fPointer))
                {
                    line_counter++;
                    //ignore copying of last line
                    if (line_counter != counterUI_ptr->final_line_count)
                    {
                        fprintf(new_record, "%s", str);
                    }
                }
            }

            fclose(counterUI_ptr->fPointer);
            fclose(new_record);

            /*Append new data*/
            new_record = fopen(NEW_RECORD_FILE_LOC, "a");
            if (new_record != NULL)
            {
                snprintf(str, TWEPOWEIGHT, "%d-%d-%d,%d,%d\n", counterUI_ptr->day_today, counterUI_ptr->month_today, counterUI_ptr->year_today, counterUI_ptr->day_Of_Week, counterUI_ptr->curr_counter);
                fprintf(new_record, "%s", str);
                fflush(new_record);
                fclose(new_record);

                remove(RECORD_FILE_LOC);
                rename(NEW_RECORD_FILE_LOC, RECORD_FILE_LOC);
#ifdef DEBUG_PRINT
                g_print("Updated record file!\n");
#endif
            }
            else
            {
                message_dialog("Update Error!", "Cannot append to new record file to save daily counter");
            }
        }
        else if (new_record == NULL || counterUI_ptr->fPointer == NULL)
        {
            message_dialog("Update Error!", "Cannot load record file to save daily counter");
#ifdef DEBUG_PRINT
            g_print("Cannot open new record file to write to!\n");
#endif
        }
    }
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
