# 
# le makefile pour see!
# 

# Compilateur et options
CC=g++
LD=g++
#CFLAGS=-Wall -c `sdl-config --cflags` -g 
CFLAGS=-Wall -c `sdl-config --cflags` -g -Wno-deprecated -O3 -I.
LDFLAGS=`sdl-config --libs` -lSDL_image -lSDL_ttf -lX11 -lGL -lGLU
LDFLAGSFUCK=-lboost_filesystem-mt #-gcc-d-1_33.1
#LDFLAGSFUCK=-lboost_filesystem #-gcc-d-1_33.1
#-lboost_filesystem-gcc-mt-1_31 -lboost_filesystem-gcc-mt-d-1_31
# -lboost_filesystem-gcc-1_31 

# Cible
TARGET=see

# objets à compiler
OBJ=descript.o fichier.o dir.o video.o core.o main.o

# Règles de base
all: $(TARGET)

tarball: clean
	tar cjvf ../see_`date +%Y_%m_%d`.tar.bz2 ../see/*.cpp ../see/*.h ../see/Makefile ../see/HOWTO ../see/TODO

install: 
	cp ./see /usr/local/bin/see
	[ -f /usr/local/bin/see.comment ] || ln -s /usr/local/bin/see /usr/local/bin/see.comment
	[ -f /usr/local/bin/see.emphase ] || ln -s /usr/local/bin/see /usr/local/bin/see.emphase
	[ -f /usr/local/bin/see.tablet  ] || ln -s /usr/local/bin/see /usr/local/bin/see.tablet
	mkdir -p /usr/share/fonts/corefonts/
	cp arialbd.ttf /usr/share/fonts/corefonts/
	
uninstall: 
	rm -f /usr/local/bin/see
clean:
	rm -f $(OBJ) $(TARGET)

# Linkage !
$(TARGET): $(OBJ) Makefile
	$(LD) $(LDFLAGS) $(OBJ) $(LDFLAGSFUCK) -o $(TARGET)

# Règle par défaut de compilation
.cpp.o .h.o:
	$(CC) $(CFLAGS) -c $< -o $@

# Dépendances 
main.o: core.o main.cpp
core.o: dir.o video.o core.h core.cpp
dir.o: fichier.o dir.h dir.cpp
video.o: fichier.o video.h video.cpp
fichier.o: fichier.h fichier.cpp
descript.o: descript.h descript.cpp
font.o: font.h font.cpp

#
# Repository shortcuts
#

push:
	git commit -a
	git push origin master

pop:
	#git fetch origin
	git pull origin master



