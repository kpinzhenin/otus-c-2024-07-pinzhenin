#include <gtk/gtk.h>
#include <sys/stat.h>

enum
{
	SIMPLE_TEXT_COLUMN,
	ANOTHER_TEXT_COLUMN,
	N_COLUMNS
};
 // because all function from example is deprecated
G_GNUC_BEGIN_IGNORE_DEPRECATIONS

static void print_some_thing(GtkWidget *widget, gpointer data);
static void activate(GtkApplication *app, gpointer user_data);
static void store_contain(GtkTreeStore *store,const gchar* dir_path, GtkTreeIter *parent_iter);
GtkWidget* create_tree_view_of_store(GtkTreeStore *store);

int main(int argc, char **argv)
{
	// declaration of app
	GtkApplication *app;
	int status = 0;
#if 1
	// initialization of app
	app = gtk_application_new("app.name", G_APPLICATION_DEFAULT_FLAGS);
	if (app == NULL)
	{ // check create app
		printf("error create app..\n");
		return -1;
	}

	// link app's signal
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	// run
	status = g_application_run(G_APPLICATION(app), argc, argv);

	// clear ref to app
	g_object_unref (app);
#endif
	return status;
}

static void activate(GtkApplication *app, gpointer user_data)
{
//======Initialization of gtk component
#if 1
	GtkWidget *window;
	GtkWidget *button;

	GtkTreeStore *store;

	GtkWidget *grid;
#endif

//===== initialization of window
#if 1
	window = gtk_application_window_new(app);
	// set window properties
	gtk_window_set_title (GTK_WINDOW (window), "Window");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 400); // width x height 
#endif
	//===== initialization grid container
	grid = gtk_grid_new();
	// Pack container inside window, set Parent for grid
	gtk_window_set_child(GTK_WINDOW(window), grid);

//===== initialization button
#if 0
	button = gtk_button_new_with_label("just_Button");
	// set signal for widget button (NULL is user data)
	g_signal_connect(button, "clicked", G_CALLBACK(print_some_thing), NULL);

	// attach the button inside the grid
	gtk_grid_attach(GTK_GRID(grid), button, 0,0,1,1);
#endif

//======initialization tree (contain only type of column)
	store = gtk_tree_store_new( N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
	// filling the storage
	store_contain(store, ".", NULL);
#if 0
	// initial tree's iterator ref to add user data
	GtkTreeIter iter;
	gtk_tree_store_append( store, &iter, NULL);

	// write data in tree
	gtk_tree_store_set( store, &iter,
			SIMPLE_TEXT_COLUMN, "field_1_content", // column number and content 
			ANOTHER_TEXT_COLUMN, "f_field_content",
			-1);			// must and
#endif
//===== create component to view of tree
#if 0
	GtkWidget *tree;
	tree = gtk_tree_view_new_with_model( GTK_TREE_MODEL( store) );

	// columns and cell render
	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_text_new(); // what doesn't mean...?
//
	GtkTreeViewColumn *column;
	column = gtk_tree_view_column_new_with_attributes( "Folder name", renderer,
						     "text", SIMPLE_TEXT_COLUMN,
						     NULL);
	gtk_tree_view_append_column( GTK_TREE_VIEW( tree), column );

	column = gtk_tree_view_column_new_with_attributes( "Folder properties", renderer,
						     "text", ANOTHER_TEXT_COLUMN,
						     NULL);
	gtk_tree_view_append_column( GTK_TREE_VIEW( tree), column );
#endif
	GtkWidget *tree = create_tree_view_of_store(store);
	// try append in grid
	gtk_grid_attach( GTK_GRID(grid), tree, 0,0,1,1);

	// show the window ?
	gtk_window_present(GTK_WINDOW(window));
}

GtkWidget* create_tree_view_of_store(GtkTreeStore *store)
{
	GtkWidget *tree;
	tree = gtk_tree_view_new_with_model( GTK_TREE_MODEL( store) );

	// columns and cell render
	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_text_new(); // what doesn't mean...?
// create column view

	GtkTreeViewColumn *column;
	column = gtk_tree_view_column_new_with_attributes( "Folder name", renderer,
						     "text", SIMPLE_TEXT_COLUMN,
						     NULL);
	gtk_tree_view_append_column( GTK_TREE_VIEW( tree), column );

// add new column view
#if 0
	column = gtk_tree_view_column_new_with_attributes( "Folder properties", renderer,
						     "text", ANOTHER_TEXT_COLUMN,
						     NULL);
	gtk_tree_view_append_column( GTK_TREE_VIEW( tree), column );
#endif	
	return tree;
}

static void store_contain(GtkTreeStore *store,
		const gchar* dir_path,
	       	GtkTreeIter *parent_iter)
{
	// create iterator for append data in tree store
	//GtkTreeIter *parent_iter = NULL;
	// ????? start of internal function
	GtkTreeIter iter;

	// create data directory structure
	GError *error = NULL;
	GDir *dir = g_dir_open(dir_path, 0, &error);
	
	const gchar *filename;
	while(filename = g_dir_read_name(dir))
	{
		gchar *file_full_path = g_build_filename(dir_path, filename, NULL);
		gtk_tree_store_append( store, &iter, parent_iter);
		gtk_tree_store_set( store, &iter,
			SIMPLE_TEXT_COLUMN, filename,
			ANOTHER_TEXT_COLUMN, "f_field_content",
			-1);
		
		struct stat file_stat;
		stat(file_full_path, &file_stat);

		if(S_ISDIR(file_stat.st_mode))
			store_contain(store, file_full_path, &iter);

	}
	g_dir_close(dir);
#if 0
	// write data in tree
	gtk_tree_store_set( store, &iter,
			SIMPLE_TEXT_COLUMN, "field_1_content", // column number and content 
			ANOTHER_TEXT_COLUMN, "f_field_content",
			-1);			// must and
	
	gtk_tree_store_append( store, &iter, NULL);
	gtk_tree_store_set( store, &iter,
			SIMPLE_TEXT_COLUMN, "field_2_content", // column number and content 
			ANOTHER_TEXT_COLUMN, "f_field_content",
			-1);			// must and

	GtkTreeIter child_iter;
	gtk_tree_store_append( store, &child_iter, &iter);
	gtk_tree_store_set( store, &child_iter,
			SIMPLE_TEXT_COLUMN, "c_field_1_content", // column number and content 
			ANOTHER_TEXT_COLUMN, "f_field_content",
			-1);			// must and
#endif

}
	
static void print_some_thing(GtkWidget *widget, gpointer data)
{
	printf("some text\n");
}
