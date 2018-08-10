#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
  GtkBuilder *builder;
  GtkWidget *window;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "ui.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "menu"));
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
  // exec
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

