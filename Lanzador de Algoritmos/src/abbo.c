#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define FLOAT_LEN 20
#define NODE_LEN 20
#define NUMBER_LEN 10
#define PATH_LEN 512

static const char GLADE[] = "series.glade";

int N = 0;
int LOADED = 0;

char** NODES = NULL;
float** table = NULL;

void on_menu_save_as_button_press_event();

typedef struct Widgets {
  GtkWindow *window;
  GtkWindow *parent;
  GtkSpinButton *sb_1;
  GtkButton *btn_1;
  GtkWidget ***format_table;
  GtkWidget ***A_table;
  GtkWidget ***R_table;
} Widgets;

void alloc_nodes(int n)
{
  NODES = malloc(n * sizeof(char*));
  for (int i = 0; i < n; ++i)
  {
    TEAMS[i] = malloc((NODE_LEN + 1) * sizeof(char));
  }
}

void alloc_table()
{
  table = malloc((GAMES_QTY + 1) * sizeof(float*));
  for (int i = 0; i < (GAMES_QTY + 1); ++i)
  {
    table[i] = malloc((GAMES_QTY + 1) * sizeof(float));
  }
  for (int i = 0; i < (GAMES_QTY + 1); ++i)
  {
    for (int j = 0; j < (GAMES_QTY + 1); ++j)
	  {
	    if (i == 0) table[i][j] = 1.0;
      else if (j == 0) table[i][j] = 0.0;
      else
	    {
	      if (get_house(i, j) == 'A')
		    {
	        table[i][j] = PH * table[i-1][j] + (1-PH) * table[i][j-1];
		    }else
		    {
		      table[i][j] = PR * table[i-1][j] + (1-PR) * table[i][j-1]; 
		    }
	    }
	  }
  }
}


int get_index_from_table(const char *team)
{
  for (int i = 0; i < 2; ++i)
  {
    if (strcmp(TEAMS[i],team) == 0)
    {
      return i;
    }
  }
  printf("Error no se econtro el nodo\n");
  return -1;
}

void get_team_name(int i, char *team_name)
{
  int j = 0;
  do
  {
    team_name[j++] = 65 + (i%26);
    i -= 26;
  }while (i > 0);
  team_name[j] = '\0';
}

/* https://stackoverflow.com/questions/1997587/function-to-check-if-input-string-is-an-integer-or-floating-point-number */
int isfloat (const char *s)
{
     char *ep = NULL;
     long i = strtol (s, &ep, 10);

     if (!*ep)
         return 0;  // it's an int

     if (*ep == 'e'  ||
         *ep == 'E'  ||
         *ep == '.')
         return 1;

     return 0;  // it not a float, but there's more stuff after it
}

/* https://rosettacode.org/wiki/Determine_if_a_string_is_numeric#C */
int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

void loadFileBetter(char *filename)
{
  int i, k = 0;
  int error = 0;
  size_t len = 0;
  ssize_t read;
  char * line = NULL;
  char *token, *str, *tofree;
  FILE *fp = fopen(filename, "r");
  if (fp)
  {
    while ((read = getline(&line, &len, fp)) != -1) 
    {
      if (line[read - 1] == '\n') 
      {
        line[read - 1] = '\0';
        --len;
      }

      if (k == 0) // Mejor de N
      {
        i = 0;
        tofree = str = strdup(line); 
        while ((token = strsep(&str, ",")) && (error != 1))
        {
          if (i == 0)
          {
            if (isNumeric(token))
            {
              int temp_qty = (int) strtol(token, (char **)NULL, 10);
              if (temp_qty > 0)
              {
                BoN = temp_qty;
                calculate_games();
                if (TEAMS)
                {
                  free(TEAMS[0]);
                  free(TEAMS[1]);
                  free(TEAMS);
                }
                alloc_teams();
              }else
              {
                error = 1;
                printf("Error: La cantidad de juegos tiene que ser mayor a 0.\n");
              }
            }else
            {
              error = 1;
              printf("Error: Cantidad de juegos no es un valor numerico.\n");
            }
          }
          i++;
        }
        free(tofree);
      }else if (k == 1) // Nombres de equipos
      {
        i = 0;
        tofree = str = strdup(line); 
        while ((token = strsep(&str, ",")) && (error != 1))
        {
          if (i < 2)
          {
            strcpy(TEAMS[i], token);
          }else
          {
            error = 1;
            printf("Solo debe ingresar dos equipos\n");
          }
          i++;
        }
        free(tofree);
      }else if (k == 2) // Cantidad de formato
      {
        i = 0;
        tofree = str = strdup(line); 
        while ((token = strsep(&str, ",")) && (error != 1))
        {
          if (i == 0)
          {
            if (isNumeric(token))
            {
              int temp_qty = (int) strtol(token, (char **)NULL, 10);
              if (temp_qty > 0)
              {
                FORMAT_QTY = temp_qty;
                alloc_format();
              }else
              {
                error = 1;
                printf("Error: La cantidad para el formato tiene que ser mayor a 0.\n");
              }
            }else
            {
              error = 1;
              printf("Error: Cantidad de juegos dentro del formato no es un valor numerico.\n");
            }
          }
          i++;
        }
        free(tofree);
      }else if (k == 3) // Formato
      {
        i = 0;
        tofree = str = strdup(line); 
        while ((token = strsep(&str, ",")) && (error != 1))
        {
          if (i < FORMAT_QTY)
          {
            if (isNumeric(token))
            {
              int temp_qty = (int) strtol(token, (char **)NULL, 10);
              format[i] = temp_qty;
            }else
            {
              error = 1;
              printf("Error: Cantidad de juegos dentro del formato no es un valor numerico.\n");
            }
          }
          i++;
        }
        free(tofree);
      }else // PH PR
      {
        i = 0;
        tofree = str = strdup(line); 
        while ((token = strsep(&str, ",")) && (error != 1))
        {
          if (isfloat(token) || isNumeric(token))
          {
            if (i == 0)
            {
              PH = atof(token);
            }else if (i == 1)
            {
              PR = atof(token);
            }
          }else
          {
            error = 1;
            printf("Error: La probabilidad ingresada no es un valor numerico.\n");
          }
          i++;
        }
        free(tofree);
      }
      if (error)
      {
        if (TEAMS)
        {
          free(TEAMS[0]);
          free(TEAMS[1]);
          free(TEAMS);
        }
        TEAMS = NULL;
        if (format)
        {
          free(format);
        }
        format = NULL;
        if (table)
        {
          for (int i = 0; i < (GAMES_QTY + 1); ++i)
          {
            free(table[i]);
          }
          free(table);
        }
        table = NULL;
        printf("Se produjo un error leyendo los contenidos del archivo. Reviselo y vuelva a intentar.\n");
        LOADED = -1;
        break;
      }
      k++;
    }
    fclose(fp);
    if (line) free(line);
  }
}

void saveFile(char *filename)
{
  FILE *fp = fopen(filename, "w");
  if (fp)
  {
    fprintf(fp, "%d\n", BoN);
    fprintf(fp, "%s,%s\n", TEAMS[0], TEAMS[1]);
    fprintf(fp, "%d\n", FORMAT_QTY);
    for (int i = 0; i < FORMAT_QTY; ++i)
    {
      if (i == 0) fprintf(fp, "%d", format[i]);
      else fprintf(fp, ",%d", format[i]);
    }
    fprintf(fp, "\n");
    fprintf(fp, "%f,%f\n", PH, PR);
    printf("Archivo guardado correctamente en: %s\n", filename);
    fclose(fp);
  }else{
    printf("Error al guardar el archivo\n");
  }
}

void close_window(GtkWidget *widget, gpointer window)
{
  gtk_widget_destroy(GTK_WIDGET(window));
}

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

/* * * * * * *
   * Message *
   * * * * * */

void on_message_ok_btn_clicked()
{
  gtk_main_quit();
}

void on_message_destroy()
{
  gtk_main_quit();
}

/* * * * * * *
   * Format  *
   * * * * * */

int all_games_selected(Widgets *data)
{
  char previous = '0';
  FORMAT_QTY = 1;
  for (int i = 2; i < (BoN + 2); ++i)
  {
    if (gtk_toggle_button_get_active(data->table[i][1]))
    {
      if (previous == 'B')
      {
        FORMAT_QTY++;
      }
      previous = 'A';
    }else if (gtk_toggle_button_get_active(data->table[i][2]))
    {
      if (previous == 'A' || previous == '0')
      {
        FORMAT_QTY++;
      }
      previous = 'B';
    }else
    {
      return 0;
    }
  }
  return 1;
}

void set_format(Widgets *data)
{
  alloc_format();
  int n = 0;
  int j = 0;
  char previous = '0';
  for (int i = 2; i < (BoN + 2); ++i)
  {
    if (gtk_toggle_button_get_active(data->table[i][1]))
    {
      if (previous == 'B')
      {
        format[j++] = n;
        n = 1;
      }else
      {
        n++;
      }
      previous = 'A';
    }else if (gtk_toggle_button_get_active(data->table[i][2]))
    {
      if (previous == 'A' || previous == '0')
      {
        format[j++] = n;
        n = 1;
      }else 
      {
        n++;
      }
      previous = 'B';
    }
  }
  format[j] = n;
}

void on_accept_format_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *exit;
  GtkWidget *label;
  GtkWidget *column;
  GtkWidget *scroll;
  GtkWidget *save;
  GtkWidget *window;
  gchar* message = NULL;
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (all_games_selected(data))
  {
    set_format(data);

    alloc_table();

    format_tb = GTK_WIDGET(gtk_builder_get_object(builder, "format_tb"));
    char format_str[FORMAT_LEN];
    char num[NUMBER_LEN];
    snprintf(num, NUMBER_LEN, "%d", format[0]);
    strcpy(format_str, num);
    for (int i = 1; i < FORMAT_QTY; ++i)
    {
      char num_s[NUMBER_LEN];
      snprintf(num_s, NUMBER_LEN, "-%d", format[i]);
      strcat(format_str, num_s);
    }
    gtk_entry_set_text (GTK_ENTRY(format_tb), format_str);

  }else message = "Falta seleccionar el equipo casa para uno o mas juegos.";

  if (message)
  {
    data->window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (data->window, data->parent);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (data->window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    gtk_label_set_text (GTK_LABEL(label), message);
    g_object_unref(builder);
    gtk_widget_show(data->window);
    
  }else
  {
    Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));

    window = GTK_WIDGET(gtk_builder_get_object(builder, "results"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_results_destroy), widgets);
    widgets->parent = window;
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);

    scroll = GTK_WIDGET(gtk_builder_get_object(builder, "results_scrolled"));

    widgets->table = calloc(GAMES_QTY + 3, sizeof(GtkWidget**));
    for(int i = 0; i < (GAMES_QTY + 3); ++i)
    {
      widgets->table[i] = calloc((GAMES_QTY + 3), sizeof(GtkWidget*));
    }
    column = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (scroll), column);
    fill_result_table(widgets, column);

    char pa_str[100];
    strcpy(pa_str, "La probabilidad de ");
    strcat(pa_str, TEAMS[0]);
    strcat(pa_str, " de ganar es de: ");
    char pa[FLOAT_LEN];
    snprintf(pa, FLOAT_LEN, "%.7f", table[GAMES_QTY][GAMES_QTY]);
    strcat(pa_str, pa);

    pa_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "pa_lbl"));
    gtk_label_set_text (GTK_LABEL(pa_lbl), pa_str);


    char pb_str[100];
    strcpy(pb_str, "La probabilidad de ");
    strcat(pb_str, TEAMS[1]);
    strcat(pb_str, " de ganar es de: ");
    char pb[FLOAT_LEN];
    snprintf(pb, FLOAT_LEN, "%.7f", 1-table[GAMES_QTY][GAMES_QTY]);
    strcat(pb_str, pb);

    pb_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "pb_lbl"));
    gtk_label_set_text (GTK_LABEL(pb_lbl), pb_str);

    save = GTK_WIDGET(gtk_builder_get_object(builder, "menu_save"));
    g_signal_connect(G_OBJECT(save), "button-press-event", G_CALLBACK(on_menu_save_as_button_press_event), widgets);

    exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_results_btn"));
    g_signal_connect (G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);
    g_object_unref(builder);
    gtk_widget_show_all(widgets->parent);
  }
  
  gtk_main();
}

void on_format_destroy(GtkWidget *widget, Widgets *data)
{
  if (data->table)
  {
    for (int i = 0; i < (BoN + 2); ++i)
    {
      free(data->table[i]);
    }
    free(data->table);
    data->table = NULL;
  }
  free(data);
  data = NULL;
  gtk_main_quit();
}

/* * * * * * *
   * Config  *
   * * * * * */

void fill_format_table(Widgets* data, GtkWidget* column)
{

  data->table[0][0] = gtk_label_new("Nombre");
  gtk_grid_attach (GTK_GRID (column),data->table[0][0] , 0, 0, 1, 1);
  data->table[0][1] = gtk_label_new("Peso");
  gtk_grid_attach (GTK_GRID (column),data->table[0][1] , 1, 0, 2, 1);

  data->table[1][1] = gtk_entry_new();
  gtk_widget_set_name (data->table[1][1], "team_a_name");
  gtk_entry_set_width_chars(GTK_ENTRY(data->table[1][1]),12);
  gtk_editable_set_editable(GTK_ENTRY(data->table[1][1]),FALSE);
  gtk_entry_set_text (GTK_ENTRY(data->table[1][1]), TEAMS[0]);
  gtk_grid_attach (GTK_GRID (column),data->table[1][1] , 1, 1, 1, 1);
  data->table[1][2] = gtk_entry_new();
  gtk_widget_set_name (data->table[1][2],"team_b_name");
  gtk_entry_set_width_chars(GTK_ENTRY(data->table[1][2]),12);
  gtk_editable_set_editable(GTK_ENTRY(data->table[1][2]),FALSE);
  gtk_entry_set_text (GTK_ENTRY(data->table[1][2]), TEAMS[1]);
  gtk_grid_attach (GTK_GRID (column),data->table[1][2] , 2, 1, 1, 1);

  for(int i = 1; i < (BoN + 1); ++i) 
  {
    char num[NUMBER_LEN];
    snprintf(num, NUMBER_LEN, "%d", i-1);
    data->table[i][0] = gtk_label_new(num);
    gtk_widget_set_name (data->table[i][0],"entry");
    gtk_grid_attach (GTK_GRID (column),data->table[i][0] , 0, i, 1, 1);

    for(int j = 1; j < 3; ++j) 
    {
        if (j == 1) data->table[i][1] = gtk_radio_button_new(NULL);
        else data->table[i][j] = gtk_radio_button_new_from_widget(data->table[i][1]);

        gtk_grid_attach (GTK_GRID (column),data->table[i][j] , j, i, 1, 1);
    }
  }
}

void on_accept_config_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
  GtkWidget *exit;
  GtkWidget *label;
  GtkWidget *column;
  GtkWidget *scroll;
  gchar* message = NULL;
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (NODES)
  {
    for (int i = 0; i < N; ++i)
    {
      free(NODES[i]);
    }
    free(NODES);
  }
  N = gtk_spin_button_get_value_as_int(data->sb_1);
  if (N <= 0)
  {
    message = "Cantidad de nodos tiene que ser superior a 0.";
  }

  if (message)
  {
    data->window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (data->window, data->parent);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (data->window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    gtk_label_set_text (GTK_LABEL(label), message);
    g_object_unref(builder);
    gtk_widget_show(data->window);
  }else
  {
    Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
    widgets->table = NULL;

    alloc_nodes(N);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "format"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_format_destroy), widgets);
    widgets->parent = window;
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);

    scroll = GTK_WIDGET(gtk_builder_get_object(builder, "format_scrolled"));
    widgets->table = calloc(BoN + 1, sizeof(GtkWidget**));
    for(int i = 0; i < (BoN + 1); ++i)
    {
      widgets->table[i] = calloc(2, sizeof(GtkWidget*));
    }
    column = gtk_grid_new();
    gtk_container_add (GTK_CONTAINER (scroll), column);
    fill_format_table(widgets, column);

    widgets->btn_1 = GTK_WIDGET(gtk_builder_get_object(builder, "accept_format_btn"));
    g_signal_connect (G_OBJECT (widgets->btn_1), "clicked", G_CALLBACK (on_accept_format_btn_clicked), widgets);

    exit = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_format_btn"));
    g_signal_connect (G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);
    g_object_unref(builder);
    gtk_widget_show_all(widgets->parent);
  }
  
  gtk_main();
}

void on_config_destroy(GtkWidget *widget, Widgets *data)
{
  if (data->table)
  {
    for (int i = 0; i < (BoN + 1); ++i)
    {
      free(data->table[i]);
    }
    free(data->table);
    data->table = NULL;
  }
  free(data);
  data = NULL;
  if (TEAMS)
  {
    free(TEAMS[0]);
    free(TEAMS[1]);
    free(TEAMS);
    TEAMS = NULL;
  }
  gtk_main_quit();
}

/* * * * * *
   * About *
   * * * * */

void on_about_ok_btn_clicked()
{
  gtk_main_quit();
}

void on_about_destroy()
{
  gtk_main_quit();
}

/* * * * * * * *
   * Menu Bar  *
   * * * * * * */


void on_load_config_btn_clicked(GtkWidget *widget, Widgets *data)
{
  printf("data = %d\n(data->sb_1) = %d\n", data, (data->sb_1));
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", 
          GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
  LOADED = 0;
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    loadFileBetter(filename);
  }
  gtk_widget_destroy (dialog);
  if (LOADED != -1)
  {
    LOADED = 1;
    fast_load(data);
  }
}

void on_menu_save_as_button_press_event()
{
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  gchar * text;
  dialog = gtk_file_chooser_dialog_new ("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "_Cancel",
          GTK_RESPONSE_CANCEL, "_Save",  GTK_RESPONSE_ACCEPT, NULL);
  chooser = GTK_FILE_CHOOSER (dialog);
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
  gtk_file_chooser_set_current_name (chooser, "Untitled document");
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    filename = gtk_file_chooser_get_filename (chooser);
    saveFile(filename);
  }
  gtk_widget_destroy (dialog);
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
  gtk_builder_add_from_file (builder, GLADE, NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "about"));
  gtk_builder_connect_signals(builder, NULL);
  accept = GTK_WIDGET(gtk_builder_get_object(builder, "about_ok_btn"));
  g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
					  
  g_object_unref(builder);
 
  gtk_widget_show(window);
  gtk_main();
}

/* * * * * * *
   * Config  *
   * * * * * */

int main(int argc, char *argv[])
{
  GtkBuilder *builder = NULL;
  GtkWidget *window = NULL;
  GError *err = NULL;

  GtkWidget *accept = NULL;
  GtkWidget *exit = NULL;
  GtkWidget *load = NULL;

  Widgets *widgets = NULL; 

  gtk_init(&argc, &argv);

  CSS();

  builder = gtk_builder_new();
  if (0 == gtk_builder_add_from_file(builder, GLADE, &err))
  {
    fprintf(stderr, "Error adding build from file. Error: %s\n", err->message);
    return 1;
  }

  window = GTK_WIDGET(gtk_builder_get_object(builder, "config"));
  if (NULL == window)
  {
    fprintf(stderr, "Unable to find object with id \"config\" \n");
    return 1;
  }
  gtk_builder_connect_signals(builder, NULL);
  
  widgets = (Widgets*)malloc(1 * sizeof(Widgets));
  widgets->parent = window;
  g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(on_config_destroy), widgets);

  widgets->sb_1 = GTK_WIDGET(gtk_builder_get_object(builder, "nodes_sb"));

  accept = GTK_WIDGET(gtk_builder_get_object(builder, "continue_config_btn"));
  g_signal_connect(G_OBJECT (accept), "clicked", G_CALLBACK (on_accept_config_btn_clicked), widgets);

  exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_config_btn"));
  g_signal_connect(G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);

  load = GTK_WIDGET(gtk_builder_get_object(builder, "load_config_btn"));
  g_signal_connect(G_OBJECT (load), "clicked", G_CALLBACK (on_load_config_btn_clicked), widgets);

  g_object_unref(builder);
  gtk_widget_show(window);
  gtk_main();
  
  return 0;
}
