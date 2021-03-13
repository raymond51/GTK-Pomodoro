#include "helper.h"

/*Func to pass correct formatted file dir to glade file for builder*/
const char *prg_path(char *file_path, const char *file_loc)
{
    char absPath[ONE_KB];
    char concat_str[2 * ONE_KB];
    char old_char = '\\';
    char new_char = '/';

    getcwd(absPath, sizeof(absPath)); //equiv to pwd - to grab cur file path location
    for (int i = 0; i < (int)strlen(absPath); i++)
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

void message_dialog(char *primary_msg, char *secondary_msg)
{
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, primary_msg);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(message), secondary_msg, 99);
    gtk_dialog_run(GTK_DIALOG(message));
    gtk_widget_destroy(message);
}

/*Enable manual update request of daily counter depending on user*/
bool message_dialog_counter_update(char *primary_msg, char *secondary_msg)
{
    bool user_status;
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, primary_msg);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(message), secondary_msg, 99);
    int response = gtk_dialog_run(GTK_DIALOG(message));
    user_status = (response == GTK_RESPONSE_OK) ? true : false;
    gtk_widget_destroy(message);
    return user_status;
}
