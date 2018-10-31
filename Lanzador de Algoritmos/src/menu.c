#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

void CSS(void)
{
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;

  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  const gchar *cssFile = "series.css";
  GError *error = 0;
  gtk_css_provider_load_from_file(provider, g_file_new_for_path(cssFile), &error);
  g_object_unref (provider);
}

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
  CSS();
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

/* 
int pid;

  if ( (pid = fork()) == 0) {
    execl("./pending", "./pending", NULL);
    perror("pending");
  }
*/

void on_rmc_btn_clicked()
{
  system("./floyd &"); 
}

void on_tba_btn_1_clicked()
{
  system("./knapsack &");
}

void on_tba_btn_2_clicked()
{
  system("./series &");
}

void on_tba_btn_3_clicked()
{
  system("./equipos &");
}

void on_tba_btn_4_clicked()
{
  system("./abbo &");
}

void on_about_ok_btn_clicked()
{
  gtk_main_quit();
}

void on_about_destroy()
{
  gtk_main_quit();
}

void on_menu_quit_button_press_event()
{
  gtk_main_quit();
}

void on_menu_about_button_press_event()
{
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
	
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "ui.glade", NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "about"));
  gtk_builder_connect_signals(builder, NULL);
  accept = GTK_WIDGET(gtk_builder_get_object(builder, "about_ok_btn"));
  g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
					  
  g_object_unref(builder);
 
  gtk_widget_show(window);
  gtk_main();
}
