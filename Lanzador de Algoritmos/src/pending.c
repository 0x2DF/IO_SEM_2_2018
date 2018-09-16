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

  const gchar *cssFile = "customj.css";
  GError *error = 0;
  gtk_css_provider_load_from_file(provider, g_file_new_for_path(cssFile), &error);
  g_object_unref (provider);
}

int main(int argc, char *argv[])
{
  GtkBuilder *builder;
  GtkWidget *window;

  GError *err = NULL;

  gtk_init(&argc, &argv);
  CSS();
  builder = gtk_builder_new();
  if (0 == gtk_builder_add_from_file(builder, "pending.glade", &err))
  {
    fprintf(stderr, "Error adding build from file. Error: %s\n", err->message);
  }

  window = GTK_WIDGET(gtk_builder_get_object(builder, "pending"));
  if (NULL == window)
  {
    fprintf(stderr, "Unable to find object with id \"pending\" \n");
  }
  
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
