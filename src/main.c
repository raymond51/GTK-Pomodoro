#include <gtk/gtk.h>
#include <stdbool.h>

/*DEFINES*/
#define WORK 0
#define REST 1
/*Defines for button ID*/
#define WORK_PLAY_PAUSE_BTN 0
#define WORK_RESET_BTN 1
#define REST_PLAY_PAUSE_BTN 2
#define REST_RESET_BTN 3

/*Structure*/
struct TimerUI
{

    GtkLabel *timeKeeper_label;
    GtkButton *play_pause_button;
    GtkButton *reset_button;
    GtkProgressBar *pbar;

    uint8_t timerType;
    int minutes;
    int seconds;
};

/*Function Prototypes*/
static void button_clicked(GtkWidget *widget, gpointer data);
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
    gdouble pbar_init = 0.0;
    static uint8_t work_pp_trig = WORK_PLAY_PAUSE_BTN;
    static uint8_t work_reset_trig = WORK_RESET_BTN;
    static uint8_t rest_pp_trig = REST_PLAY_PAUSE_BTN;
    static uint8_t rest_reset_trig = REST_RESET_BTN;

    switch (timerType_data)
    {
    case WORK:
        timerUi->timerType = WORK;
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_working"));
        /*SET INITIAL VAL OF WORKING TIMER LABEL*/
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_working"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(button_clicked), &work_pp_trig); //callback function upon clicked action, PARAM (final): pass address of data
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_working"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(button_clicked), &work_reset_trig);
        timerUi->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pbar_working"));
        gtk_progress_bar_set_fraction(timerUi->pbar, pbar_init);
        status_flag = reset_timer(timerUi) ? true : false;
        break;
    case REST:
        timerUi->timerType = REST;
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_resting"));
        /*SET INITIAL VAL OF WORKING TIMER LABEL*/
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_resting"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(button_clicked), &rest_pp_trig);
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_resting"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(button_clicked), &rest_reset_trig);
        timerUi->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pbar_resting"));
        gtk_progress_bar_set_fraction(timerUi->pbar, pbar_init);
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
    return status_flag;
}

/*sends activate signal upon function call g_application_run(), behave like virtual function that application responds to*/
static void
button_clicked(GtkWidget *widget,
               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    g_print("Hello World, Value of btn: %d \n", *(uint8_t *)data);
    //gtk_label_set_text(myLabel, "Helloooooooooo!");
}

//func to init display: set pbar to 0
//func to link res in code i.e g resource
