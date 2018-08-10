#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
  GtkBuilder *builder;
  GtkWidget *window;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "pending.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "pending"));
  gtk_builder_connect_signals(builder, NULL);
  
  g_object_unref(builder);
  gtk_widget_show(window);
  gtk_main();

  return 0;
}

void on_pending_destroy()
{
  gtk_main_quit();
}

void on_pending_btn_clicked()
{
  gtk_main_quit();
}
