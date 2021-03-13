#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

/*includes*/
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

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