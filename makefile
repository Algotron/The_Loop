.SILENT:

GRILLESDL=GrilleSDL
RESSOURCES=Ressources
ASTAR=AStar

CC = g++ -g -m64 -DSUN -DDEBUG -I$(GRILLESDL) -I$(RESSOURCES) -I$(ASTAR)
OBJS = $(ASTAR)/AStar.o $(GRILLESDL)/GrilleSDL.o $(RESSOURCES)/Ressources.o
PROGRAMS = TheLoop

ALL: $(PROGRAMS)

TheLoop:	TheLoop.c TheLoop.h $(OBJS)
	echo Creation de TheLoop...
	$(CC) TheLoop.c -o TheLoop $(OBJS) -lrt -lpthread -lSDL

$(GRILLESDL)/GrilleSDL.o:	$(GRILLESDL)/GrilleSDL.c $(GRILLESDL)/GrilleSDL.h
		echo Creation de GrilleSDL.o ...
		$(CC) -c $(GRILLESDL)/GrilleSDL.c
		mv GrilleSDL.o $(GRILLESDL)

$(RESSOURCES)/Ressources.o:	$(RESSOURCES)/Ressources.c $(RESSOURCES)/Ressources.h
		echo Creation de Ressources.o ...
		$(CC) -c $(RESSOURCES)/Ressources.c
		mv Ressources.o $(RESSOURCES)

$(ASTAR)/AStar.o:	$(ASTAR)/AStar.c $(ASTAR)/AStar.h
		echo Creation de AStar.o ...
		$(CC) -c $(ASTAR)/AStar.c
		mv AStar.o $(ASTAR)

clean:
	@rm -f $(OBJS) core

clobber:	clean
	@rm -f tags $(PROGRAMS)
