# flags
# gtk gcc flags
CFLAGS_GTK =$(shell pkg-config --cflags gtk4)
LFLAGS_GTK =$(shell pkg-config --libs gtk4)
FLAGS =$(-Wall -Wextra -Wpedantic -std=c11)

# build filename 
SRC_FILENAME=gtk_fileTree
#SRC_GLIB_FILENAME=glib_fileTree


# glib gcc flags
CFLAGS_GLIB=$(shell pkg-config --cflags glib-2.0)
LFLAGS_GLIB=$(shell pkg-config --libs glib-2.0)

build_gtk: $(SRC_FILENAME).c
	gcc $(CFLAGS_GTK) -o $(SRC_FILENAME) $(SRC_FILENAME).c $(FLAGS) $(LFLAGS_GTK)

#build_glib2.0: $(SRC_GLIB_FILENAME).c
#	gcc -g $(CFLAGS_GLIB) -o $(SRC_GLIB_FILENAME) $(SRC_GLIB_FILENAME).c $(LFLAGS_GLIB)

#build_all:$(SRC_FILENAME).c
#	gcc -g $(CFLAGS_GLIB) $(CFLAGS_GTK) -o $(SRC_FILENAME) $(SRC_FILENAME).c $(LFLAGS_GLIB) $(LFLAGS_GTK)

