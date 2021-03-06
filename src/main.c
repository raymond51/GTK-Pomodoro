#include <gtk/gtk.h>
#include <stdbool.h>

/*DEFINES*/
#define WORK 0
#define REST 1
#define MINUTE_SECONDS 60
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
    unsigned int minutes;
    unsigned int seconds;
};

/*Function Prototypes*/
static void working_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
static void working_reset_btn_clicked(GtkWidget *widget, gpointer data);
static void resting_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
static void resting_reset_btn_clicked(GtkWidget *widget, gpointer data);
bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, uint8_t timerType); //return true if success
bool reset_timer(struct TimerUI *timerUi);

int main(int argc,
         char *argv[])
{
    /*Variables*/
    GtkWidget *window; //GTK window
    GtkBuilder *builder;
    GError *error = NULL;

    struct TimerUI work_TimerUI, rest_TimerUI;

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

    if (!init_timer_interface(builder, &work_TimerUI, WORK) || !init_timer_interface(builder, &rest_TimerUI, REST))
    {
        g_printerr("Error loading init_timer_interface()\n");
        return 1;
    }

    /*INIT DAILY COUNTER FUNC()*/

    /* Connect signal handlers to the constructed widgets. */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));   //PARAM: , widget ID
    gtk_window_set_title(GTK_WINDOW(window), "Pomodoroooo!");             //Set title of program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //callback func to destroy window upon exit

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
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(working_play_pause_btn_clicked), &timerUi); //callback function upon clicked action, PARAM (final): pass address of data (single static struct variable)
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_working"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(working_reset_btn_clicked), &timerUi);
        timerUi->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pbar_working"));
        status_flag = reset_timer(timerUi) ? true : false;
        break;
    case REST:
        timerUi->timerType = REST;
        timerUi->is_playing = false;
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_resting"));
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_resting"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(resting_play_pause_btn_clicked), &timerUi);
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_resting"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(resting_reset_btn_clicked), &timerUi);
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
    switch (timerUi->timerType)
    {
    case WORK:
        timerUi->minutes = 25;
        timerUi->seconds = 0;
        gtk_label_set_text(timerUi->timeKeeper_label, "25:00");

        break;
    case REST:
        timerUi->minutes = 5;
        timerUi->seconds = 0;
        gtk_label_set_text(timerUi->timeKeeper_label, "5:00");
        break;
    default:
        status_flag = false;
    }
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
    g_print("Value of timerType: %d \n", timerUI_ptr->timerType);
}

static void
working_reset_btn_clicked(GtkWidget *widget,
                          gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    g_print("Value of timerType: %d \n", timerUI_ptr->timerType);
}

static void
resting_play_pause_btn_clicked(GtkWidget *widget,
                               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    g_print("Value of timerTypet: %d \n", timerUI_ptr->timerType);
}

static void
resting_reset_btn_clicked(GtkWidget *widget,
                          gpointer data)
{
    (void)widget; //To get rid of compiler warning
    struct TimerUI *timerUI_ptr = data;
    g_print("Value of timerType: %d \n", timerUI_ptr->timerType);
}

//func to link res in code i.e g resource
