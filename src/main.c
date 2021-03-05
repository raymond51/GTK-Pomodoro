#include <gtk/gtk.h>

/*DEFINES*/
#define WORK 0
#define REST 1

/*Variables*/
GtkWidget *g_window; //GTK window

/*Function Prototypes*/
static void button_clicked(GtkWidget *widget, gpointer data);

int main(int argc,
         char *argv[])
{

    GtkBuilder *builder;
    GtkLabel *workingLabel;
    GtkLabel *restingLabel;
    GtkButton *workButton_play;
    GtkButton *restButton_play;
    GError *error = NULL;

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

    /* Connect signal handlers to the constructed widgets. */
    g_window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));   //PARAM: , widget ID
    g_signal_connect(g_window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //callback func to destroy window upon exit

    workingLabel = GTK_LABEL(gtk_builder_get_object(builder, "label_working")); //Load label
    restingLabel = GTK_LABEL(gtk_builder_get_object(builder, "label_rest"));
    workButton_play = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_working"));
    g_signal_connect(workButton_play, "clicked", G_CALLBACK(button_clicked), WORK); //callback function upon clicked action

    restButton_play = GTK_BUTTON(gtk_builder_get_object(builder, "play_btn_rest"));
    g_signal_connect(restButton_play, "clicked", G_CALLBACK(button_clicked), REST);

    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show_all(g_window);
    gtk_main();              //run program loop
    g_object_unref(builder); //free object in memory

    return 0;
}

/*sends activate signal upon function call g_application_run(), behave like virtual function that application responds to*/
static void
button_clicked(GtkWidget *widget,
               gpointer data)
{
    (void)widget; //To get rid of compiler warning
    g_print("Hello World, Value of btn: %d \n", data);
    //gtk_label_set_text(myLabel, "Helloooooooooo!");
}

//func to init display: set pbar to 0
//func to link res in code i.e g resource
