#include "timer_interface.h"

/*Static function prototypes*/
static void play_pause_action(gpointer data);
static void reset_action(gpointer data);
static void pbar_update(struct TimerUI *timerUi);
static bool reset_timer(struct TimerUI *timerUi);
static void format_Countdown(struct TimerUI *timerUi);
static gboolean timer_handler(struct TimerUI *timerUi);

bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, char *file_path, uint8_t timerType)
{
    bool status_flag = true;

    switch (timerType)
    {
    case WORK:
        timerUi->Type = WORK;
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
        timerUi->Type = REST;
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

/*sends activate signal upon function call g_application_run(), behave like virtual function that application responds to*/
void working_play_pause_btn_clicked(GtkWidget *widget,
                                    gpointer data)
{
    (void)widget; //To get rid of compiler warning
    play_pause_action(data);
}

void working_reset_btn_clicked(GtkWidget *widget,
                               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    reset_action(data);
}

void resting_play_pause_btn_clicked(GtkWidget *widget,
                                    gpointer data)
{
    (void)widget; //To get rid of compiler warning
    play_pause_action(data);
}

void resting_reset_btn_clicked(GtkWidget *widget,
                               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    reset_action(data);
}

static void play_pause_action(gpointer data)
{

    struct TimerUI *timerUI_ptr = data;
    if (timerUI_ptr->is_playing != true)
    {
        timerUI_ptr->is_playing = true;
        //init tick handler (1 second timer)
        timerUI_ptr->timer_tag = g_timeout_add_seconds(1, (GSourceFunc)timer_handler, timerUI_ptr); //store tag to destroy timeout()
        gtk_image_set_from_file(timerUI_ptr->play_pause_btn_image, prg_path(timerUI_ptr->file_path_loc, ICON_PAUSE_RES_LOC));
#ifdef DEBUG_PRINT
        g_print("Value of timerType enabeld?: %d & timer tag: %d\n", timerUI_ptr->is_playing, timerUI_ptr->timer_tag);
#endif
    }
    else
    {
        timerUI_ptr->is_playing = false;
        g_source_remove(timerUI_ptr->timer_tag);
        gtk_image_set_from_file(timerUI_ptr->play_pause_btn_image, prg_path(timerUI_ptr->file_path_loc, ICON_PLAY_RES_LOC));
#ifdef DEBUG_PRINT
        g_print("Value of timerType enabeld?: %d\n", timerUI_ptr->is_playing);
#endif
    }
}

static void reset_action(gpointer data)
{
    struct TimerUI *timerUI_ptr = data;
    if (timerUI_ptr->is_playing != true)
    {
        char formattedTime[TIME_FORMAT_STRING_LEN];
        gdouble pbar_init = PBAR_INIT;
        if (timerUI_ptr->Type == WORK)
        {
            timerUI_ptr->minutes = WORKING_INIT_TIME_MINS;
            timerUI_ptr->seconds = WORKING_INIT_TIME_SECS;
        }
        else
        {
            timerUI_ptr->minutes = RESTING_INIT_TIME_MINS;
            timerUI_ptr->seconds = RESTING_INIT_TIME_SECS;
        }
        gtk_progress_bar_set_fraction(timerUI_ptr->pbar, pbar_init);
        snprintf(formattedTime, TIME_FORMAT_STRING_LEN, "%d:%2.2d", timerUI_ptr->minutes, timerUI_ptr->seconds);
        gtk_label_set_text(timerUI_ptr->timeKeeper_label, formattedTime);
    }
}

// handler for the 1 second timer tick
static gboolean timer_handler(struct TimerUI *timerUi)
{
    format_Countdown(timerUi);
    return TRUE;
}

static void format_Countdown(struct TimerUI *timerUi)
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

        g_source_remove(timerUi->timer_tag);
        timerUi->is_playing = false;
        gtk_image_set_from_file(timerUi->play_pause_btn_image, prg_path(timerUi->file_path_loc, ICON_PLAY_RES_LOC));

        timerUi->minutes = 0;
        timerUi->seconds = 0;

        /*play audio timer up*/
        timer_up_audio_play();
    }
    snprintf(formattedTime, TIME_FORMAT_STRING_LEN, "%d:%2.2d", timerUi->minutes, timerUi->seconds);
    gtk_label_set_text(timerUi->timeKeeper_label, formattedTime);
}

static void pbar_update(struct TimerUI *timerUi)
{
    float total_time = (timerUi->Type == WORK) ? WORKING_INIT_TIME_MINS * MINUTE_SECONDS + WORKING_INIT_TIME_SECS : RESTING_INIT_TIME_MINS * MINUTE_SECONDS + RESTING_INIT_TIME_SECS;
    float ratio = (total_time - ((float)timerUi->minutes * MINUTE_SECONDS + (float)timerUi->seconds)) / total_time;
    gtk_progress_bar_set_fraction(timerUi->pbar, ratio);
}

static bool reset_timer(struct TimerUI *timerUi)
{
    bool status_flag = true;
    gdouble pbar_init = PBAR_INIT;
    char formattedTime[TIME_FORMAT_STRING_LEN];

    switch (timerUi->Type)
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