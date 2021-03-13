#ifndef HELPER_H
#define HELPER_H

/*includes*/
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

/*defines*/
#define DEBUG_PRINT //ENABLE debug print messages by uncommenting
#define GLADE_FILE_LOC "/GTK-Pomodoro/glade/mainUI.glade"
#define ONE_KB 1024
#define TWEPOWEIGHT 256

/*Function Prototypes*/
const char *prg_path(char *file_path, const char *file_loc);
void message_dialog(char *primary_msg, char *secondary_msg);


#endif //HELPER_H