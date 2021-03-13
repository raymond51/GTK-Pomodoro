#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

/*includes*/
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

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

#endif //TIMER_INTERFACE_H_