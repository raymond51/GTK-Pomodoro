#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

/*includes*/
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "helper.h"

/*defines*/
#define WORK 0
#define REST 1
#define TIME_FORMAT_STRING_LEN 6
#define MINUTE_SECONDS 59
#define PBAR_INIT 0.0
#define COUNTER_INIT 0
#define WORKING_INIT_TIME_MINS 1
#define WORKING_INIT_TIME_SECS 0
#define RESTING_INIT_TIME_MINS 5
#define RESTING_INIT_TIME_SECS 0
#define ICON_PAUSE_RES_LOC "/GTK-Pomodoro/res/icon-pause.png"
#define ICON_PLAY_RES_LOC "/GTK-Pomodoro/res/icon-play.png"

/*Enum*/
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

    uint8_t Type;
    bool is_playing;
    char *file_path_loc;
    guint timer_tag;
    int minutes;
    int seconds;
};

/*Function prototpyes*/
bool init_timer_interface(GtkBuilder *builder, struct TimerUI *timerUi, char *file_path, uint8_t timerType); //return true if success
void working_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
void working_reset_btn_clicked(GtkWidget *widget, gpointer data);
void resting_play_pause_btn_clicked(GtkWidget *widget, gpointer data);
void resting_reset_btn_clicked(GtkWidget *widget, gpointer data);
void play_pause_action(gpointer data);
void reset_action(gpointer data);
gboolean timer_handler(struct TimerUI *timerUi);
void format_Countdown(struct TimerUI *timerUi);
void pbar_update(struct TimerUI *timerUi);
bool reset_timer(struct TimerUI *timerUi);

#endif //TIMER_INTERFACE_H_