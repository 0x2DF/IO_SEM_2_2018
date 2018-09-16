#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define FLOAT_LEN 20
#define TEAM_LEN 20

#define NUMBER_LEN 10
#define NODES_QTY_LEN 16
#define PATH_LEN 512

static const char GLADE[] = "series.glade";

int BoN = 1;
int GAMES_QTY = 1;
int FORMAT_QTY = 1;
float PH = 0.5;
float PR = 0.5;

char** TEAMS = NULL;
float** table = NULL;
int* format = NULL;

int calculate_games(void) { GAMES_QTY = ((BoN + 1) / 2) + 1; }

typedef struct Widgets {
  GtkWindow *window;
  GtkWindow *parent;
  GtkEntry *tb_1;
  GtkEntry *tb_2;
  GtkEntry *tb_3;
  GtkEntry *tb_4;
  GtkEntry *tb_5;
  GtkButton *btn_1;
  GtkButton *btn_2;
  GtkButton *btn_3;
  GtkWidget ***table;
  GtkLabel *lbl_1;
  GtkLabel *lbl_2;
} Widgets;

void clear()
{
  if (TEAMS)
  {
    free(TEAMS[0]);
    free(TEAMS[1]);
    free(TEAMS);
  }
  
  if (table)
  {
    for (int i = 0; i < (GAMES_QTY + 1); ++i)
    {
      free(table[i]);
    }
    free(table);
  }
  
  if (format)
  {
    free(format);
  }
  
}

char get_house(int i, int j)
{
  int k = ((GAMES_QTY * 2) + 1) - (i + j); 
  char house = 'A';
  for (int i = 0; i < FORMAT_QTY; ++i)
  {
	if (format[i] > k) k-=format[i];
	else return house;
	
    if (house == 'A') house = 'B';
	else house = 'A';
  }
}

void alloc_format()
{
  format = malloc(FORMAT_QTY * sizeof(char*));
}

void alloc_teams()
{
  TEAMS = malloc(2 * sizeof(char*));
  for (int i = 0; i < 2; ++i)
  {
    TEAMS[i] = malloc((TEAM_LEN + 1) * sizeof(char));
  }
}

void alloc_table()
{
  table = malloc((GAMES_QTY + 1) * sizeof(float*));
  for (int i = 0; i < (GAMES_QTY + 1); ++i)
  {
    table[i] = malloc((GAMES_QTY + 1) * sizeof(float));
  }
  
  for (int i = 0; i < (GAMES_QTY + 1); ++j)
  {
    for (int j = 0; j < (GAMES_QTY + 1); ++i)
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

  const gchar *cssFile = "custom.css";
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

/* * * * * *
   * Node  *
   * * * * */

void on_add_node_btn_clicked(GtkWidget *widget, Widgets *data)
{
  int error = 0;
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *label;
  char* input;
  gchar* message;

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (gtk_entry_get_text_length(GTK_ENTRY(data->text_box_2)))
  {
    input = gtk_entry_get_text(GTK_ENTRY(data->text_box_2));
    for (int i = 0; i < NODES_QTY; ++i)
    {
       if (strcmp(NODES[i],input) == 0)
       {
          error = 1;
          break;
       }
    }
  }else
  {
    error = 2;
  }
  if (error)
  {
    data->window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (data->window, data->parent);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (data->window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    if (error == 1) message = "Error: Ya existe un nodo con ese nombre.";
    else message = "Error: Ingrese un nombre para el nodo.";
    gtk_label_set_text (GTK_LABEL(label), message);
    g_object_unref(builder);
    gtk_widget_show(data->window);
    gtk_main(); 
  }else
  {
    if (NODES_QTY == 0)
    {
      alloc_nodes(1);
      alloc_D(1);
    }else
    {
      realloc_nodes();
      realloc_D();
    }
    strcpy(NODES[NODES_QTY++], input);
    g_signal_connect (G_OBJECT (data->graph), "draw", G_CALLBACK (on_graph_draw), NULL);
    char number[NUMBER_LEN];
    snprintf(number, 10, "%d", NODES_QTY);
    gtk_entry_set_text (GTK_ENTRY(data->text_box_1), number);
    close_window(data->btn, data->parent);
  }
}

void on_node_destroy(GtkWidget *widget, Widgets *data)
{
  free(data);
  gtk_main_quit();
}

/* * * * * * *
   * Edge *
   * * * * * */

void on_add_edge_btn_clicked(GtkWidget *widget, Widgets *data)
{
  int error = 0;
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *label;
  char* input;
  float weight;
  gchar* message;
  gchar *node1;
  gchar *node2;
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (gtk_entry_get_text_length(GTK_ENTRY(data->text_box_2)))
  {
    input = gtk_entry_get_text(GTK_ENTRY(data->text_box_2));
    weight = atof(input);
    if (weight <= 0) error = 3;
  }else error = 2;
  
  node1 = gtk_combo_box_text_get_active_text(data->cb_1);
  node2 = gtk_combo_box_text_get_active_text(data->cb_2);
  
  if (strcmp(node1,node2) == 0)
  {
    error = 1;
  }

  if (error)
  {
    data->window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (data->window, data->parent);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (data->window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    if (error == 1) message = "Error: Nodos no pueden ser iguales.";
    else if (error == 2) message = "Error: Ingrese un peso para la arista.";
    else message = "Error: Peso no puede ser menor que 0";
    gtk_label_set_text (GTK_LABEL(label), message);
    g_object_unref(builder);
    gtk_widget_show(data->window);
    gtk_main(); 
  }else
  {
    int i = get_index_from_table(node1);
    int j = get_index_from_table(node2);
    if (i != -1 && j != -1) D[i][j] = weight;
    g_signal_connect (G_OBJECT (data->graph), "draw", G_CALLBACK (on_graph_draw), NULL);
    close_window(data->btn, data->parent);
  }
}

void on_edge_destroy(GtkWidget *widget, Widgets *data)
{
  free(data);
  gtk_main_quit();
}

/* * * * * * *
   * Routes  *
   * * * * * */

void find_optimal_route(int node1, int node2, char path [])
{
  char next[5] = " -> ";
  int mid = P[node1][node2];

  if (mid == 0)
  {
    strcat(path, next);
    strcat(path, NODES[node2]);
  }
  else
  {
    strcat(path, next);
    strcat(path, NODES[mid - 1]);
    find_optimal_route(mid - 1, node2, path);
  }
}

void on_calc_routes_btn_clicked(GtkWidget *widget, Widgets *data)
{
  gchar *node1;
  gchar *node2;
  node1 = gtk_combo_box_text_get_active_text(data->cb_1);
  node2 = gtk_combo_box_text_get_active_text(data->cb_2);
  
  int n1 = -1;
  int n2 = -1;
  
  for (int i = 0; (i < NODES_QTY) && ((n1 == -1) || (n2 == -1)); ++i)
  {
    if (n1 == -1)
    {
      if (strcmp(node1,NODES[i]) == 0) n1 = i;
    }
    if (n2 == -1)
    {
      if (strcmp(node2,NODES[i]) == 0) n2 = i;
    }
  }
  char path[PATH_LEN];
  snprintf(path, PATH_LEN, "Distancia: %f\nRuta: %s", D[n1][n2], NODES[n1]);
  find_optimal_route(n1, n2, path);
  gtk_text_buffer_set_text(data->buffer,path,-1);
  gtk_widget_show_all(data->parent);
}

void on_routes_destroy(GtkWidget *widget, Widgets *data)
{
  free(data);
  gtk_main_quit();
}

/* * * * * * * * *
   * table_edit  *
   * * * * * * * */

void on_random_btn_clicked(GtkWidget *widget, Widgets *data)
{
  float number = 0.0;
  for(int i = 1; i < (TEMP_NODES_QTY + 1); ++i) 
  {
    for(int j = 1; j < (TEMP_NODES_QTY + 1); ++j) 
    {
      if (i != j)
      {
        number = (float)((double)rand()/(double)(RAND_MAX)) * (float)(1<<10);
        char snumber[NUMBER_LEN*2];
        snprintf(snumber, NUMBER_LEN*2, "%f", number);
        gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), snumber);
      }
    }
  }
  gtk_widget_show_all(data->parent);
}

/* Funciones auxiliares de table_edit */

int empty_node_entry(Widgets *data)
{
  for (int i = 1; i < (TEMP_NODES_QTY + 1); ++i)
  {
    if (!gtk_entry_get_text_length(GTK_ENTRY(data->table[0][i])))
    {
      return 1;
    }
  }
  return 0;
}

int wrong_weight_entry(Widgets *data)
{
  float weight = 0;
  for (int i = 1; i < (TEMP_NODES_QTY + 1); ++i)
  {
    for (int j = 1; j < (TEMP_NODES_QTY + 1); ++j)
    {
      if (i != j)
      {
        if ( gtk_entry_get_text_length( GTK_ENTRY(data->table[i][j]) ) )
        {
          char* input = gtk_entry_get_text(GTK_ENTRY(data->table[i][j]));
          if (strcmp(input,"INF") != 0)
          {
            if ( isfloat(input) || isNumeric(input) )
            {
              weight = atof(input);
              if (weight <= 0) return 1;
            }else  return 1;
          }
        }else return 1;
      }
    }
  }
  return 0;
}

int same_node_entry(Widgets *data)
{
  for (int i = 1; i < (TEMP_NODES_QTY + 1); ++i)
  {
    for (int j = i + 1; j < (TEMP_NODES_QTY + 1); ++j)
    {
       if (strcmp(gtk_entry_get_text(GTK_ENTRY(data->table[0][i])), gtk_entry_get_text(GTK_ENTRY(data->table[0][j]))) == 0)
       {
         return 1;
       }
    }
  }
  return 0;
}

void assign_nodes(Widgets *data)
{
  for (int i = 1; i < (TEMP_NODES_QTY + 1); ++i)
  {
    char *no = gtk_entry_get_text(GTK_ENTRY(data->table[0][i]));
    strcpy(NODES[i-1], no);
  }
}
void assign_weights(Widgets *data)
{
  float weight = 0;
  for (int i = 1; i < (TEMP_NODES_QTY + 1); ++i)
  {
    for (int j = 1; j < (TEMP_NODES_QTY + 1); ++j)
    {
      if (i != j)
      {
        char* input = gtk_entry_get_text(GTK_ENTRY(data->table[i][j]));
        if (strcmp(input,"INF") != 0)
        {
          weight = atof(input);
          D[i-1][j-1] = weight;
        }else
        {
          D[i-1][j-1] = INF;
        }
      }
    }
  }
}



void on_table_edit_destroy(GtkWidget *widget, Widgets *data)
{
  for(int i = 0; i < (TEMP_NODES_QTY + 1); ++i)
  {
    free(data->table[i]);
  }
  free(data->table);
  free(data);
  gtk_main_quit();
}

/* * * * * * * * *
   * table_view  *
   * * * * * * * */

void reset_table_colours(Widgets *data)
{
  for (int i = 0; i < NODES_QTY; ++i)
  {
    for (int j = 0; j < NODES_QTY; ++j)
    {
      gtk_widget_set_name (data->table[i+1][j+1],"entry");
      gtk_widget_set_name (data->table_2[i+1][j+1],"entry");
    }
  }
  
}

void on_next_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *label;
  gchar* message;
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (CURR_INDEX < NODES_QTY)
  {
    int i, j;
    float t;

    reset_table_colours(data);

    char curr[NODES_QTY_LEN + 4];
    snprintf(curr, NODES_QTY_LEN + 4, "D(%d)", CURR_INDEX+1);
    gtk_label_set_text(data->lbl_1, curr);
    curr[0] = 'P';
    gtk_label_set_text(data->lbl_2, curr);

    for (i = 0; i < NODES_QTY; ++i)
    {
      if (i != CURR_INDEX)
      {
        for (j = 0; j < NODES_QTY; ++j)
        {
          if ((j != CURR_INDEX) && (j != i))
          {
            t = D[i][CURR_INDEX] + D[CURR_INDEX][j];
            if (t < D[i][j])
            {
              P[i][j] = CURR_INDEX+1;

              char k[NODES_QTY_LEN];
              snprintf(k, sizeof(k), "%d", P[i][j]);
              gtk_entry_set_text (GTK_ENTRY(data->table_2[i+1][j+1]), k);

              D[i][j] = t;

              char weight[NODES_QTY_LEN];
              snprintf(weight, sizeof(weight), "%.3f", t);
              gtk_entry_set_text (GTK_ENTRY(data->table[i+1][j+1]), weight);

              gtk_widget_set_name (data->table[i+1][j+1],"newVal");
              gtk_widget_set_name (data->table_2[i+1][j+1],"newVal");
            }
          }
        }
      }
    }
    CURR_INDEX++;
    
    if (CURR_INDEX == NODES_QTY)
    {
      gtk_widget_set_sensitive(data->btn,FALSE);
      gtk_widget_set_sensitive(data->btn_2,FALSE);
      gtk_widget_set_sensitive(data->btn_3,TRUE);
    }
    gtk_widget_show_all(data->parent);
  }
}

void on_last_btn_clicked(GtkWidget *widget, Widgets *data)
{
  if (CURR_INDEX < NODES_QTY)
  {
    int i, j;
    float t;
    
    for (; CURR_INDEX < NODES_QTY; ++CURR_INDEX)
    {
      for (i = 0; i < NODES_QTY; ++i)
      {
        if (i != CURR_INDEX)
        {
          for (j = 0; j < NODES_QTY; ++j)
          {
            if ((j != CURR_INDEX) && (j != i))
            {
              t = D[i][CURR_INDEX] + D[CURR_INDEX][j];
              if (t < D[i][j])
              {
                P[i][j] = CURR_INDEX+1;

                char k[NODES_QTY_LEN];
                snprintf(k, sizeof(k), "%d", P[i][j]);
                gtk_entry_set_text (GTK_ENTRY(data->table_2[i+1][j+1]), k);

                D[i][j] = t;

                char weight[NODES_QTY_LEN];
                snprintf(weight, sizeof(weight), "%.3f", t);
                gtk_entry_set_text (GTK_ENTRY(data->table[i+1][j+1]), weight);
              }
            }
          }
        }
      }
    }
    
    char curr[NODES_QTY_LEN + 4];
    snprintf(curr, NODES_QTY_LEN + 4, "D(%d)", CURR_INDEX);
    gtk_label_set_text(data->lbl_1, curr);
    curr[0] = 'P';
    gtk_label_set_text(data->lbl_2, curr);
    
    gtk_widget_set_sensitive(data->btn,FALSE);
    gtk_widget_set_sensitive(data->btn_2,FALSE);
    gtk_widget_set_sensitive(data->btn_3,TRUE);
    gtk_widget_show_all(data->parent);
  }
}

void on_route_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *exit;
  GtkWidget *entry;
  GtkWidget *window;
  Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "routes"));
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_routes_destroy), widgets);

  widgets->parent = window;
  gtk_builder_connect_signals(builder, NULL);
  
  gtk_window_set_transient_for (widgets->parent, data->parent);
  
  widgets->cb_1 = GTK_WIDGET(gtk_builder_get_object(builder, "routes_node1_cb"));

  widgets->cb_2 = GTK_WIDGET(gtk_builder_get_object(builder, "routes_node2_cb"));

  for (int i = 0; i < NODES_QTY; ++i)
  {
    char id[NUMBER_LEN];
    sprintf(id, "%d", i);
    gtk_combo_box_text_append(widgets->cb_1, id, NODES[i]);
    gtk_combo_box_text_append(widgets->cb_2, id, NODES[i]);
  }
  if (NODES_QTY > 0)
  {
    gtk_combo_box_set_active(widgets->cb_1, 0);
    gtk_combo_box_set_active(widgets->cb_2, 0);
  }

  widgets->buffer = gtk_builder_get_object(builder, "route_text_buffer");

  widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "calc_routes_btn"));
  g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_calc_routes_btn_clicked), widgets);

  exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_routes_btn"));
  g_signal_connect (G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);
  g_object_unref(builder);
 
  gtk_widget_show(widgets->parent);
  gtk_main();
}

void on_table_view_destroy(GtkWidget *widget, Widgets *data)
{
  for(int i = 0; i < (NODES_QTY + 1); ++i)
  {
    free(data->table[i]);
    free(data->table_2[i]);
  }
  free(data->table);
  free(data->table_2);
  free(data);

  for (int i = 0; i < NODES_QTY; ++i)
  {
    free(P[i]);
  }
  free(P);

  restore_d_values();
  gtk_main_quit();
}

/* * * * * * *
   * Display *
   * * * * * */

void fill_d_table(Widgets *data, GtkWidget *column, int type)
{
  for(int i = 0; i < (TEMP_NODES_QTY + 1); ++i) 
  {
    for(int j = 0; j < (TEMP_NODES_QTY + 1); ++j) 
    {
      if (i != 0 || j != 0)
      {
        /* INIT */
        data->table[i][j] = gtk_entry_new();
        gtk_widget_set_name (data->table[i][j],"entry");
        gtk_entry_set_width_chars(GTK_ENTRY(data->table[i][j]),12);
        if ( (type == 1) || (type == 2))
        {
          gtk_widget_set_sensitive(data->table[i][j],TRUE);
          gtk_widget_set_tooltip_text(data->table[i][j],"INF : valor infinito");
        }else if (type == 3)
        {
          gtk_editable_set_editable(GTK_ENTRY(data->table[i][j]),FALSE);
        }
        gtk_grid_attach (GTK_GRID (column),data->table[i][j] , j, i, 1, 1);
        
        /* FIRST ROW : NODES NAMES */
        if (i == 0)
        {
          if (type == 1)
          {
            char node_name[NODE_LEN];
            get_node_name(j-1, node_name);
            gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), node_name);
          } else
          {
            gtk_entry_set_text(GTK_ENTRY(data->table[i][j]),NODES[j-1]);
          }
        /* FIRST COL : NODES NAMES */
        }else if (j == 0)
        {
          if ((type == 1) || (type == 2)) gtk_widget_set_sensitive(data->table[i][j], FALSE);
          if (type == 1)
          {
            char node_name[NODE_LEN];
            get_node_name(i-1, node_name);
            gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), node_name);
          } else
          {
            gtk_entry_set_text(GTK_ENTRY(data->table[i][j]),NODES[i-1]);
          }
        /* DIAGONAL : DIRECT : 0 */
        }else if (i == j)
        {
          gtk_entry_set_text (GTK_ENTRY(data->table[i][j]),"0");
          gtk_widget_set_sensitive(data->table[i][j], FALSE);
        /* REST */
        }else
        {
          if (type == 1)
          {
            gtk_entry_set_text (GTK_ENTRY(data->table[i][j]),"INF");
          } else
          {
            if (D[i-1][j-1] == INF)
            {
              gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), "INF");
            }else
            {
              char weight[NODES_QTY_LEN];
              snprintf(weight, sizeof(weight), "%.3f", D[i-1][j-1]);
              gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), weight);
            }
          }
        }
      }
    }
  }
}

void fill_p_table(Widgets *data, GtkWidget *column)
{
  for(int i = 0; i < (TEMP_NODES_QTY + 1); ++i) 
  {
    for(int j = 0; j < (TEMP_NODES_QTY + 1); ++j) 
    {
      if (i != 0 || j != 0)
      {
        /* INIT */
        data->table_2[i][j] = gtk_entry_new();
        gtk_widget_set_name (data->table_2[i][j],"entry");
        gtk_entry_set_width_chars(GTK_ENTRY(data->table_2[i][j]),12);
        gtk_editable_set_editable(GTK_ENTRY(data->table_2[i][j]),FALSE);
        gtk_grid_attach (GTK_GRID (column),data->table_2[i][j] , j, i, 1, 1);
        
        /* FIRST ROW : NODES NAMES */
        if (i == 0)
        {
          gtk_entry_set_text(GTK_ENTRY(data->table_2[i][j]),NODES[j-1]);
        /* FIRST COL : NODES NAMES */
        }else if (j == 0)
        {
          gtk_entry_set_text(GTK_ENTRY(data->table_2[i][j]),NODES[i-1]);
        /* DIAGONAL : DIRECT : 0 */
        }else
        {
          char weight[NODES_QTY_LEN];
          snprintf(weight, sizeof(weight), "%d", P[i-1][j-1]);
          gtk_entry_set_text (GTK_ENTRY(data->table_2[i][j]), weight);
        }
      }
    }
  }
}

void on_new_btn_clicked(GtkWidget *widget, Widgets *data)
{
  int error = 1;
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
  GtkWidget *cancel;
  GtkWidget *label;
  GtkWidget *random;
  GtkWidget *column;
  GtkWidget *scroll;

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (gtk_entry_get_text_length(GTK_ENTRY(data->text_box_1)))
  {
    TEMP_NODES_QTY = (int) strtol(gtk_entry_get_text(GTK_ENTRY(data->text_box_1)), (char **)NULL, 10);
    if (TEMP_NODES_QTY > 0)
    {
      Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
      window = GTK_WIDGET(gtk_builder_get_object(builder, "table_edit"));
      g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_table_edit_destroy), widgets);
      widgets->parent = window;
      gtk_builder_connect_signals(builder, NULL);
      gtk_window_set_transient_for (widgets->parent, data->parent);

      widgets->graph = data->graph;
      widgets->text_box_2 = GTK_WIDGET(gtk_builder_get_object(builder, "nodes_display_tb"));

      widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "accept_btn"));
      g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_accept_btn_clicked), widgets);

      widgets->btn_2 = GTK_WIDGET(gtk_builder_get_object(builder, "random_btn"));
      g_signal_connect (G_OBJECT (widgets->btn_2), "clicked", G_CALLBACK (on_random_btn_clicked), widgets);

      cancel = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_btn"));
      g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);
      scroll = GTK_WIDGET(gtk_builder_get_object(builder, "table_edit_scrolled"));

      widgets->table = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget**));

      for(int i = 0; i < (TEMP_NODES_QTY + 1); ++i)
      {
        widgets->table[i] = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget*));
      }

      column = gtk_grid_new ();
      gtk_container_add (GTK_CONTAINER (scroll), column);

      fill_d_table(widgets, column, 1);
      error = 0;
    }
  }
  
  if (error)
  {
    window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_window_set_transient_for (window, data->parent);
    gtk_builder_connect_signals(builder, NULL);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    const gchar* message = "Ingrese una cantidad de nodos mayor a 0.";
    gtk_label_set_text (GTK_LABEL(label), message);
  }				  
  g_object_unref(builder);
  gtk_widget_show_all(window);
  gtk_main();
}

void on_calc_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
  GtkWidget *exit;
  GtkWidget *label;
  GtkWidget *column_D;
  GtkWidget *column_P;
  GtkWidget *scroll_D;
  GtkWidget *scroll_P;

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (NODES_QTY)
  {
    TEMP_NODES_QTY = NODES_QTY; /* fill_d_table and fill_p_table uses TEMP_NODES_QTY */
    CURR_INDEX = 0;
    Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "table_view"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_table_view_destroy), widgets);
    widgets->parent = window;
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);

    widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "next_btn"));
    g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_next_btn_clicked), widgets);

    widgets->btn_2 = GTK_WIDGET(gtk_builder_get_object(builder, "last_btn"));
    g_signal_connect (G_OBJECT (widgets->btn_2), "clicked", G_CALLBACK (on_last_btn_clicked), widgets);

    widgets->btn_3 = GTK_WIDGET(gtk_builder_get_object(builder, "route_btn"));
    g_signal_connect (G_OBJECT (widgets->btn_3), "clicked", G_CALLBACK (on_route_btn_clicked), widgets);
    gtk_widget_set_sensitive(widgets->btn_3,FALSE);

    exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_table_view_btn"));
    g_signal_connect (G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);

    widgets->lbl_1 = GTK_WIDGET(gtk_builder_get_object(builder, "d_table"));
    widgets->lbl_2 = GTK_WIDGET(gtk_builder_get_object(builder, "p_table"));
    gtk_label_set_text(widgets->lbl_1, "D(0)");
    gtk_label_set_text(widgets->lbl_2, "P(0)");

    scroll_D = GTK_WIDGET(gtk_builder_get_object(builder, "table_view_d_scrolled"));
    scroll_P = GTK_WIDGET(gtk_builder_get_object(builder, "table_view_p_scrolled"));

    widgets->table = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget**));
    widgets->table_2 = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget**));

    for(int i = 0; i < (TEMP_NODES_QTY + 1); ++i)
    {
      widgets->table[i] = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget*));
      widgets->table_2[i] = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget*));
    }

    column_D = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (scroll_D), column_D);

    column_P = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (scroll_P), column_P);

    fill_d_table(widgets, column_D, 3);
    backup_d_values();

    alloc_P(NODES_QTY);
    fill_p_table(widgets, column_P);

  }else
  {
    window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_builder_connect_signals(builder, NULL);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    const gchar* message = "Se necesita crear o cargar un grafo primero.";
    gtk_label_set_text (GTK_LABEL(label), message);
  }
				  
  g_object_unref(builder);
  gtk_widget_show_all(window);
  gtk_main();
}

void on_edit_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
  GtkWidget *cancel;
  GtkWidget *label;
  GtkWidget *random;
  GtkWidget *column;
  GtkWidget *scroll;

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (NODES_QTY)
  {
    Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
    TEMP_NODES_QTY = NODES_QTY;
    window = GTK_WIDGET(gtk_builder_get_object(builder, "table_edit"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_table_edit_destroy), widgets);
    widgets->parent = window;
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);

    widgets->graph = data->graph;
    widgets->text_box_2 = GTK_WIDGET(gtk_builder_get_object(builder, "nodes_display_tb"));

    widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "accept_btn"));
    g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_accept_btn_clicked), widgets);

    widgets->btn_2 = GTK_WIDGET(gtk_builder_get_object(builder, "random_btn"));
    g_signal_connect (G_OBJECT (widgets->btn_2), "clicked", G_CALLBACK (on_random_btn_clicked), widgets);

    cancel = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_btn"));
    g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);
    scroll = GTK_WIDGET(gtk_builder_get_object(builder, "table_edit_scrolled"));

    widgets->table = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget**));

    for(int i = 0; i < (TEMP_NODES_QTY + 1); ++i)
    {
      widgets->table[i] = calloc(TEMP_NODES_QTY + 1, sizeof(GtkWidget*));
    }

    column = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (scroll), column);

    fill_d_table(widgets, column, 2);
  }else
  {
    window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_window_set_transient_for (window, data->parent);
    gtk_builder_connect_signals(builder, NULL);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    const gchar* message = "Se necesita crear o cargar un grafo primero.";
    gtk_label_set_text (GTK_LABEL(label), message);
  }				  
  g_object_unref(builder);
  gtk_widget_show_all(window);
  gtk_main();
}

void on_edge_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *cancel;
  GtkWidget *entry;
  GtkWidget *window;
  Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "edge"));
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_edge_destroy), widgets);

  widgets->parent = window;
  gtk_builder_connect_signals(builder, NULL);
  
  gtk_window_set_transient_for (widgets->parent, data->parent);
  
  widgets->cb_1 = GTK_WIDGET(gtk_builder_get_object(builder, "edge_node1_cb"));

  widgets->cb_2 = GTK_WIDGET(gtk_builder_get_object(builder, "edge_node2_cb"));

  for (int i = 0; i < NODES_QTY; ++i)
  {
    char id[NUMBER_LEN];
    sprintf(id, "%d", i);
    gtk_combo_box_text_append(widgets->cb_1, id, NODES[i]);
    gtk_combo_box_text_append(widgets->cb_2, id, NODES[i]);
  }
  if (NODES_QTY > 0)
  {
    gtk_combo_box_set_active(widgets->cb_1, 0);
    gtk_combo_box_set_active(widgets->cb_2, 0);
  }

  widgets->graph = data->graph;
  widgets->text_box_2 = GTK_WIDGET(gtk_builder_get_object(builder, "weight_tb"));

  widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "add_edge_btn"));
  g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_add_edge_btn_clicked), widgets);

  cancel = GTK_WIDGET(gtk_builder_get_object(builder, "exit_edge_btn"));
  g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);
  g_object_unref(builder);
 
  gtk_widget_show(widgets->parent);
  gtk_main();
}

void on_node_btn_clicked(GtkWidget *widget, Widgets *data)
{

  GtkBuilder *builder;
  GtkWidget *cancel;
  GtkWidget *entry;
  GtkWidget *window;
  Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "node"));
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_node_destroy), widgets);

  widgets->parent = window;
  gtk_builder_connect_signals(builder, NULL);
  
  gtk_window_set_transient_for (widgets->parent, data->parent);
  
  widgets->graph = data->graph;
  widgets->text_box_1 = data->text_box_1;
  widgets->text_box_2 = GTK_WIDGET(gtk_builder_get_object(builder, "node_tb"));

  widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "add_node_btn"));
  g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_add_node_btn_clicked), widgets);

  cancel = GTK_WIDGET(gtk_builder_get_object(builder, "exit_node_btn"));
  g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);
  g_object_unref(builder);
 
  gtk_widget_show(widgets->parent);
  gtk_main();
}

/* * * * * * *
   * Config  *
   * * * * * */
   
void on_accept_config_btn_clicked(GtkWidget *widget, Widgets *data)
{
  int error = 0;
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *exit;
  GtkWidget *label;
  GtkWidget *team_a;
  GtkWidget *team_b;
  gchar* message = NULL;
  char* team_A_name;
  char* team_B_name;
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, GLADE, NULL);

  if (gtk_entry_get_text_length(GTK_ENTRY(data->tb_1)))
  {
    if (isNumeric(data->tb_1))
    {
      BoN = (int) strtol(gtk_entry_get_text(GTK_ENTRY(data->tb_1)), (char **)NULL, 10);
      if (BoN <= 0) message = "Cantidad de juegos tiene que ser superior a 0.";
    }else message = "Valor ingresado para la cantidad de juegos no es un valor numerico.";
  }else message = "No ingreso la cantidad de juegos en la serie";

  if (!message)
  {
    if (gtk_entry_get_text_length(GTK_ENTRY(data->tb_2)))
    {
      team_A_name = gtk_entry_get_text(GTK_ENTRY(data->tb_2));
    }else message = "Ingrese un nombre para el equipo A";
  }

  if (!message)
  {
    if (gtk_entry_get_text_length(GTK_ENTRY(data->tb_3)))
    {
      team_B_name = gtk_entry_get_text(GTK_ENTRY(data->tb_3));
    }else message = "Ingrese un nombre para el equipo B";
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
    clear();
    alloc_teams();
    strcpy(TEAMS[0], team_A_name);
    strcpy(TEAMS[1], team_B_name);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "format"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_format_destroy), widgets);
    widgets->parent = window;
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);
  
    team_a = GTK_WIDGET(gtk_builder_get_object(builder, "team_a_format_lbl"));
    gtk_label_set_text (GTK_LABEL(team_a), TEAMS[0]);
    team_b = GTK_WIDGET(gtk_builder_get_object(builder, "team_b_format_lbl"));
    gtk_label_set_text (GTK_LABEL(team_b), TEAMS[1]);

    widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "accept_format_btn"));
    g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_accept_format_btn_clicked), widgets);

    exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_format_btn"));
    g_signal_connect (G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);
    g_object_unref(builder);
    gtk_widget_show(widgets->parent);
  }
  
  gtk_main();
}

void on_config_destroy(GtkWidget *widget, Widgets *data)
{
  free(data);
  clear();
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

/* * * * * 
   * Set *
   * * * */

void on_set_ph_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *label;
  gchar* message;

  if (gtk_entry_get_text_length(GTK_ENTRY(data->tb_4)))
  {
	  if (isfloat(GTK_ENTRY(data->tb_4)))
	  {
      float _ph = (float) strtof(gtk_entry_get_text(GTK_ENTRY(data->tb_4)), (char **)NULL);
      if (_ph > 0 && _ph < 1)
      {
        PH = _ph;
        _ph = 1 - _ph;
	      char ph_s[FLOAT_LEN];
	      snprintf(ph_s, sizeof(ph_s), "%.3f", ph);
	      gtk_entry_set_text (GTK_ENTRY(data->lbl_1), ph_s);
	      gtk_widget_show_all(data->parent);
	    }else message = "La probabilidad de que el equipo A gane debe estar entre 0.0 y 1.0.";
	  }else message = "El valor ingresado en la probabilidad de que el equipo A gane en casa no es un valor flotante.";
  }else message = "No hay datos en el campo de la probabilidad de que el equipo A gane en casa.";

  if (message)
  {
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, GLADE, NULL);
	  window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_window_set_transient_for (window, data->parent);
    gtk_builder_connect_signals(builder, NULL);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    gtk_label_set_text (GTK_LABEL(label), message);
	  g_object_unref(builder);
	  gtk_widget_show_all(window);
    gtk_main();
  }
}   

void on_set_pr_btn_clicked(GtkWidget *widget, Widgets *data)
{
  GtkBuilder *builder;
  GtkWidget *accept;
  GtkWidget *label;
  gchar* message;
  int error = 0;

  if (gtk_entry_get_text_length(GTK_ENTRY(data->tb_4)))
  {
    if (isfloat(GTK_ENTRY(data->tb_4)))
    {
      float _pr = (float) strtof(gtk_entry_get_text(GTK_ENTRY(data->tb_5)), (char **)NULL);
      if (_pr > 0 && _pr < 1)
      {
        PR = _pr;
        _pr = 1 - _pr;
        char pr_s[FLOAT_LEN];
        snprintf(pr_s, sizeof(pr_s), "%.3f", _pr);
        gtk_entry_set_text (GTK_ENTRY(data->lbl_2), pr_s);
        gtk_widget_show_all(data->parent);
      }else error = 2;
    }else error = 3;
  }else error = 1;

  if (error)
  {
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, GLADE, NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    gtk_window_set_transient_for (window, data->parent);
    gtk_builder_connect_signals(builder, NULL);
    accept = GTK_WIDGET(gtk_builder_get_object(builder, "message_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "message_lbl"));
    if (error == 1)
    {
      const gchar* message = "No hay datos en el campo de la probabilidad de que el equipo A gane de visita.";
      gtk_label_set_text (GTK_LABEL(label), message);
    }else if (error = 2)
    {
      const gchar* message = "El valor ingresado en la probabilidad de que el equipo A gane de visita no es un valor flotante.";
      gtk_label_set_text (GTK_LABEL(label), message);
    }else if (error = 3)
    {
      const gchar* message = "La probabilidad de que el equipo A gane debe estar entre 0.0 y 1.0.";
      gtk_label_set_text (GTK_LABEL(label), message);
    }
    g_object_unref(builder);
    gtk_widget_show_all(window);
    gtk_main();
  }
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

  GtkWidget *set_ph = NULL;
  GtkWidget *set_pr = NULL;

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

  widgets->tb_1 = GTK_WIDGET(gtk_builder_get_object(builder, "best_of_tb"));
  widgets->tb_2 = GTK_WIDGET(gtk_builder_get_object(builder, "team_a_tb"));
  widgets->tb_3 = GTK_WIDGET(gtk_builder_get_object(builder, "team_b_tb"));
  widgets->tb_4 = GTK_WIDGET(gtk_builder_get_object(builder, "ph_tb"));
  widgets->tb_5 = GTK_WIDGET(gtk_builder_get_object(builder, "pr_tb"));

  widgets->lbl_1 = GTK_WIDGET(gtk_builder_get_object(builder, "qr_lbl"));
  widgets->lbl_2 = GTK_WIDGET(gtk_builder_get_object(builder, "qh_lbl"));

  set_ph = GTK_WIDGET(gtk_builder_get_object(builder, "set_ph_btn"));
  g_signal_connect(G_OBJECT (set_ph), "clicked", G_CALLBACK (on_set_ph_btn_clicked), widgets);

  set_pr = GTK_WIDGET(gtk_builder_get_object(builder, "set_pr_btn"));
  g_signal_connect(G_OBJECT (set_pr), "clicked", G_CALLBACK (on_set_pr_btn_clicked), widgets);

  accept = GTK_WIDGET(gtk_builder_get_object(builder, "continue_btn"));
  g_signal_connect(G_OBJECT (accept), "clicked", G_CALLBACK (on_accept_config_btn_clicked), widgets);

  exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_btn"));
  g_signal_connect(G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);

  g_object_unref(builder);
  gtk_widget_show(window);
  gtk_main();
  
  return 0;
}
