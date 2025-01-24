#ifndef _LOCAL_GUI_TYPE_H
#define _LOCAL_GUI_TYPE_H

//#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct
{
	double x;
	double y;
} point;

static void take_a_b_arg_from_points ( point *p1, point *p2, double *a, double *b );
static double take_Y_dac( double *Y_adc, double *a_adc, double *b_adc, double *a_dac, double *b_dac);

G_BEGIN_DECLS
//==================================================ExampleType
/*  template of "get_type" macros
	#define OURNAMESPACE_TYPE_OUROBJECT ournamespace_ourobject_get_type()
*/
#define MY_TYPE_OBJECT ( my_object_get_type() )

/* template of type definition macros
	G_DECARE_DERIVABLE_TYPE (NamespaceObject, namespace_object, NAMESPACE, OBJECT, ParentClass)
*/
G_DECLARE_FINAL_TYPE (MyObject, my_object, MY, OBJECT, GObject)

struct _MyObject
{
    GObject parent_instance; // Parent class
    char *clmn1;            // User field name
	char *clmn2;            
	char *clmn3;            
};

struct _MyObjectClass
{
    GObjectClass parent_class; // Родительский класс
};

MyObject* my_object_new(const char *clmn1_val, const char *clmn2_val, const char *clmn3_val);

G_END_DECLS

#endif