#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

/*DEFINES*/
#define DEBUG_PRINT //ENABLE debug print messages by uncommenting
#define ONE_KB 1024
#define WORK 0
#define REST 1
#define TIME_FORMAT_STRING_LEN 6
#define MINUTE_SECONDS 59
#define PBAR_INIT 0.0
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
    GtkImage *play_pause_btn_image;
    GtkProgressBar *pbar;

    uint8_t timerType;
    bool is_playing;
    char *file_path_loc;
    guint timer_tag;
    int minutes;
    int seconds;
};

/*Function Prototypes*/
const char *prg_path(char *file_path, const char *file_loc);
static void working_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
static void working_reset_btn_clicked(GtkWidget *widget, gpointer data);
static void resting_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
static void resting_reset_btn_clicked(GtkWidget *widget, gpointer data);
static void counter_up_btn_clicked(GtkWidget *widget, gpointer data);
static void counter_down_btn_clicked(GtkWidget *widget, gpointer data);
bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, char *file_path, uint8_t timerType); //return true if success
bool reset_timer(struct TimerUI *timerUi);
void delete_allocation(struct TimerUI *ptr, gpointer data);
void delete_file_path_allocation(char *file_path);
void play_pause_action(gpointer data);
void reset_action(gpointer data);
gboolean timer_handler(struct TimerUI *timerUi);
void format_Countdown(struct TimerUI *timerUi);
void pbar_update(struct TimerUI *timerUi);

int main(int argc,
         char *argv[])
{
    /*Variables*/
    GtkWidget *window; //GTK window
    GtkBuilder *builder;
    char *file_path = malloc(2 * ONE_KB);
    GtkButton *counter_up_btn;
    GtkButton *counter_down_btn;
    FILE *fPointer;
    GError *error = NULL;

    //alocation heap
    struct TimerUI *work_TimerUI = malloc(sizeof *work_TimerUI), *rest_TimerUI = malloc(sizeof *rest_TimerUI);

    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder, prg_path(file_path, "/GTK-Pomodoro/glade/mainUI.glade"), &error) == 0)
    {
#ifdef DEBUG_PRINT
        g_printerr("Error loading file: %s\n", error->message);
#endif

        g_clear_error(&error);
        return 1;
    }

    if (!init_timer_interface(builder, work_TimerUI, file_path, WORK) || !init_timer_interface(builder, rest_TimerUI, file_path, REST))
    {
#ifdef DEBUG_PRINT
        g_printerr("Error loading init_timer_interface()\n");
#endif
        return 1;
    }

    /*INIT DAILY COUNTER FUNC()*/
    counter_up_btn = GTK_BUTTON(gtk_builder_get_object(builder, "counter_up_btn"));
    counter_down_btn = GTK_BUTTON(gtk_builder_get_object(builder, "counter_down_btn"));
    //g_signal_connect(counter_up_btn, "clicked", G_CALLBACK(counter_up_btn_clicked), NULL);
    //g_signal_connect(counter_down_btn, "clicked", G_CALLBACK(counter_down_btn_clicked), NULL);

    // fPointer = fopen("record.txt", "r+");
    // fprintf(fPointer, "Hi!\n");
    // fclose(fPointer);

    /* Connect signal handlers to the constructed widgets. */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));   //PARAM: , widget ID
    gtk_window_set_title(GTK_WINDOW(window), "Pomodoroooo!");             //Set title of program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //callback func to destroy window upon exit
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation), work_TimerUI);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_allocation), rest_TimerUI);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(delete_file_path_allocation), file_path);
    /*CALL DESTROY FOR FILE POINTER*/

    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show_all(window);
    gtk_main();              //run program loop
    g_object_unref(builder); //free object in memory

    return 0;
}

/*Func to pass correct formatted file dir to glade file for builder*/
const char *prg_path(char *file_path, const char *file_loc)
{
    char absPath[ONE_KB];
    char concat_str[2 * ONE_KB];
    char old_char = '\\';
    char new_char = '/';

    getcwd(absPath, sizeof(absPath)); //equiv to pwd - to grab cur file path location
    for (int i = 0; i < strlen(absPath); i++)
    {
        if (absPath[i] == old_char)
        {
            absPath[i] = new_char;
        }
    }

    snprintf(concat_str, sizeof(concat_str), "%s%s", absPath, file_loc);
    strncpy(file_path, concat_str, 2 * ONE_KB); //safe copy to prevent buffer overflow

#ifdef DEBUG_PRINT
    g_print("Combined Current working dir: %s\n", concat_str);
#endif
    return file_path;
}

bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, char *file_path, uint8_t timerType)
{
    bool status_flag = true;

    switch (timerType)
    {
    case WORK:
        timerUi->timerType = WORK;
        timerUi->file_path_loc = file_path;
        timerUi->is_playing = false; //start initial at pause state
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_working"));
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_working"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(working_play_pause_btn_clicked), timerUi); //callback function upon clicked action, PARAM (final): pass address of data (single static struct variable)
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_working"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(working_reset_btn_clicked), timerUi);
        timerUi->play_pause_btn_image = GTK_IMAGE(gtk_builder_get_object(builder, "working_play_btn_image"));
        timerUi->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "pbar_working"));
        status_flag = reset_timer(timerUi) ? true : false;
        break;
    case REST:
        timerUi->timerType = REST;
        timerUi->file_path_loc = file_path;
        timerUi->is_playing = false;
        timerUi->timeKeeper_label = GTK_LABEL(gtk_builder_get_object(builder, "label_resting"));
        timerUi->play_pause_button = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_resting"));
        g_signal_connect(timerUi->play_pause_button, "clicked", G_CALLBACK(resting_play_pause_btn_clicked), timerUi);
        timerUi->reset_button = GTK_BUTTON(gtk_builder_get_object(builder, "reset_btn_resting"));
        g_signal_connect(timerUi->reset_button, "clicked", G_CALLBACK(resting_reset_btn_clicked), timerUi);
        timerUi->play_pause_btn_image = GTK_IMAGE(gtk_builder_get_object(builder, "resting_play_btn_image"));
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
    gdouble pbar_init = PBAR_INIT;
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
    play_pause_action(data);
}

static void
working_reset_btn_clicked(GtkWidget *widget,
                          gpointer data)
{
    (void)widget; //To get rid of compiler warning
    reset_action(data);
}

static void
resting_play_pause_btn_clicked(GtkWidget *widget,
                               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    play_pause_action(data);
}

static void
resting_reset_btn_clicked(GtkWidget *widget,
                          gpointer data)
{
    (void)widget; //To get rid of compiler warning
    reset_action(data);
}

static void
counter_up_btn_clicked(GtkWidget *widget,
                       gpointer data)
{
    (void)widget; //To get rid of compiler warning
    (void)data;
}

static void
counter_down_btn_clicked(GtkWidget *widget,
                         gpointer data)
{
    (void)widget; //To get rid of compiler warning
    (void)data;
}

/*Free malloc()*/
void delete_allocation(struct TimerUI *ptr, gpointer data)
{
    (void)data; //To get rid of compiler warning
#ifdef DEBUG_PRINT
    g_print("Memory allocation freed %u! \n", ptr->timerType);
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

void play_pause_action(gpointer data)
{

    struct TimerUI *timerUI_ptr = data;
    if (timerUI_ptr->is_playing != true)
    {
        timerUI_ptr->is_playing = true;
        //init tick handler (1 second timer)
        timerUI_ptr->timer_tag = g_timeout_add_seconds(1, (GSourceFunc)timer_handler, timerUI_ptr); //store tag to destroy timeout()
        gtk_image_set_from_file(timerUI_ptr->play_pause_btn_image, prg_path(timerUI_ptr->file_path_loc, "/GTK-Pomodoro/res/icon-pause.png"));
#ifdef DEBUG_PRINT
        g_print("Value of timerType enabeld?: %d & timer tag: %d\n", timerUI_ptr->is_playing, timerUI_ptr->timer_tag);
#endif
    }
    else
    {
        timerUI_ptr->is_playing = false;
        g_source_remove(timerUI_ptr->timer_tag);
        gtk_image_set_from_file(timerUI_ptr->play_pause_btn_image, prg_path(timerUI_ptr->file_path_loc, "/GTK-Pomodoro/res/icon-play.png"));
#ifdef DEBUG_PRINT
        g_print("Value of timerType enabeld?: %d\n", timerUI_ptr->is_playing);
#endif
    }
}

void reset_action(gpointer data)
{
    struct TimerUI *timerUI_ptr = data;
    if (timerUI_ptr->is_playing != true)
    {
        char formattedTime[TIME_FORMAT_STRING_LEN];
        if (timerUI_ptr->timerType == WORK)
        {
            timerUI_ptr->minutes = WORKING_INIT_TIME_MINS;
            timerUI_ptr->seconds = WORKING_INIT_TIME_SECS;
        }
        else
        {
            timerUI_ptr->minutes = RESTING_INIT_TIME_MINS;
            timerUI_ptr->seconds = RESTING_INIT_TIME_SECS;
        }
        snprintf(formattedTime, TIME_FORMAT_STRING_LEN, "%d:%2.2d", timerUI_ptr->minutes, timerUI_ptr->seconds);
        gtk_label_set_text(timerUI_ptr->timeKeeper_label, formattedTime);
    }
}

// handler for the 1 second timer tick
gboolean timer_handler(struct TimerUI *timerUi)
{
    format_Countdown(timerUi);
    return TRUE;
}

void format_Countdown(struct TimerUI *timerUi)
{
    char formattedTime[TIME_FORMAT_STRING_LEN];
    timerUi->seconds--;
    pbar_update(timerUi);

    if (timerUi->minutes > 0 && timerUi->seconds < 0)
    {
        timerUi->minutes--;
        timerUi->seconds = MINUTE_SECONDS;
    }
    else if (timerUi->minutes <= 0 && timerUi->seconds < 0)
    {
        //PLAY TIMER UP SOUND
        g_source_remove(timerUi->timer_tag);
        timerUi->is_playing = false;
        gtk_image_set_from_file(timerUi->play_pause_btn_image, prg_path(timerUi->file_path_loc, "/GTK-Pomodoro/res/icon-play.png"));

        timerUi->minutes = 0;
        timerUi->seconds = 0;
    }
    snprintf(formattedTime, TIME_FORMAT_STRING_LEN, "%d:%2.2d", timerUi->minutes, timerUi->seconds);
    gtk_label_set_text(timerUi->timeKeeper_label, formattedTime);
}

void pbar_update(struct TimerUI *timerUi)
{
    float total_time = (timerUi->timerType == WORK) ? WORKING_INIT_TIME_MINS * MINUTE_SECONDS + WORKING_INIT_TIME_SECS : RESTING_INIT_TIME_MINS * MINUTE_SECONDS + RESTING_INIT_TIME_SECS;
    float ratio = (total_time - ((float)timerUi->minutes * MINUTE_SECONDS + (float)timerUi->seconds)) / total_time;
    gtk_progress_bar_set_fraction(timerUi->pbar, ratio);
}
