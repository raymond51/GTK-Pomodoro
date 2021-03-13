#include "counter_interface.h"

/*Static function prototypes*/
static bool equal_today_date(struct CounterUI *counterUI, FILE *fPointer_ptr);
static void init_curr_date_counter(struct CounterUI *counterUI);
static bool file_append_new_date_entry(struct CounterUI *counterUI, FILE *fPointer_ptr);
static void update_daily_counter(struct CounterUI *counterUI);

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

static void init_curr_date_counter(struct CounterUI *counterUI)
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

/*Open file in read mode and update counterUI struct with today date*/
static bool equal_today_date(struct CounterUI *counterUI, FILE *fPointer_ptr)
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

static bool file_append_new_date_entry(struct CounterUI *counterUI, FILE *fPointer_ptr)
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

void counter_up_btn_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct CounterUI *counterUI_ptr = data;
    counterUI_ptr->increment_counter = true;
    update_daily_counter(counterUI_ptr);
}

void counter_down_btn_clicked(GtkWidget *widget, gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct CounterUI *counterUI_ptr = data;
    counterUI_ptr->increment_counter = false;
    update_daily_counter(counterUI_ptr);
}

static void update_daily_counter(struct CounterUI *counterUI)
{
    char str[TWEPOWEIGHT];
    counterUI->curr_counter = (counterUI->increment_counter == true) ? (counterUI->curr_counter + 1) : (counterUI->curr_counter - 1);
    if (counterUI->curr_counter < 0)
        counterUI->curr_counter = 0;
    sprintf(str, "%d", counterUI->curr_counter);
    gtk_label_set_text(counterUI->counter_label, str);
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