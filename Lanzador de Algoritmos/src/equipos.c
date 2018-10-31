#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

static const char GLADE[] = "equipos.glade";

typedef struct Widgets {
  GtkWindow *window;
  GtkWindow *parent;
  GtkSpinButton *sb_1;
  GtkSpinButton *sb_2;
  GtkSpinButton *sb_3;
  GtkWidget ***table;
  GtkWidget ***table_result;
  GtkButton *btn_1;
  GtkButton *btn_2;
  GtkButton *btn_3;
  GtkLabel *lbl_1;
  GtkDrawingArea *graph;

  GtkEntry *tb_1;
  GtkEntry *tb_2;
  GtkLabel *lbl_2;
} Widgets;

typedef struct  {
  int year; 
  int sale;
  int maintenance;
} Objects;

typedef struct  {
  int year[31];
  int value;  
  int position;
  int profit;
} ObjectsResult;

typedef struct {
  int value;
  int year;
} Candidate;

typedef struct {
  int year[30];
  int position;
} Plans;

struct Node {
  int coorX;
  int coorY;
  int id;
  int isResult;
};

Objects *initialData;
ObjectsResult *finalData;
Plans posiblePlans[300];
GtkLabel *lbl_file = NULL;

int lifetime = 0;
int price = 0;
int project_time = 0;
int counter_plans = 0;
int counter_x = 0;
int file = 0;

int** values;
int** nodes;

void close_window(GtkWidget *widget, gpointer window);

void alloc_values() {

  values = calloc(lifetime, sizeof(int*));

  for(int i = 0; i < lifetime; i++)
  {
    values[i] = calloc(3, sizeof(int));
  }
}

void alloc_node() {

  nodes = calloc(counter_plans, sizeof(int*));

  for(int i = 0; i < counter_plans; i++)
  {
    nodes[i] = calloc(project_time, sizeof(int));
  }
}

int isNumeric (const char *s) {

  if(s == NULL || *s == '\0' || isspace(*s)) {

    return 0;
  }

  char * p;
  strtod(s, &p);

  return *p == '\0';
}

void loadFileBetter(char *filename) {

  file = 1;

  int i, k = 0;
  int error = 0;
  size_t len = 0;
  ssize_t read;
  char * line = NULL;
  char *token, *str, *tofree;
  FILE *fp = fopen(filename, "r");
  
  if(fp) {

    while((read = getline(&line, &len, fp)) != -1)  {
        
      if(line[read - 1] == '\n') {

        line[read - 1] = '\0';
        --len;
      }

      if(k == 0) {

        i = 0;
        tofree = str = strdup(line);

        while((token = strsep(&str, ",")) && (error != 1)) {

          if(i == 0) {

            if(isNumeric(token)) {

              int temp_qty = (int) strtol(token, (char **)NULL, 10);
              
              if(temp_qty > 0) {

                lifetime = temp_qty;
                alloc_values();
              }
              else {

                error = 1;
                printf("Error: La cantidad de nodos tiene que ser mayor a 0.\n");
              }
            }
            else {

              error = 1;
              printf("Error: Cantidad de nodos no es un valor numerico.\n");
            }
          }
          else if (i == 1) {

            if (isNumeric(token)) {

              int temp_qty = (int) strtol(token, (char **)NULL, 10);

              if (temp_qty > 0) {

                price = temp_qty;
              }
              else {

                error = 1;
                printf("Error: La cantidad de nodos tiene que ser mayor a 0.\n");
              }
            }
            else {

              error = 1;
              printf("Error: Cantidad de nodos no es un valor numerico.\n");
            }
          }
          else if (i == 2) {

            if (isNumeric(token)) {

              int temp_qty = (int) strtol(token, (char **)NULL, 10);

              if (temp_qty > 0) {

                project_time = temp_qty;
              }
              else {

                error = 1;
                printf("Error: La cantidad de nodos tiene que ser mayor a 0.\n");
              }
            }
            else {

              error = 1;
              printf("Error: Cantidad de nodos no es un valor numerico.\n");
            }
          }
          else
          {
            error = 1;
            printf("Error: Solo puede haber un valor en la primera linea del archivo; la cantidad de nodos\n");
          }

          i++;
        }

        free(tofree);
      }
      else {

        if (k >= (lifetime + 1)) {

          error = 1;
          printf("Warning: El archivo contiene lineas adicionales. Ignorandolas.\n");
        }
        else {

          i = 0;
          tofree = str = strdup(line);

          while ((token = strsep(&str, ",")) && (error != 1)) {

            if (i < lifetime) {

              if (isNumeric(token)) {

                values[k - 1][i] = atof(token);
              }
              else {

                error = 1;
                printf("Error: Peso en la arista no es un valor numerico.\n");
              }
            }
            else {
              error = 1;
              printf("Error: La cantidad de nodos definida en la linea %d excede la cantidad [N = %d] definida en la primer linea.\n", k+1, lifetime);
            }

            i++;
          }

          free(tofree);
        }
      }

      if(error)
      {
        if (k == 0) lifetime = 0;
        if (k == 1) price = 0;
        if (k == 2) project_time = 0;
        printf("Se produjo un error leyendo los contenidos del archivo. Reviselo y vuelva a intentar.\n");
        break;
      }

      k++;
    }

    fclose(fp);
    
    if(line) {

      free(line);
    }
  }

  gtk_widget_set_name (lbl_file, "load_label");
}

void saveFile(char *filename) {

  FILE *fp = fopen(filename, "w");

  if(fp) {

    fprintf(fp, "%d,%d,%d\n", lifetime, price, project_time);

    for (int i = 0; i < lifetime; i++) {

      fprintf(fp, "%d,%d,%d\n", initialData[i].year, initialData[i].sale, initialData[i].maintenance);
    }

    printf("Archivo guardado correctamente en: %s\n", filename);
    fclose(fp);
  }
  else {

    printf("Error al guardar el archivo\n");
  }
}

void on_menu_open_button_press_event() {

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", 
                    GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
 
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *filename;
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      loadFileBetter(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_menu_save_as_button_press_event() {

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

void on_result_destroy(GtkWidget *widget, Widgets *data) {

  for(int i = 0; i < (lifetime + 1); i++)
  {
    free(data->table_result[i]);
  }

  for(int i = 0; i < (counter_plans + 1); i++)
  {
    free(nodes[i]);
  }

  free(data->table_result);
  free(nodes);
  free(initialData);

  price = 0;
  project_time = 0;
  counter_plans = 0;
  counter_x = 0;

  gtk_main_quit();
}

static gboolean on_graph_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {

  /*for(int i = 0; i < counter_plans; i++){
    for(int j = 0; j < project_time; j++){
      printf("%d ", nodes[i][j]);
    }
    printf("\n");
  }*/

  do_drawing(cr, widget);
  
  return FALSE;
}

int posById(struct Node *array, int id){
  
  for(int i = 0; i < project_time + 1; i++){
    
    //printf("%d == %d\n",array[i].id,id);
    if(array[i].id == id){
      
      return i;
    }
  }
  
  return -1;
}

void do_drawing(cairo_t *cr, GtkWidget *widget) {
  
  struct Node nodos[project_time + 1];
  GdkRGBA color;
  int width = gtk_widget_get_allocated_width(widget);
  int height = gtk_widget_get_allocated_height(widget);
  int size = 25;

  for(int j = 0; j < project_time + 1; j++) {

    nodos[j].coorX = (width / (project_time + 1)) * j + size;
    nodos[j].coorY = height / 2;
    nodos[j].id = j;
    nodos[j].isResult = 0;
  }
    
  //Ciclo que dibuja las líneas
  for(int i = 0; i < counter_plans; i++) {

    double color1 = (double)rand() / (double)RAND_MAX;
    double color2 = (double)rand() / (double)RAND_MAX;
    double color3 = (double)rand() / (double)RAND_MAX;
    
    for(int j = 0; j < project_time-1; j++) {
      
      //nodos[i][j].coorY = nodos[i][j].coorY * (i+1);
      int origin = posById(nodos, nodes[i][j]);
      int Destination = posById(nodos, nodes[i][j + 1]);
      cairo_set_source_rgb(cr, color1, color2, color3);

      if(origin != -1 && Destination != -1 && Destination != 0) {

        //printf("Origin: %d Destination: %d\n",origin,Destination );
        //double distance = getDistance(nodos[origin],nodos[Destination]);
        cairo_set_line_width(cr, 1);
        cairo_move_to(cr, nodos[origin].coorX, nodos[origin].coorY);
        cairo_curve_to(cr, nodos[origin].coorX, nodos[origin].coorY, 
                      (nodos[origin].coorX + nodos[Destination].coorX) / 2, 
                      nodos[origin].coorY / 2 * i, nodos[Destination].coorX,
                      nodos[Destination].coorY);
        //cairo_curve_to(cr,150,50,160,40,300,100);
        //cairo_line_to(cr,aux.Destination.coorX,aux.Destination.coorY);
        cairo_stroke(cr);
      }
    }
  }
  
  //ciclo que dibuja los círculos
  for(int j = 0; j < project_time + 1; j++) {

    cairo_arc(cr, nodos[j].coorX , nodos[j].coorY, size, 0, 2 * G_PI);
    gtk_style_context_get_color(gtk_widget_get_style_context (widget), 0, &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_fill(cr);
  }

  //ciclo que dibuja los números
  for(int j = 0; j < project_time + 1; j++) {

    char buf[50];
    cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 10);
    cairo_set_source_rgb(cr, 1, 1, 1); 
    cairo_move_to(cr, nodos[j].coorX - 5, nodos[j].coorY);
    gcvt(nodos[j].id, 6, buf);
    cairo_text_path(cr, buf);
  }

  cairo_clip(cr);
  cairo_paint_with_alpha(cr, 10);
}

void on_draw_btn_clicked(GtkWidget *widget, Widgets *data) {

  int error = 1;
  GtkBuilder *builder;
  GtkWidget *window;

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, GLADE, NULL);

  Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
  window = GTK_WIDGET(gtk_builder_get_object(builder, "draw"));
  widgets->parent = window;
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (close_window), widgets->parent);

  gtk_builder_connect_signals(builder, NULL);
  gtk_window_set_transient_for (widgets->parent, data->parent);

  widgets->graph = GTK_WIDGET(gtk_builder_get_object(builder, "graph"));
  g_signal_connect (G_OBJECT (widgets->graph), "draw", G_CALLBACK (on_graph_draw), NULL);

  g_object_unref(builder);
  gtk_widget_show_all(window);
  gtk_main();
}

void createObjects(Widgets *data){

  initialData = calloc(lifetime, sizeof(Objects));
  Objects object;

  for (int i = 1; i <= lifetime; i++)
  {
    object.year = atoi(gtk_entry_get_text(GTK_ENTRY(data->table[i][0])));
    object.sale = atoi(gtk_entry_get_text(GTK_ENTRY(data->table[i][1])));
    object.maintenance = atoi(gtk_entry_get_text(GTK_ENTRY(data->table[i][2])));

    initialData[i-1] = object;
  }
}

void replaceAlgorithm(Widgets *data, ObjectsResult *finalData) {
  
  int cost[lifetime];
  int mantain = 0;
  
  for (int i = 0; i < lifetime; i++) {

    for (int range = 0; range <= i; range++) {
      mantain = mantain + initialData[range].maintenance;
    }

    cost[i] = price + mantain - initialData[i].sale;
    mantain = 0;
  }

  for (int i = project_time; i >= 0; i--){

    int min = 0;
    int flag = 0;

    Candidate options_result[lifetime];
    Candidate options;

    options.value = 0;
    options.year = i;

    ObjectsResult result;

    for (int x = (i + 1); x <= project_time; x++) {

      if (flag < (lifetime)){

        options.value = cost[(x - i) - 1] + finalData[x].value;
        options.year = x;
        options_result[flag] = options;
        flag++;
      }
    }

    if (flag == 0) {

      result.value = options.value;
      result.year[0] = options.year;
      result.position = 0;
      result.profit = price;
    }

    for (int i = 0; i < flag; i++){

      Candidate aux;
      aux = options_result[i];

      if (i == 0){

        min = aux.value;
        result.year[0] = aux.year;
        result.position = 1;
      }
      else{

        if (aux.value == min){

          result.year[result.position] = aux.year;
          result.position++;
        }

        if (aux.value < min){

          result.year[0] = aux.year;
          result.position = 1;
          min = aux.value;
        }
      }

      result.value = min;
      result.profit = price - min;
    }

    finalData[i] = result;
  }
}

void show_result(ObjectsResult *finalData){

  int stop = 0;
  int counter = 0;
   
  Plans plans;
  plans.position = 0;
  plans.year[0] = 0;

  ObjectsResult result = finalData[counter];

  while(stop == 0) {

    if (result.position == 0) {

      stop = 1;
    }

    else{

      plans.position = plans.position + 1;
      plans.year[plans.position] = result.year[result.position - 1]; 
      
      result = finalData[result.year[result.position - 1]];
    }
  }

  posiblePlans[counter_x] = plans;
  counter_plans++;
  counter_x ++;
}

void plans(ObjectsResult *finalData){

  for (int i = 0; i <= project_time; i++){

    if (finalData[i].position > 1) {

      for (int x = finalData[i].position;x>=1;x--) {

        show_result(finalData);
        finalData[i].position = finalData[i].position - 1;

        if (finalData[i].position == 0){

          finalData[i].position = 1;
        }
      }
    }
  }
  counter_plans = counter_x;
}

int repeat(Plans posiblePlans[300], int index){

  int result = 0;
  int coincidence = 0;

  if (index > 0) {

    for (int i = 0; i < index; i++) {

      if (posiblePlans[i].position == posiblePlans[index].position) {

        for (int x = 0; x < posiblePlans[index].position; x++) {

          if (posiblePlans[index].year[x] == posiblePlans[i].year[x]) {

            result = 1;
            coincidence ++;
          }
          else {
            result = 0;
            coincidence = 0;
          }
        }
      }
    }
  }

  if (result == 1 && coincidence == (posiblePlans[index].position + 1)) {

    return 1;
  }
  else {

    return 0;
  }
}

void createOptimalSolution(Widgets *data, int counter_plans) {

  alloc_node();

  char text[1000];
  char number[3];
  strcpy(text, " ");

  for (int i = 0; i < counter_plans; i++) {

    Plans aux = posiblePlans[i];

    if (repeat(posiblePlans, i) ==0) {

      for (int x = 0; x <= aux.position; x++) {

        sprintf(number, "%d", aux.year[x]);
        strcat(text, number);

        if (x < aux.position){

          strcat(text," -> ");
        }

        else{
          strcat(text," \n");
        }

        nodes[i][x] = aux.year[x];
      }
    }
  }
  
  gtk_label_set_text(data->lbl_1, text);
}

void createFinalTable(Widgets *data, GtkWidget *column, ObjectsResult *finalData) {

  for(int i = 0; i < (project_time + 2); ++i)
  {

    for(int j = 0; j < 4; j++)
    {

      if(i == 0)
      {

        switch(j)
        {
          case 0:
            data->table_result[i][j] = gtk_label_new("t");
            gtk_widget_set_name(data->table_result[i][j],"label");
            break;

          case 1:
            data->table_result[i][j] = gtk_label_new("G(t)");
            gtk_widget_set_name(data->table_result[i][j],"label");
            break;

          case 2:
            data->table_result[i][j] = gtk_label_new("Próximo");
            gtk_widget_set_name(data->table_result[i][j],"label");
            break;

          case 3:
            data->table_result[i][j] = gtk_label_new("Ganancia");
            gtk_widget_set_name(data->table_result[i][j],"label");
            break;
        }
      }
      else if(j == 0)
      {

        data->table_result[i][j] = gtk_entry_new();
        gtk_widget_set_name (data->table_result[i][j],"entry");
        gtk_widget_set_sensitive(data->table_result[i][j],FALSE);
        gtk_entry_set_width_chars(GTK_ENTRY(data->table_result[i][j]),12);

        char setText[2];
        char arrayText[2];

        strcpy(setText, "");
        sprintf(arrayText, "%d", (i - 1));
        strcat(setText, arrayText);

        gtk_entry_set_text(GTK_ENTRY(data->table_result[i][j]), setText);
      }
      else
      {

        data->table_result[i][j] = gtk_entry_new();
        gtk_widget_set_name(data->table_result[i][j],"entry");
        gtk_widget_set_sensitive(data->table_result[i][j],FALSE);
        gtk_entry_set_width_chars(GTK_ENTRY(data->table_result[i][j]),12);
        
        char setText[10];
        char arrayText[10];

        strcpy(setText, "");

        switch(j)
        {
          case 1:
            sprintf(arrayText, "%d", finalData[i - 1].value);
            strcat(setText, arrayText);
            break;

          case 2:
            for(int k = 0; k < finalData[i - 1].position; k++) {
              sprintf(arrayText, "%d", finalData[i - 1].year[k]);
              strcat(setText, arrayText);

              if(k < (finalData[i - 1].position - 1)) {

                strcat(setText,",");
              }
            }
            break;

          case 3:
            sprintf(arrayText, "%d", finalData[i - 1].profit);
            strcat(setText, arrayText);
            break;
        }

        gtk_entry_set_text(GTK_ENTRY(data->table_result[i][j]), setText);
      }

      gtk_grid_attach (GTK_GRID(column), data->table_result[i][j], j, i, 1, 1);
    }
  }
}

void excecuteAlgorithm(Widgets *widgets, Widgets *data, GtkWidget *column) {

  createObjects(data);

  ObjectsResult *finalData = calloc(project_time + 1, sizeof(ObjectsResult));

  replaceAlgorithm(data, finalData);

  createFinalTable(widgets, column, finalData);

  plans(finalData);
  counter_plans = counter_x;
  createOptimalSolution(widgets, counter_plans);
}

void on_accept_btn_clicked(GtkWidget *widget, Widgets *data) {

  int error = 1;
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *draw;
  GtkWidget *accept;
  GtkWidget *cancel;
  GtkWidget *column;
  GtkWidget *scroll;
  GtkWidget *label;

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, GLADE, NULL);

  if(file == 0) {
    price = gtk_spin_button_get_value_as_int(data->sb_2);
    project_time = gtk_spin_button_get_value_as_int(data->sb_3);
  }

  if(price && project_time)
  {
    Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "results"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_result_destroy), widgets);
    widgets->parent = window;

    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);

    widgets->btn_3 = GTK_WIDGET(gtk_builder_get_object(builder, "draw_btn"));
    g_signal_connect (G_OBJECT (widgets->btn_3), "clicked", G_CALLBACK (on_draw_btn_clicked), widgets);
    gtk_widget_set_name (widgets->btn_3, "button");

    cancel = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_result_btn"));
    g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);
    gtk_widget_set_name (cancel, "button");

    widgets->lbl_1 = GTK_WIDGET(gtk_builder_get_object(builder, "result_lbl"));
    gtk_widget_set_name (widgets->lbl_1, "label");

    scroll = GTK_WIDGET(gtk_builder_get_object(builder, "result_scroll"));

    widgets->table_result = calloc((project_time + 2), sizeof(GtkWidget**));

    for(int i = 0; i < (project_time + 2); ++i)
    {
      widgets->table_result[i] = calloc(4, sizeof(GtkWidget*));
    }

    column = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (scroll), column);

    excecuteAlgorithm(widgets, data, column);
    error = 0;
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

void fill_data(Widgets *data, GtkWidget *column) {

  for(int i = 0; i < (lifetime + 1); i++)
  {
    for(int j = 0; j < 3; j++)
    {
      if(i == 0)
      {
        switch (j)
        {
          case 0:
            data->table[i][j] = gtk_label_new("Año");
            gtk_widget_set_name (data->table[i][j],"label");
            break;

          case 1:
            data->table[i][j] = gtk_label_new("Venta");
            gtk_widget_set_name (data->table[i][j],"label");
            break;

          case 2:
            data->table[i][j] = gtk_label_new("Mantenimiento");
            gtk_widget_set_name (data->table[i][j],"label");
            break;
        }
      }
      else if(j == 0)
      {
        data->table[i][j] = gtk_entry_new();
        gtk_widget_set_name (data->table[i][j],"entry");
        gtk_widget_set_sensitive(data->table[i][j],FALSE);
        gtk_entry_set_width_chars(GTK_ENTRY(data->table[i][j]),12);

        char setText[2];
        char arrayText[2];

        strcpy(setText, "");
        sprintf(arrayText, "%d", i);
        strcat(setText, arrayText);

        gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), setText);
      }
      else
      {
        data->table[i][j] = gtk_entry_new();
        gtk_widget_set_name (data->table[i][j],"entry");
        gtk_widget_set_sensitive(data->table[i][j],TRUE);
        gtk_entry_set_width_chars(GTK_ENTRY(data->table[i][j]),12);

        if(file == 1) {

          char setText[100];
          char arrayText[100];
          strcpy(setText, "");
          sprintf(arrayText, "%d", values[i - 1][j]);
          strcat(setText, arrayText);

          gtk_entry_set_text(GTK_ENTRY(data->table[i][j]), setText);
        }
      }

      gtk_grid_attach (GTK_GRID (column), data->table[i][j], j, i, 1, 1);
    }
  }

  file = 0;
}

void close_window(GtkWidget *widget, gpointer window) {

    gtk_widget_destroy(GTK_WIDGET(window));
}

void on_data_destroy(GtkWidget *widget, Widgets *data) {

  for(int i = 0; i < (lifetime + 1); ++i)
  {
    free(data->table[i]);
  }

  free(data->table);
  free(initialData);
  free(finalData);

  lifetime = 0;
  file = 0;

  gtk_main_quit();
}

void on_create_display_btn_clicked(GtkWidget *widget, Widgets *data) {

  int error = 1;
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *accept;
  GtkWidget *cancel;
  GtkWidget *column;
  GtkWidget *scroll;
  GtkWidget *label;

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, GLADE, NULL);

  if(file == 0){
    lifetime = gtk_spin_button_get_value_as_int(data->sb_1);
  }

  if(lifetime)
  {
    Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "data"));
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_data_destroy), widgets);
    widgets->parent = window;

    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_transient_for (widgets->parent, data->parent);

    widgets->btn_1 = GTK_WIDGET(gtk_builder_get_object(builder, "result_btn"));
    g_signal_connect (G_OBJECT (widgets->btn_1), "clicked", G_CALLBACK (on_accept_btn_clicked), widgets);
    gtk_widget_set_name (widgets->btn_1, "button");

    cancel = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_btn"));
    g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);
    gtk_widget_set_name (cancel, "button");

    widgets->sb_2 = GTK_WIDGET(gtk_builder_get_object(builder, "price"));
    gtk_spin_button_set_range (GTK_SPIN_BUTTON(widgets->sb_2),1,1000000000);
    gtk_spin_button_set_increments (GTK_SPIN_BUTTON(widgets->sb_2),1,2);
    gtk_widget_set_name (widgets->sb_2, "spin");

    widgets->sb_3 = GTK_WIDGET(gtk_builder_get_object(builder, "project_time"));
    gtk_spin_button_set_range (GTK_SPIN_BUTTON(widgets->sb_3),1,99);
    gtk_spin_button_set_increments (GTK_SPIN_BUTTON(widgets->sb_3),1,2);
    gtk_widget_set_name (widgets->sb_3, "spin");

    if(file == 1) {
      gtk_spin_button_set_value(widgets->sb_2, price);
      gtk_spin_button_set_value(widgets->sb_3, project_time);
    }

    scroll = GTK_WIDGET(gtk_builder_get_object(builder, "data_scrolled"));

    widgets->table = calloc(lifetime + 1, sizeof(GtkWidget**));

    for(int i = 0; i < (lifetime + 1); ++i)
    {
      widgets->table[i] = calloc(3, sizeof(GtkWidget*));
    }

    column = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (scroll), column);

    fill_data(widgets, column);

    error = 0;
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

  gtk_widget_set_name(lbl_file,"no_load_label");

  g_object_unref(builder);
  gtk_widget_show_all(window);
  gtk_main();
}

void on_display_destroy(GtkWidget *widget, Widgets *data) {

    free(data);
    gtk_main_quit();
}

void on_menu_about_button_press_event() {

    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *accept;
  
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, GLADE, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "about"));
    gtk_builder_connect_signals(builder, NULL);

    accept = GTK_WIDGET(gtk_builder_get_object(builder, "about_ok_btn"));
    g_signal_connect (G_OBJECT (accept), "clicked", G_CALLBACK (close_window), G_OBJECT (window));
    gtk_widget_set_name (accept, "button");
            
    g_object_unref(builder);
 
    gtk_widget_show(window);
    gtk_main();
}

void on_menu_quit_button_press_event() {

  gtk_main_quit();
}

void CSS(void) {

    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *cssFile = "equipos.css";
    GError *error = 0;
    gtk_css_provider_load_from_file(provider, g_file_new_for_path(cssFile), &error);
    g_object_unref (provider);
}

int main(int argc, char *argv[]) {

  srand((unsigned int)time(NULL));

	GtkBuilder *builder = NULL;
	GtkWidget *window = NULL;
	GError *err = NULL;

	GtkWidget *create = NULL;
	GtkWidget *exit = NULL;

	Widgets *widgets = NULL; 

	gtk_init(&argc, &argv);

	CSS();

	builder = gtk_builder_new();
	if (0 == gtk_builder_add_from_file(builder, GLADE, &err))
	{
	  fprintf(stderr, "Error adding build from file. Error: %s\n", err->message);
	  return 1;
	}

	window = GTK_WIDGET(gtk_builder_get_object(builder, "display"));
	if (NULL == window)
	{
  	fprintf(stderr, "Unable to find object with id \"display\" \n");
  	return 1;
	}
	gtk_builder_connect_signals(builder, NULL);

	widgets = (Widgets*)malloc(1 * sizeof(Widgets));
	widgets->parent = window;
	g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(on_display_destroy), widgets);

  widgets->sb_1 = GTK_WIDGET(gtk_builder_get_object(builder, "lifetime"));
  gtk_spin_button_set_range (GTK_SPIN_BUTTON(widgets->sb_1),1,99);
  gtk_spin_button_set_increments (GTK_SPIN_BUTTON(widgets->sb_1),1,2);
  gtk_widget_set_name (widgets->sb_1, "spin");

	create = GTK_WIDGET(gtk_builder_get_object(builder, "create_display_btn"));
	g_signal_connect(G_OBJECT (create), "clicked", G_CALLBACK (on_create_display_btn_clicked), widgets);
  gtk_widget_set_name (create, "button");

	exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_display_btn"));
	g_signal_connect(G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);
  gtk_widget_set_name (exit, "button");

	lbl_file = GTK_WIDGET(gtk_builder_get_object(builder, "file_load_lbl"));

	g_object_unref(builder);
	gtk_widget_show(window);
	gtk_main();

	return 0;
}