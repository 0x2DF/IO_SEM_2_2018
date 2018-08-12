#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

void close_window(GtkWidget *widget, gpointer window)
{
    gtk_widget_destroy(GTK_WIDGET(window));
}

int main(int argc, char *argv[])
{
  GtkBuilder *builder;
  GtkWidget *window;
  GError *err = NULL;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (0 == gtk_builder_add_from_file(builder, "ui.glade", &err))
  {
    fprintf(stderr, "Error adding build from file. Error: %s\n", err->message);
  }

  window = GTK_WIDGET(gtk_builder_get_object(builder, "menu"));
  if (NULL == window)
  {
    fprintf(stderr, "Unable to find object with id \"menu\" \n");
  }
  gtk_builder_connect_signals(builder, NULL);
  
  g_object_unref(builder);
  gtk_widget_show(window);
  gtk_main();

  return 0;
}

void on_menu_destroy()
{
  gtk_main_quit();
}

void on_exit_btn_clicked()
{
  gtk_main_quit();
}

void on_rmc_btn_clicked()
{
  int status;
  if (fork() == 0)
  {
    status = system("./pending");
    exit(0);
  }

}

void on_tba_btn_1_clicked()
{
  
  int status;
  if (fork() == 0)
  {
    status = system("./pending");
    exit(0);
  }
}

void on_tba_btn_2_clicked()
{

  int status;
  if (fork() == 0)
  {
    status = system("./pending");
    exit(0);
  }
}

void on_tba_btn_3_clicked()
{

  int status;
  if (fork() == 0)
  {
    status = system("./pending");
    exit(0);
  }
}

void on_tba_btn_4_clicked()
{

  int status;
  if (fork() == 0)
  {
    status = system("./pending");
    exit(0);
  }
}

void on_about_ok_btn_clicked()
{
  gtk_main_quit();
}

void on_about_destroy()
{
  gtk_main_quit();
}

void on_about_menu_button_press_event()
{
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
	
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "ui.glade", NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "about"));
  gtk_builder_connect_signals(builder, NULL);
  accept = GTK_WIDGET(gtk_builder_get_object(builder, "about_ok_btn"));
  g_signal_connect (G_OBJECT (accept),
					  "clicked",
					  G_CALLBACK (close_window),
					  G_OBJECT (window));
					  
  g_object_unref(builder);
 
  gtk_widget_show(window);
  gtk_main();
}
