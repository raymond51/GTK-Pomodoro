#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>

/*DEFINES*/
#define WORK 0
#define REST 1
#define TIME_FORMAT_STRING_LEN 6
#define MINUTE_SECONDS 59
#define WORKING_INIT_TIME_MINS 1
#define WORKING_INIT_TIME_SECS 0
#define RESTING_INIT_TIME_MINS 5
#define RESTING_INIT_TIME_SECS 0
/*Defines for button ID*/
typedef enum
{
    WORK_PLAY_PAUSE_BTN = 0,
    WORK_RESET_BTN,
    REST_PLAY_PAUSE_BTN,
    REST_RESET_BTN
} button_id_Enum;

/*Structure*/
struct TimerUI
{

    GtkLabel *timeKeeper_label;
    GtkButton *play_pause_button;
    GtkButton *reset_button;
    GtkProgressBar *pbar;

    uint8_t timerType;
    bool is_playing;
    guint timer_tag;
    int minutes;
    int seconds;
};

/*Function Prototypes*/
static void working_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
static void working_reset_btn_clicked(GtkWidget *widget, gpointer data);
static void resting_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
static void resting_reset_btn_clicked(GtkWidget *widget, gpointer data);
bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, uint8_t timerType); //return true if success
bool reset_timer(struct TimerUI *timerUi);
void delete_allocation(struct TimerUI *ptr, gpointer data);
gboolean working_timer_handler(struct TimerUI *timerUi);

int main(int argc,
         char *argv[])
{
    /*Variables*/
    GtkWidget *window; //GTK window
    GtkBuilder *builder;
    GError *error = NULL;

    //alocation heap
    struct TimerUI *work_TimerUI = malloc(sizeof *work_TimerUI), *rest_TimerUI = malloc(sizeof *rest_TimerUI);

    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new();
    //using absolute path for now
    if (gtk_builder_add_from_file(builder, "C:/Users/raymo/Documents/VS-code/C projects/GTK/GTK-pomodoro/glade/mainUI.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    if (!init_timer_interface(builder, work_TimerUI, WORK) || !init_timer_interface(builder, rest_TimerUI, REST))
    {
        g_printerr("Error loading init_timer_interface()\n");
        return 1;
    }

    /*INIT DAILY COUNTER FUNC()*/

    /* Connect signal handlers to the constructed widgets. */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));   //PARAM: , widget ID
    gtk_window_set_title(GTK_WINDOW(window), "Pomodoroooo!");             //Set title of program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //callback func to destroy window upon exit
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation), work_TimerUI);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation), rest_TimerUI);

    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show_all(window);
    gtk_main();              //run program loop
    g_object_unref(builder); //free object in memory

    return 0;
}

bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, uint8_t timerType_data)
{
    bool status_flag = true;

    switch (timerType_data)
    {
    case WORK:
        timerUi->timerType = WORK;
        timerUi->is_playing = false; //start initial at pause state
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_working"));
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_working"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(working_play_pause_btn_clicked), timerUi); //callback function upon clicked action, PARAM (final): pass address of data (single static struct variable)
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_working"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(working_reset_btn_clicked), timerUi);
        timerUi->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pbar_working"));
        status_flag = reset_timer(timerUi) ? true : false;
        break;
    case REST:
        timerUi->timerType = REST;
        timerUi->is_playing = false;
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_resting"));
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_resting"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(resting_play_pause_btn_clicked), timerUi);
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_resting"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(resting_reset_btn_clicked), timerUi);
        timerUi->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pbar_resting"));
        status_flag = reset_timer(timerUi) ? true : false;
        break;
    default:
        status_flag = false;
    }
    return status_flag;
}

bool reset_timer(struct TimerUI *timerUi)
{
    bool status_flag = true;
    gdouble pbar_init = 0.0;
    char formattedTime[TIME_FORMAT_STRING_LEN];

    switch (timerUi->timerType)
    {
    case WORK:
        timerUi->minutes = WORKING_INIT_TIME_MINS;
        timerUi->seconds = WORKING_INIT_TIME_SECS;

        break;
    case REST:
        timerUi->minutes = RESTING_INIT_TIME_MINS;
        timerUi->seconds = RESTING_INIT_TIME_SECS;

        break;
    default:
        status_flag = false;
    }
    snprintf(formattedTime, TIME_FORMAT_STRING_LEN, "%d:%2.2d", timerUi->minutes, timerUi->seconds);
    gtk_label_set_text(timerUi->timeKeeper_label, formattedTime);
    gtk_progress_bar_set_fraction(timerUi->pbar, pbar_init);
    return status_flag;
}

/*sends activate signal upon function call g_application_run(), behave like virtual function that application responds to*/
static void
working_play_pause_btn_clicked(GtkWidget *widget,
                               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    if (timerUI_ptr->is_playing != true)
    {
        timerUI_ptr->is_playing = true;
        //init tick handler (1 second timer)
        timerUI_ptr->timer_tag = g_timeout_add_seconds(1, (GSourceFunc)working_timer_handler, timerUI_ptr); //store tag to destroy timeout()
        //update image icon
        g_print("Value of timerType enabeld?: %d & timer tag: %d\n", timerUI_ptr->is_playing, timerUI_ptr->timer_tag);
    }
    else
    {
        timerUI_ptr->is_playing = false;
        g_source_remove(timerUI_ptr->timer_tag);
        //update image icon
        g_print("Value of timerType enabeld?: %d\n", timerUI_ptr->is_playing);
    }
}

static void
working_reset_btn_clicked(GtkWidget *widget,
                          gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    g_print("Value of timerType: %s \n", gtk_label_get_text(timerUI_ptr->timeKeeper_label));
}

static void
resting_play_pause_btn_clicked(GtkWidget *widget,
                               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    g_print("Value of timerTypet: %s \n", gtk_label_get_text(timerUI_ptr->timeKeeper_label));
}

static void
resting_reset_btn_clicked(GtkWidget *widget,
                          gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    g_print("Value of timerType: %s \n", gtk_label_get_text(timerUI_ptr->timeKeeper_label));
}

/*Free malloc()*/
void delete_allocation(struct TimerUI *ptr, gpointer data)
{
    (void)data; //To get rid of compiler warning
    g_print("Memory allocation freed %u! \n", ptr->timerType);
    g_free(ptr);
}

// handler for the 1 second timer tick
gboolean working_timer_handler(struct TimerUI *timerUi)
{
    char formattedTime[TIME_FORMAT_STRING_LEN];
    timerUi->seconds--;
    if (timerUi->minutes > 0 && timerUi->seconds < 0)
    {
        timerUi->minutes--;
        timerUi->seconds = MINUTE_SECONDS;
        //UPDATE PBAR
    }
    else if (timerUi->minutes <= 0 && timerUi->seconds < 0)
    {
        //PLAY TIMER UP SOUND
        g_source_remove(timerUi->timer_tag);
        timerUi->is_playing = false;
        //UPDATE ICON
        timerUi->minutes = 0;
        timerUi->seconds = 0;
    }
    snprintf(formattedTime, TIME_FORMAT_STRING_LEN, "%d:%2.2d", timerUi->minutes, timerUi->seconds);
    gtk_label_set_text(timerUi->timeKeeper_label, formattedTime);
    return TRUE;
}
//func to link res in code i.e g resource
