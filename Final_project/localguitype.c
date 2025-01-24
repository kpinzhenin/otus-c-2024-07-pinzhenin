#include "localguitype.h"

// struct _ here ??

G_DEFINE_TYPE (MyObject, my_object, G_TYPE_OBJECT )

// init user custom class
static void my_object_init(MyObject *self)
{
	//g_print("try to create instance \n");
	// do this for explanation, what this funct was do
	/*self->clmn1 = NULL;
	self->clmn2 = NULL;
	self->clmn3 = NULL; */
}

// registration some properties or class method and signals
static void my_object_class_init( MyObjectClass *class)
{
	//g_print("try to create object init \n");
}

MyObject* my_object_new(const char *clmn1_val, const char *clmn2_val, const char *clmn3_val)
{
	MyObject *item = g_object_new( MY_TYPE_OBJECT, NULL );
	
	item->clmn1 = g_strdup( clmn1_val );
	item->clmn2 = g_strdup( clmn2_val );
	item->clmn3 = g_strdup( clmn3_val );
	
	return item;
}

void add_item_to_table( GListStore *store, const char *distance, const char *adc_val)
{
	guint store_size;
	//take count of element
	store_size = g_list_model_get_n_items( G_LIST_MODEL( store ) );
	
	// for empty list - just add elemnt
	if ( store_size == 0 )
	{
		g_list_store_append( store, my_object_new( distance, adc_val, "" ) );
	}
	else
	{
		// for not empty list calc full coefficient
			
	}
	
}

static void take_a_b_arg_from_points ( point *p1, point *p2, double *a, double *b )
{
	/*
		from equtation of line ( y = ax + b ) 
	
		a = ( y2 - y1 ) / ( x2 - x1 )
		b = y - ax
	*/
	
	*a = ( p2->y - p1->y ) / ( p2->x - p1->x);
	*b = p2->y - (*a * p2->x);	
}

static double take_Y_dac( double *Y_adc, double *a_adc, double *b_adc, double *a_dac, double *b_dac)
{
	/*
		calc it from two line equtation:
		Yadc = a_adc * x_dis + b_adc; x_dis = ( Yadc - b_adc ) / a_adc;
		Ydac = a_dac * x_dis + b_dac; x_dis = ( Ydac - b_dac) / a_dac;
		
		that means:
		Ydac = ( Yadc - b_adc ) * a_dac / a_adc + b_dac;		
	*/
	
	return   (*Y_adc - *b_adc) * *a_dac / *a_adc + *b_dac;
}