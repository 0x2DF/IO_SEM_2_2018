#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define INF 1<<30	// 1,073,741,824
#define V 1
#define R 0
#define NAME_LEN 20

static const char GLADE[] = "pm.glade";

const char *alphabet[27]={"A","B","C","D","E","F","G","H","I","J","K","L","M","N","Ñ","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
char **header = NULL;

typedef struct Widgets {
  	GtkEntry *text_box_1;
  	GtkEntry *text_box_2;
  	GtkWidget ***table;
  	GtkWidget ***table_2;
  	GtkLabel *lbl_1;
  	GtkLabel *lbl_2;
  	GtkLabel *lbl_3;
  	GtkButton *btn;
  	GtkWindow *window;
  	GtkWindow *parent;
} Widgets;

typedef struct  {
   int color;
   int value;	
   int cost;	
   int totalObjects;
} Objects;

GtkLabel *lbl_file = NULL;

GtkButton *onezero_btn;
GtkButton *bounded_btn;
GtkButton *unbounded_btn;

int CELL_QTY = 0;
int BACKPACK_QTY = 0;
int file = 0;

int onezero = 1;
int bounded = 0;
int unbounded = 0;

int** table_cell_data;

Objects *objectProblem;

void close_window(GtkWidget *widget, gpointer window);

void alloc_table_cell_data() {

	table_cell_data = calloc(CELL_QTY, sizeof(int*));

	for(int i = 0; i < CELL_QTY; i++)
	{
  		table_cell_data[i] = calloc(3, sizeof(int));
  	}
}

void alloc_header() {

	header = malloc(NAME_LEN * sizeof(char*));

	for (int i = 0; i < CELL_QTY; ++i)
	{
	    header[i] = malloc((NAME_LEN + 1) * sizeof(char));
	}
}

int isNumeric (const char *s) {

    if (s == NULL || *s == '\0' || isspace(*s))
    {
    	return 0;
    }

    char * p;
    strtod (s, &p);

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
  	
  	if (fp)
  	{
    	while ((read = getline(&line, &len, fp)) != -1) 
    	{
      		if (line[read - 1] == '\n') 
      		{
        		line[read - 1] = '\0';
        		--len;
      		}
      		if (k == 0)
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
                				CELL_QTY = temp_qty;
                				alloc_header();
                				alloc_table_cell_data();
              				}
              				else
              				{
                				error = 1;
                				printf("Error: La cantidad de nodos tiene que ser mayor a 0.\n");
              				}
            			}
            			else
            			{
              				error = 1;
              				printf("Error: Cantidad de nodos no es un valor numerico.\n");
            			}
          			}
          			else if (i == 1)
          			{
          				if (isNumeric(token))
            			{
              				int temp_qty = (int) strtol(token, (char **)NULL, 10);

              				if (temp_qty > 0)
              				{
                				BACKPACK_QTY = temp_qty;
              				}
              				else
              				{
                				error = 1;
                				printf("Error: La cantidad de nodos tiene que ser mayor a 0.\n");
              				}
            			}
            			else
            			{
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
      		else if (k == 1)
      		{
        		i = 0;
        		tofree = str = strdup(line);
        		while ((token = strsep(&str, ",")) && (error != 1))
        		{
          			if (i < CELL_QTY)
          			{
            			char setText[100];
					  	strcpy(setText, token);
					  	strcpy(header[i], setText);
          			}
          			else
          			{
            			error = 1;
            			printf("Error: La cantidad de nodos definida en la linea %d excede la cantidad [N = %d] definida en la primer linea.\n", k+1, CELL_QTY);
          			}

          			i++;
        		}

        		free(tofree);
      		}
      		else
      		{
        		if (k >= (CELL_QTY+2))
        		{
          			error = 1;
          			printf("Warning: El archivo contiene lineas adicionales. Ignorandolas.\n");
        		}
        		else
        		{
          			i = 0;
          			tofree = str = strdup(line);

          			while ((token = strsep(&str, ",")) && (error != 1))
          			{
            			if (i < CELL_QTY)
            			{
              				if (isNumeric(token))
              				{
                				table_cell_data[k-2][i] = atof(token);
              				}
              				else
              				{
                				error = 1;
                				printf("Error: Peso en la arista no es un valor numerico.\n");
              				}
            			}
            			else
            			{
             	 			error = 1;
              				printf("Error: La cantidad de nodos definida en la linea %d excede la cantidad [N = %d] definida en la primer linea.\n", k+1, CELL_QTY);
            			}

            			i++;
          			}

          			free(tofree);
        		}
      		}

      		if (error)
      		{
        		if (k == 0) CELL_QTY = 0;
        		if (k == 1) BACKPACK_QTY = 0;
        		printf("Se produjo un error leyendo los contenidos del archivo. Reviselo y vuelva a intentar.\n");
        		break;
      		}

      		k++;
    	}

    	fclose(fp);
    	if (line) free(line);
  	}

  	gtk_widget_set_name (lbl_file,"load_label");
}

void saveFile(char *filename) {

  	FILE *fp = fopen(filename, "w");

  	if (fp)
  	{
    	fprintf(fp, "%d,", CELL_QTY);
    	fprintf(fp, "%d\n", BACKPACK_QTY);

    	for (int i = 0; i < CELL_QTY; ++i)
    	{
      		if (i == 0) fprintf(fp, "%s", header[i]);
      		else fprintf(fp, ",%s", header[i]);
    	}

    	fprintf(fp, "\n");

    	for (int i = 0; i < CELL_QTY; i++)
    	{
      		for (int j = 0; j < 3; j++)
      		{
        		if (j == 0)
        		{
        			fprintf(fp, "%d", table_cell_data[i][j]);
        		}
        		else 
        		{
        			fprintf(fp, ",%d", table_cell_data[i][j]);
        		}
      		}

      		fprintf(fp, "\n");
    	}
    	printf("Archivo guardado correctamente en: %s\n", filename);
    	fclose(fp);
  	}
  	else
  	{
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

  	gtk_widget_destroy (dialog);
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

void on_menu_save_button_press_event() {

  /*gtk_main_quit();*/
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
					  
  	g_object_unref(builder);
 
  	gtk_widget_show(window);
  	gtk_main();
}

void on_menu_quit_button_press_event() {

  gtk_main_quit();
}

void suelo(Objects todos[CELL_QTY]){

	for (int i = 0; i < CELL_QTY; i++)
	{
		Objects objetos;
		objetos = todos[i];

		if (objetos.totalObjects > 1)
		{
			int suelo = objetos.cost / (BACKPACK_QTY - 1);
			objetos.totalObjects = suelo;
		}
	}
}

void knapsackAlgorithm(Objects matrizObjects[BACKPACK_QTY][CELL_QTY], Objects todos[CELL_QTY]) {

	int matriz[BACKPACK_QTY + 1][CELL_QTY];
	suelo(todos);

	int con = 0;

	Objects zero;
	zero.cost = 0;
	zero.value = 0;
	zero.totalObjects =0;

	for (int i = 0; i < BACKPACK_QTY+1; i++)
	{
		for (int j = 0; j < CELL_QTY; j++)
		{
			matriz[i][j] =0;
		}
	}

	for (int i = 0; i < BACKPACK_QTY+1; i++) 
	{
		for (int j = 0; j < CELL_QTY; j++) 
		{
			matrizObjects[i][j] = zero;
		}
	}
	
	for (int j = 0; j < CELL_QTY; j++)
	{
		for (int i = 0; i < BACKPACK_QTY+1; i++)
		{
			Objects actual = todos[j];
			Objects guardar;
			guardar.cost = 0;
			guardar.value = 0;
			guardar.totalObjects =0;

			if (j == 0) 
			{
				if (actual.cost <= i) 
				{
					guardar.color = V;
					int aux = con +1;
					if (actual.totalObjects == 1){
						con =1;
					}
					else
					{
						if (aux * actual.cost == i && i!=0 && con <= actual.cost)
						{
							con ++;
						}	
					}
				}
				else
				{ 
					guardar.color = R;
				}

				guardar.value = actual.value * con;
				guardar.totalObjects = con;
				guardar.cost = actual.cost * con;
					
				matriz[i][j] = con * actual.value;
				matrizObjects[i][j] = guardar;
			}

			else 
			{
				if (actual.cost > i) 
				{
					guardar = matrizObjects[i][j-1];
					guardar.totalObjects = 0;
					guardar.color = R;
					matrizObjects[i][j] =guardar;
					matriz[i][j] = matriz[i][j-1];

				}
				else 
				{
					guardar.cost = matrizObjects[i][j-1].cost;
					guardar.color = R;		
					int optimo = matriz[i][j-1];
					int candidato = actual.value + matriz[i-actual.cost][j-1];
					int cuenta_candidato = 1;
					int cuenta_optimo = 0;  

					for ( int contador = 1; contador <= actual.totalObjects; contador++) 
					{
						if (actual.cost * contador <= i) 
						{
							if (candidato < (contador * actual.value + matriz[i-(contador*actual.cost)][j-1]) ) 
							{
								candidato = contador * actual.value + matriz[i-(contador*actual.cost)][j-1];
								cuenta_candidato = contador;
							}
						}
						else 
						{
							break;
						}
					}
					
					if (optimo < candidato) 
					{
						optimo = candidato;
						cuenta_optimo = cuenta_candidato;
						guardar.cost = actual.cost * cuenta_optimo;
						guardar.color = V;
					}					
						
					guardar.value = optimo;
					guardar.totalObjects = cuenta_optimo;
					matriz[i][j] = optimo;
					matrizObjects[i][j] = guardar;
				}
			}
		}
	}
}

void createHeader(Widgets *data) {

	if(file == 0){
		alloc_header();
	}

	for(int i =0; i < (CELL_QTY + 1); i++) 
	{
		for(int j=0; j < 4; j++) 
		{
			if (j == 0 && i != 0){

				header[i - 1] = gtk_entry_get_text(GTK_ENTRY(data->table[i][j]));
			}
		}
	}
}

void createObjects(Widgets *data) {

	objectProblem = malloc(CELL_QTY * sizeof(Objects));
  	Objects numbers;

  	for (int i = 1; i <= CELL_QTY; i++)
  	{
    	numbers.value = atoi(gtk_entry_get_text(GTK_ENTRY(data->table[i][1])));
    	numbers.cost = atoi(gtk_entry_get_text(GTK_ENTRY(data->table[i][2])));

    	if (strcmp(gtk_entry_get_text(GTK_ENTRY(data->table[i][3])),"INF") == 0) 
    	{
      		numbers.totalObjects = INF;
    	}
    	else 
    	{
      		numbers.totalObjects = atoi(gtk_entry_get_text(GTK_ENTRY(data->table[i][3])));
    	}

    	objectProblem[i-1] = numbers;

    	table_cell_data[i-1][0] = numbers.value;
    	table_cell_data[i-1][1] = numbers.cost;
    	table_cell_data[i-1][2] = numbers.totalObjects;
  	}
}

void getOptimalSolution(Objects *optimalSolution, Objects matrizObjects[BACKPACK_QTY][CELL_QTY]) {

	int row = BACKPACK_QTY;
	int column = CELL_QTY;
	int count = 0;

	Objects object = matrizObjects[row][column - 1];

	for (int i = CELL_QTY; i > 0; --i)
	{
		optimalSolution[count] = object;

		if (object.color == 0) 
		{
			column--;
		}
		else 
		{
			column--;
			row = row - object.cost;
		}

		object = matrizObjects[row][column - 1];
		count++;
	}
}

void createOptimalSolutionLabel(Objects *optimalSolution, Widgets* data) {

  	char setText[500];
  	char setCost[500];
  	char setValue[500];

  	char arrayText[500];
  	char arrayValue[500];
  	char arrayCost[500];

  	// Z = <optimalSolution>
  	strcpy(setText, "Z = ");
  	sprintf(arrayText, "%d", optimalSolution[0].value);
  	strcat(setText, arrayText);

  	strcpy(setValue, "Z = ");
  	sprintf(arrayValue, "%sX_%s", gtk_entry_get_text(GTK_ENTRY(data->table[1][1])), header[0]);
  	strcat(setValue,arrayValue);

  	strcpy(setCost, "");
  	sprintf(arrayCost, "%sX_%s", gtk_entry_get_text(GTK_ENTRY(data->table[1][2])), header[0]);
  	strcat(setCost,arrayCost);

  	int valueCounter = 2;
  	int nameCounter1 = 0;
  	int nameCounter2 = 1;

  	for (int i = CELL_QTY; i > 0; i--)
  	{
    	strcat(setText, " X_");
    	sprintf(arrayText, "%s = %d ", header[nameCounter1], optimalSolution[i - 1].totalObjects);
    	strcat(setText, arrayText);

    	if(nameCounter2 <= (CELL_QTY - 1)) {

    		// //Maximizar
	    	strcat(setValue, "");
	    	sprintf(arrayValue, " + %sX_%s", gtk_entry_get_text(GTK_ENTRY(data->table[valueCounter][1])), header[nameCounter2]);
	    	strcat(setValue, arrayValue);
	    
	    	// //Sujeto a
	    	strcat(setCost, "");
	    	sprintf(arrayCost, " + %sX_%s", gtk_entry_get_text(GTK_ENTRY(data->table[valueCounter][2])), header[nameCounter2]);
	    	strcat(setCost, arrayCost);

			nameCounter2++;
			valueCounter++;
    	}

    	nameCounter1++;

    	if((i - 1) == 0){

    		strcat(setCost, " <=");
	    	sprintf(arrayCost, " %d", BACKPACK_QTY);
	    	strcat(setCost, arrayCost);

    		gtk_label_set_text(data->lbl_1, setValue);
    		gtk_label_set_text(data->lbl_2, setCost);
    		gtk_label_set_text(data->lbl_3, setText);
    	}
  	}
}

void createFinalTable(Widgets* data, Objects matrizObjects[BACKPACK_QTY][CELL_QTY], GtkWidget *solution) {

	for(int i = 0; i < (BACKPACK_QTY + 2); i++)
	{
		for(int j = 0; j < (CELL_QTY + 1); j++)
		{
			if(i == 0 && j == 0)
			{
				data->table_2[i][j] = gtk_label_new("");
				gtk_widget_set_name (data->table_2[i][j], "label_null");
			}
			else if(i == 0 && j != 0)
			{
				data->table_2[i][j] = gtk_entry_new();
        		gtk_widget_set_name (data->table_2[i][j],"entry_name");
        		gtk_widget_set_sensitive(data->table_2[i][j],FALSE);
        		gtk_entry_set_width_chars(GTK_ENTRY(data->table_2[i][j]),12);
        		gtk_entry_set_text (GTK_ENTRY(data->table_2[i][j]), header[j - 1]);
			}
			else if(i != 0 && j == 0)
			{
				char setTextValue[10];
			  	char arrayTextValue[10];

			  	strcpy(setTextValue, "");
			  	sprintf(arrayTextValue, " %d ", i - 1);
			  	strcat(setTextValue, arrayTextValue);

			  	data->table_2[i][j] = gtk_entry_new();
        		gtk_widget_set_name (data->table_2[i][j],"entry_value");
        		gtk_widget_set_sensitive(data->table_2[i][j],FALSE);
        		gtk_entry_set_width_chars(GTK_ENTRY(data->table_2[i][j]),5);
        		gtk_entry_set_text (GTK_ENTRY(data->table_2[i][j]), setTextValue);
			}
			else
			{
				char setText[20];
			  	char arrayText[20];

			  	strcpy(setText, "");
			  	sprintf(arrayText, " %d ", matrizObjects[i - 1][j - 1].value);
			  	strcat(setText, arrayText);

				data->table_2[i][j] = gtk_entry_new();
        		gtk_widget_set_sensitive(data->table_2[i][j],FALSE);
        		gtk_entry_set_width_chars(GTK_ENTRY(data->table_2[i][j]),12);
        		gtk_entry_set_text (GTK_ENTRY(data->table_2[i][j]), setText);

				if(matrizObjects[i - 1][j - 1].color == 0)
				{
					gtk_widget_set_name (data->table_2[i][j],"entry_valueR");
				}
				else
				{
					gtk_widget_set_name (data->table_2[i][j],"entry_valueG");
				}
			}

			gtk_grid_attach (GTK_GRID (solution), data->table_2[i][j], j, i, 1, 1);
		}
	}
}

void excecuteAlgorithm(Widgets *widgets, Widgets *data, GtkWidget *solution) {

	createHeader(data);
	createObjects(data);

	Objects finalMatrix[BACKPACK_QTY + 1][CELL_QTY];
	knapsackAlgorithm(finalMatrix, objectProblem);

	Objects optimalSolution[CELL_QTY + 1];
	getOptimalSolution(optimalSolution, finalMatrix);

	createOptimalSolutionLabel(optimalSolution, data);

	createFinalTable(widgets, finalMatrix, solution);
}

void fill_data_table(Widgets *data, GtkWidget *column) {

	for(int i = 0; i < (CELL_QTY + 1); i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(i == 0 && j == 0)
			{
				data->table[i][j] = gtk_label_new("");
				gtk_widget_set_name (data->table[i][j],"label");
			}
			else if(i == 0)
			{
				switch (j)
				{
					case 1:
						data->table[i][j] = gtk_label_new("valor");
						gtk_widget_set_name (data->table[i][j],"label");
						break;

					case 2:
						data->table[i][j] = gtk_label_new("costo");
						gtk_widget_set_name (data->table[i][j],"label");
						break;

					case 3:
						data->table[i][j] = gtk_label_new("cantidad");
						gtk_widget_set_name (data->table[i][j],"label");
						break;
				}
			}
			else if(j == 0)
			{
				data->table[i][j] = gtk_entry_new();
        		gtk_widget_set_name (data->table[i][j],"entry_name");
        		gtk_widget_set_sensitive(data->table[i][j],TRUE);
        		gtk_entry_set_width_chars(GTK_ENTRY(data->table[i][j]),12);

        		if (file == 1)
        		{
        			gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), header[i-1]);
        		}
        		else
        		{
        			gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), alphabet[i-1]);
        		}
			}
			else if(j == 3)
			{
				data->table[i][j] = gtk_entry_new();
        		gtk_widget_set_name (data->table[i][j],"entry");
        		gtk_entry_set_width_chars(GTK_ENTRY(data->table[i][j]),12);

        		if(file == 1)
        		{
        			char setText[100];
				  	char arrayText[100];

				  	strcpy(setText, "");

				  	if(table_cell_data[i - 1][j - 1] != INF)
				  	{
				  		sprintf(arrayText, "%d", table_cell_data[i - 1][j - 1]);
				  	}
				  	else
				  	{
				  		sprintf(arrayText, "%s", "INF");
				  	}

				  	strcat(setText, arrayText);

        			gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), setText);
        		}
        		else
        		{
        			if(unbounded == 1){
        				gtk_entry_set_text (GTK_ENTRY(data->table[i][j]),"INF");
        				gtk_widget_set_sensitive(data->table[i][j],FALSE);
        			}
        			else if(onezero == 1){
        				gtk_entry_set_text (GTK_ENTRY(data->table[i][j]),"1");
        				gtk_widget_set_sensitive(data->table[i][j],FALSE);
        			}
        			else if(bounded){
        				gtk_widget_set_sensitive(data->table[i][j],TRUE);
        			}
        		}
			}
			else
			{
				data->table[i][j] = gtk_entry_new();
        		gtk_widget_set_name (data->table[i][j],"entry");
        		gtk_widget_set_sensitive(data->table[i][j],TRUE);
        		gtk_entry_set_width_chars(GTK_ENTRY(data->table[i][j]),12);


        		if(file == 1)
        		{
        			char setText[5];
				  	char arrayText[5];

				  	strcpy(setText, "");
				  	sprintf(arrayText, "%d", table_cell_data[i - 1][j - 1]);
				  	strcat(setText, arrayText);

        			gtk_entry_set_text (GTK_ENTRY(data->table[i][j]), setText);
        		}
			}

			gtk_grid_attach (GTK_GRID (column), data->table[i][j], j, i, 1, 1);
		}
	}

	file = 0;
}

void on_objects_destroy(GtkWidget *widget, Widgets *data) {

	for(int i = 0; i < (BACKPACK_QTY + 2); i++)
  	{
    	free(data->table_2[i]);
  	}

    free(objectProblem);
    free(data->table_2);

	BACKPACK_QTY = 0;

  	gtk_main_quit();
}

void on_table_data_destroy(GtkWidget *widget, Widgets *data) {

	if(file == 1) {

	  	for(int i = 0; i < CELL_QTY; i++)
	  	{
	  		free(table_cell_data[i]);
	  	}

	  	free(table_cell_data);
	}

	for(int i = 0; i < (CELL_QTY + 1); i++)
  	{
    	free(data->table[i]);
  	}

  	if(file == 1){

  	  	for (int i = 0; i < CELL_QTY; i++)
  	    {
  	      	free(header[i]);
  	    }
  	  	
  		free(header);
  	}

  	free(data->table);

  	CELL_QTY = 0;
  	BACKPACK_QTY = 0;
  	file = 0;

  	gtk_main_quit();
}

void on_accept_btn_clicked(GtkWidget *widget, Widgets *data) {

	int error = 1;
	GtkBuilder *builder;
	GtkWidget *window;
	GtkWidget *accept;
  	GtkWidget *label;
  	GtkWidget *quit;
  	GtkWidget *column_optimalSolution;
	GtkWidget *scrolledwindow_optimalSolution;

	builder = gtk_builder_new();
  	gtk_builder_add_from_file(builder, GLADE, NULL);

  	if(gtk_entry_get_text_length(GTK_ENTRY(data->text_box_2)))
  	{
  		BACKPACK_QTY = (int)strtol(gtk_entry_get_text(GTK_ENTRY(data->text_box_2)), (char **)NULL, 10);

  		if(BACKPACK_QTY > 0)
  		{
  			Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
		    window = GTK_WIDGET(gtk_builder_get_object(builder, "final"));
		    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_objects_destroy), widgets);
		    widgets->parent = window;
		    
		    gtk_builder_connect_signals(builder, NULL);
		    gtk_window_set_transient_for (widgets->parent, data->parent);

		    quit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_final_btn"));
		    g_signal_connect (G_OBJECT (quit), "clicked", G_CALLBACK (close_window), widgets->parent);

      		scrolledwindow_optimalSolution = GTK_WIDGET(gtk_builder_get_object(builder, "scrolledwindow_table"));

    		widgets->table_2 = calloc(BACKPACK_QTY + 2, sizeof(GtkWidget**));

	      	for(int i = 0; i < (BACKPACK_QTY + 2); i++)
	      	{
	        	widgets->table_2[i] = calloc((CELL_QTY + 1), sizeof(GtkWidget*));
	      	}

	      	column_optimalSolution = gtk_grid_new ();
	      	gtk_container_add (GTK_CONTAINER (scrolledwindow_optimalSolution), column_optimalSolution);

	      	data->lbl_1 = GTK_WIDGET(gtk_builder_get_object(builder, "max_label"));
	      	gtk_widget_set_name (data->lbl_1, "max_lbl");

	      	data->lbl_2 = GTK_WIDGET(gtk_builder_get_object(builder, "restriction_label"));
	      	gtk_widget_set_name (data->lbl_2, "restriction_lbl");

	      	data->lbl_3 = GTK_WIDGET(gtk_builder_get_object(builder, "solution_label"));
	      	gtk_widget_set_name (data->lbl_3, "solution_lbl");

	      	if(file == 0)
	      	{
	      		alloc_table_cell_data();
	      	}

	      	excecuteAlgorithm(widgets, data, column_optimalSolution);

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

void on_create_display_btn_clicked(GtkWidget *widget, Widgets *data) {

	int error = 1;
	GtkBuilder *builder;
	GtkWidget *window;
	GtkWidget *accept;
  	GtkWidget *cancel;
  	GtkWidget *label;
  	GtkWidget *column;
  	GtkWidget *scroll;

	builder = gtk_builder_new();
  	gtk_builder_add_from_file(builder, GLADE, NULL);

  	if(gtk_entry_get_text_length(GTK_ENTRY(data->text_box_1)))
  	{
  		if(CELL_QTY == 0)
  		{
  			CELL_QTY = (int)strtol(gtk_entry_get_text(GTK_ENTRY(data->text_box_1)), (char **)NULL, 10);
  		}

  		if(CELL_QTY > 0)
  		{
  			Widgets *widgets = (Widgets*)malloc(1 * sizeof(Widgets));
		    window = GTK_WIDGET(gtk_builder_get_object(builder, "table_data"));
		    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_table_data_destroy), widgets);
		    widgets->parent = window;

		    gtk_builder_connect_signals(builder, NULL);
		    gtk_window_set_transient_for (widgets->parent, data->parent);

		    widgets->btn = GTK_WIDGET(gtk_builder_get_object(builder, "accept_btn"));
      		g_signal_connect (G_OBJECT (widgets->btn), "clicked", G_CALLBACK (on_accept_btn_clicked), widgets);

      		cancel = GTK_WIDGET(gtk_builder_get_object(builder, "cancel_btn"));
      		g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (close_window), widgets->parent);

      		scroll = GTK_WIDGET(gtk_builder_get_object(builder, "table_data_scrolled"));

      		widgets->table = calloc(CELL_QTY + 1, sizeof(GtkWidget**));

	      	for(int i = 0; i < (CELL_QTY + 1); ++i)
	      	{
	        	widgets->table[i] = calloc(4, sizeof(GtkWidget*));
	      	}

	      	column = gtk_grid_new ();
	      	gtk_container_add (GTK_CONTAINER (scroll), column);

	      	widgets->text_box_2 = GTK_WIDGET(gtk_builder_get_object(builder, "weight_display"));

	      	if(BACKPACK_QTY != 0)
	      	{
	      		char setText[5];
			  	char arrayText[5];

			  	strcpy(setText, "");
			  	sprintf(arrayText, "%d", BACKPACK_QTY);
			  	strcat(setText, arrayText);

	      		gtk_entry_set_text (GTK_ENTRY(widgets->text_box_2), setText); 
	      	}

	      	//alloc_header();

	      	fill_data_table(widgets, column);
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

  	gtk_widget_set_name (lbl_file,"no_load_label");

  	g_object_unref(builder);
  	gtk_widget_show_all(window);
  	gtk_main();
}

void on_display_destroy(GtkWidget *widget, Widgets *data) {

  	free(data);
  	gtk_main_quit();
}

void close_window(GtkWidget *widget, gpointer window) {

  	gtk_widget_destroy(GTK_WIDGET(window));
}

void CSS(void) {

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

void on_enable_onezero_button() {

	onezero = 1;
	bounded = 0;
	unbounded = 0;

	gtk_widget_set_name (onezero_btn, "button_select");
	gtk_widget_set_name (bounded_btn, "button_select");
	gtk_widget_set_name (unbounded_btn, "button_select");
}

void on_enable_bounded_button() {

	onezero = 0;
	bounded = 1;
	unbounded = 0;

	gtk_widget_set_name (onezero_btn, "button_select");
	gtk_widget_set_name (bounded_btn, "button_select");
	gtk_widget_set_name (unbounded_btn, "button_select");
}

void on_enable_unbounded_button() {

	onezero = 0;
	bounded = 0;
	unbounded = 1;

	gtk_widget_set_name (onezero_btn, "button_select");
	gtk_widget_set_name (bounded_btn, "button_select");
	gtk_widget_set_name (unbounded_btn, "button_select");
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

  	widgets->text_box_1 = GTK_WIDGET(gtk_builder_get_object(builder, "stuff_display"));
  	gtk_entry_set_text (GTK_ENTRY(widgets->text_box_1), "0");

  	create = GTK_WIDGET(gtk_builder_get_object(builder, "create_display_btn"));
  	g_signal_connect(G_OBJECT (create), "clicked", G_CALLBACK (on_create_display_btn_clicked), widgets);

  	exit = GTK_WIDGET(gtk_builder_get_object(builder, "exit_display_btn"));
  	g_signal_connect(G_OBJECT (exit), "clicked", G_CALLBACK (close_window), widgets->parent);

  	lbl_file = GTK_WIDGET(gtk_builder_get_object(builder, "file_load_lbl"));

  	onezero_btn = GTK_WIDGET(gtk_builder_get_object(builder, "10_btn"));
  	bounded_btn = GTK_WIDGET(gtk_builder_get_object(builder, "bounded_btn"));
  	unbounded_btn = GTK_WIDGET(gtk_builder_get_object(builder, "unbounded_btn"));

  	g_object_unref(builder);
  	gtk_widget_show(window);
  	gtk_main();
  
  	return 0;
}
