#ifndef COUNTER_INTERFACE_H
#define COUNTER_INTERFACE_H

/*includes*/
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include "helper.h"

/*Defines*/
#define COUNTER 2
#define COUNTER_INIT 0
#define RECORD_FILE_LOC "GTK-Pomodoro/res/record.txt"
#define NEW_RECORD_FILE_LOC "GTK-Pomodoro/res/new_record.txt"

/*structure*/
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

/*Function prototype*/
bool init_tracking_counter(GtkBuilder *builder, struct CounterUI *counterUI, FILE *fPointer_ptr);
bool equal_today_date(struct CounterUI *counterUI, FILE *fPointer_ptr);
void counter_up_btn_clicked(GtkWidget *widget, gpointer data);
void counter_down_btn_clicked(GtkWidget *widget, gpointer data);
void init_curr_date_counter(struct CounterUI *counterUI);
bool file_append_new_date_entry(struct CounterUI *counterUI, FILE *fPointer_ptr);
void update_daily_counter(struct CounterUI *counterUI);
void update_record_file(struct CounterUI *ptr, gpointer data);

#endif //COUNTER_INTERFACE_H