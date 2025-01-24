// gcc -o calibration_app calibration_app.c localguitype.c telnet.c $(pkg-config gtk4 --cflags --libs) -lws2_32

#include <gtk/gtk.h>
#include "telnet.h"
#include "localguitype.h"

// #include <glib.h> // mb not need

static void activate ( GtkApplication *app, gpointer user_data );

static void set_text_view_string( GtkTextView *textView, guchar* str );
static void btn_test_text_view ( GtkButton *btn, char *txt_to_print );

static void on_entry_activate( GtkEntry *entry, GListStore *store );
static void textView_signal_insert( GtkTextView *textView, char *txt_to_print );

static void factory_setup_cb ( GtkListItemFactory *factory, GObject *list_item );
void factory_bind_cb_clmn1( GtkListItemFactory *factory, GObject *list_item );
void factory_bind_cb_clmn2( GtkListItemFactory *factory, GObject *list_item );
void factory_bind_cb_clmn3( GtkListItemFactory *factory, GObject *list_item );

static void add_entry_to_clmn( GtkButton *btn, GListStore *store );
static void add_entry_from_file( GtkButton *btn, GListStore *store );
static void clear_store( GtkButton *btn, GListStore *store );

// app Data
GtkTextView *txtView_out; // for using into any widgets
GtkColumnView *clmn_view;
GtkEntry *entry_clmn1, *entry_clmn2, *entry_clmn3;  
SOCKET sock;
char recv_buffer[1024]; // add static (mb need to move to heap)

/*
typedef struct {
	double key;
	unsigned val1;
	unsigned val2;
} RowData;	

RowData init_row_data = { 11, 22, 33};
*/

int main ( int argc, char **argv )
{
	
	GtkApplication *app = gtk_application_new ( "org.gtk.example", G_APPLICATION_DEFAULT_FLAGS );
	g_signal_connect ( app, "activate", G_CALLBACK (activate), NULL );

	int status = g_application_run ( G_APPLICATION (app), argc, argv );

	closesocket( sock );
	WSACleanup();

	g_object_unref( app );
	return status;
}

static void activate (GtkApplication *app, gpointer user_data)
{
	GObject *btn_add_entry_from_file, *btn_clear_store, *btn_tst_msg_for_txtView;
	GtkColumnViewColumn* column;
	GListStore *store;
	
// ===============================GUI Create=========================
	GtkBuilder *builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, "calibration_app_gui.ui", NULL );
	
	/* Connect signal handlers to the constructed widgets. */
	GObject *window = gtk_builder_get_object( builder, "window" );
	gtk_window_set_application( GTK_WINDOW( window ), app );

	// output field
	txtView_out = (GtkTextView *)gtk_builder_get_object( builder, "textView_out" );
	set_text_view_string( txtView_out, "\t\tinit string\n" );
	g_signal_connect( txtView_out, "insert-at-cursor", G_CALLBACK( textView_signal_insert ), g_strdup( "text from signal\n" ) );
	
	// button for textView
	btn_tst_msg_for_txtView = gtk_builder_get_object( builder, "test_text_view" );
	g_signal_connect( btn_tst_msg_for_txtView, "clicked", G_CALLBACK( btn_test_text_view ), g_strdup( "text some\n" ) );
	
	//------------column View--------------------------------------------------------------------------------
	clmn_view = GTK_COLUMN_VIEW( gtk_builder_get_object( builder, "clmn_view" ) );
	
	char *column_names[] = { "Distance", "ADC code", "DAC code" }; // koef for scale to dac code 
	
	// data set for column
	store = g_list_store_new( G_TYPE_OBJECT ); //may be can set direct name of child class
	g_list_store_append( store, my_object_new( "clmn1_row1", "clmn2_row1", "clmn3_row1" ) );
	g_list_store_append( store, my_object_new( "clmn1_row2", "clmn2_row2", "clmn3_row2" ) );
	
	GtkSingleSelection *selection = gtk_single_selection_new( G_LIST_MODEL( store ) ); // list_model
	
	gtk_column_view_set_model( clmn_view, GTK_SELECTION_MODEL( selection ) );

	/* Initialize the array of GtkListItemFactory - one for each column.
	 */
	GtkListItemFactory *factory_distance_clmn = gtk_signal_list_item_factory_new();
	g_signal_connect( factory_distance_clmn, "setup", G_CALLBACK( factory_setup_cb ), NULL );
	g_signal_connect( factory_distance_clmn, "bind", G_CALLBACK( factory_bind_cb_clmn1 ), NULL );

	GtkListItemFactory *factory_adc_clmn = gtk_signal_list_item_factory_new();
	g_signal_connect( factory_adc_clmn, "setup", G_CALLBACK( factory_setup_cb ), NULL );
	g_signal_connect( factory_adc_clmn, "bind", G_CALLBACK( factory_bind_cb_clmn2 ), NULL );
	
	GtkListItemFactory *factory_dac_clmn = gtk_signal_list_item_factory_new();
	g_signal_connect( factory_dac_clmn, "setup", G_CALLBACK( factory_setup_cb ), NULL );
	g_signal_connect( factory_dac_clmn, "bind", G_CALLBACK( factory_bind_cb_clmn3 ), NULL );

	/* Create the columns.
	 */
	column = gtk_column_view_column_new( column_names[0], factory_distance_clmn );
	gtk_column_view_append_column( clmn_view, column );
	
	column = gtk_column_view_column_new( column_names[1], factory_adc_clmn );
	gtk_column_view_append_column( clmn_view, column );
	
	column = gtk_column_view_column_new( column_names[2], factory_dac_clmn );
	gtk_column_view_append_column( clmn_view, column );
	
	// input filed
	GObject *entry_  = gtk_builder_get_object( builder, "entryTxt");
	g_signal_connect(entry_, "activate", G_CALLBACK( on_entry_activate ), store );
	
	// entryes for test columnView
	entry_clmn1 = ( GtkEntry* )gtk_builder_get_object( builder, "entry_clmn1" );
	entry_clmn2 = ( GtkEntry* )gtk_builder_get_object( builder, "entry_clmn2" );
	entry_clmn3 = ( GtkEntry* )gtk_builder_get_object( builder, "entry_clmn3" );
	
	// button for add entryes
	GObject *btn_add_const_entry = gtk_builder_get_object( builder, "do_some_btn" );
	g_signal_connect( btn_add_const_entry, "clicked", G_CALLBACK( add_entry_to_clmn ), store );
	
	// button to add entrys for column from file
	btn_add_entry_from_file = gtk_builder_get_object( builder, "btn_contant_from_file" );
	g_signal_connect( btn_add_entry_from_file, "clicked", G_CALLBACK( add_entry_from_file ), store );

	// button for clear store
	btn_clear_store = gtk_builder_get_object( builder, "btn_clear_column_view" );
	g_signal_connect( btn_clear_store, "clicked", G_CALLBACK( clear_store ), store );
	
	// quit button 
	GObject *q_btn = gtk_builder_get_object ( builder, "quit_button" );
	g_signal_connect_swapped ( q_btn, "clicked", G_CALLBACK(gtk_window_destroy), window );

//=====================================================================
	// move status msg inside a function
	if (Telnet_socket_init( &sock ) == -1)
		g_signal_emit_by_name( txtView_out, "insert-at-cursor", "error to create socket..." );
	else
		g_signal_emit_by_name( txtView_out, "insert-at-cursor", "create socket success..." );
	
	if (Telnet_socket_connect( &sock ) == -1)
		g_signal_emit_by_name( txtView_out, "insert-at-cursor", "error to connect socket..." );
	else
		g_signal_emit_by_name( txtView_out, "insert-at-cursor", "connect socket success..." );
	
	
	gtk_widget_grab_focus( GTK_WIDGET( entry_) );
	gtk_widget_set_visible( GTK_WIDGET (window), TRUE );

	/* We do not need the builder any more */
	g_object_unref( builder );
}

static void clear_store( GtkButton *btn, GListStore *store )
{
	g_list_store_remove_all( store );
}


static void add_entry_from_file( GtkButton *btn, GListStore *store )
{
	// open file inside the folder
	GFile *file;
	GError **error;
	char *content;
	gchar **str_content, **tag_split;
	
	file = g_file_new_for_path( "tst_file.txt" );
		
	// read full string from file
	g_file_load_contents( file, NULL, &content, NULL, NULL, error );	

	// split to line
	str_content = g_strsplit( content, "\n", 0 );
	//g_print(" full contant: [%s] \n", content);
	for(gsize i = 0; str_content[i] != NULL; i++)
	{
		
		//g_print("hex : [%x]\n", str_content[i]);
		
		tag_split = g_strsplit( str_content[i], " ", 0 );
		//for( gsize i = 0; tag_split[i] != NULL; i++){}
		//	g_print( "%s ", tag_split[i] );
		//g_print( "\n" );
		//g_print("%s\n", str_content[i]);
		g_list_store_append( store, my_object_new( tag_split[0], tag_split[1], tag_split[2] ) ); 
	}
	g_print("list contain %lu element\n", g_list_model_get_n_items( G_LIST_MODEL( store ) ) );

	// clear mem
	g_free( content );
	g_strfreev( str_content );
	g_strfreev( tag_split );
}

// to add some new string to textView
static void textView_signal_insert( GtkTextView *textView, char *txt_to_print )
{
	GtkTextBuffer *textview_buffer = gtk_text_view_get_buffer( txtView_out );
	
	GtkTextIter end_iter;
	gtk_text_buffer_get_end_iter(textview_buffer, &end_iter);
	
	gtk_text_buffer_insert(textview_buffer, &end_iter, txt_to_print, -1);
	gtk_text_buffer_insert(textview_buffer, &end_iter, "\n", -1);
}

static void btn_test_text_view ( GtkButton *btn, char *txt_to_print )
{
	
	g_signal_emit_by_name(txtView_out, "insert-at-cursor", "text_from_button_action");
 
}

static void set_text_view_string( GtkTextView *textView, guchar* str )
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textView);
	
	gtk_text_buffer_set_text (buffer, str, -1);

}
/*
static void parse_adc_dac_from_rtt_to_GList(char *rtt_buffer, GList *sensor_val)
{
	// parse info from rtt to 
	
	gchar **
}
*/

static void on_entry_activate(GtkEntry *entry, GListStore *store)
{
	// get text from entry
	GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer ( entry );
	const char *entry_buffer_text = gtk_entry_buffer_get_text ( entry_buffer );
	
	// get contant from entry
	//const char *direct_entry = gtk_editable_get_text( ( GtkEditable* )entry );
	
	// print cmd to "console"
	g_signal_emit_by_name(txtView_out, "insert-at-cursor", entry_buffer_text);
	//g_signal_emit_by_name(txtView_out, "insert-at-cursor", direct_entry);

	// choice cmd 
	// just send and receive answer	
	Telnet_socket_send_msg( &sock, entry_buffer_text );
	int recv_size;
	if ( ( recv_size = Telnet_socket_receive( &sock, recv_buffer, 1024 ) ) != -1 )	
		g_signal_emit_by_name(txtView_out, "insert-at-cursor", recv_buffer);
	
	// test console print
	g_print("receive [%d] from Rtt = [%s] \n", recv_size, recv_buffer ); 
	
	// make double split for parcing value from MCU (A newly-allocated array of strings, freed with g_strfreev().)
	if (recv_size == 25)
	{	
		gchar **fb_answer = g_strsplit( recv_buffer, "adc_get_val", 0 ); // check for NULL terminator return, fro check correct cmd answer 
		if ( fb_answer != NULL )
		{
			//expect correct answer- mean fb_answer[1] contain data
			gchar **fb_value = g_strsplit( fb_answer[1], "->", 0 );
			// expect correct value 
			
			const char *tmp_entry1 = gtk_editable_get_text( (GtkEditable*)entry_clmn1 ); // mb need copy
			g_list_store_append( store, my_object_new( tmp_entry1, fb_value[0], fb_value[1] ) );
			
			g_strfreev( fb_answer );
			g_strfreev( fb_value );
		}
	}
	// clear entry buffer after text copy
	gtk_entry_buffer_set_text ( entry_buffer, "", -1);

}

//===============================================CALL BACK function============
static void factory_setup_cb ( GtkListItemFactory *factory, GObject *list_item )
{
	GtkWidget *label = gtk_label_new( NULL );
	gtk_list_item_set_child( GTK_LIST_ITEM( list_item ), label );
	
}

/* The bind function for the other factories, corresponding to columns after
 * the primary key column.
 */

void factory_bind_cb_clmn1( GtkListItemFactory *factory, GObject *list_item )
{
	GtkWidget *label = gtk_list_item_get_child( GTK_LIST_ITEM( list_item ) );
	GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( list_item ) ); // take current item (instance of row )
	
	const char* item_contain_text = ( ( MyObject* )item )->clmn1;
	gtk_label_set_label( GTK_LABEL( label ), item_contain_text );
}

void factory_bind_cb_clmn2( GtkListItemFactory *factory, GObject *list_item )
{
	GtkWidget *label = gtk_list_item_get_child( GTK_LIST_ITEM( list_item ) );
	GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( list_item ) ); // take current item (instance of row )
	
	const char* item_contain_text = ( ( MyObject* )item )->clmn2;
	gtk_label_set_label( GTK_LABEL( label ), item_contain_text );
}

void factory_bind_cb_clmn3( GtkListItemFactory *factory, GObject *list_item )
{
	GtkWidget *label = gtk_list_item_get_child( GTK_LIST_ITEM( list_item ) );
	GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( list_item ) ); // take current item (instance of row )
	
	const char* item_contain_text = ( ( MyObject* )item )->clmn3;
	gtk_label_set_label( GTK_LABEL( label ), item_contain_text );
}

// create new entry from test field
static void add_entry_to_clmn (GtkButton *btn, GListStore *store)
{
	const char *char_buf[3];
	
	// create inside MyObject* and add it to tbl
	GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer( entry_clmn1 );
	char_buf[0] = gtk_entry_buffer_get_text( entry_buffer );
	
	entry_buffer = gtk_entry_get_buffer(entry_clmn2);
	char_buf[1] = gtk_entry_buffer_get_text( entry_buffer );
	
	entry_buffer = gtk_entry_get_buffer(entry_clmn3);
	char_buf[2] = gtk_entry_buffer_get_text( entry_buffer );	
	
	g_list_store_append( store, my_object_new( char_buf[0], char_buf[1], char_buf[2]) );	
}

//========================================================================